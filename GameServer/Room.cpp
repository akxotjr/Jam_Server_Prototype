#include "pch.h"
#include "Room.h"
#include "Player.h"
#include <TimeManager.h>
#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "SessionManager.h"
#include "GameUdpSession.h"
#include "Values.h"
#include "PhysicsManager.h"
#include "Floor.h"
#include "Bot.h"

Room::Room()
{
	_roomId = IdManager::Instance().Generate(IdType::Room);
	_physicsQueue = std::make_shared<core::thread::LockQueue<job::Job>>();

	_playerColors = {
		physx::PxVec3(1.0f, 0.0f, 0.0f),
		physx::PxVec3(1.0f, 1.0f, 0.0f),
		physx::PxVec3(0.0f, 1.0f, 0.0f),
		physx::PxVec3(0.0f, 0.0f, 1.0f)
	};
}

Room::~Room()
{
	PhysicsManager::Instance().RemovePhysicsQueue(_roomId);

	if (_pxScene)
		_pxScene->release();
}

void Room::Init()
{
	_pxScene = PhysicsManager::Instance().CreatePxScene();
	ASSERT_CRASH(_pxScene != nullptr)

	for (int32 i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		_pxRewindScenes[i] = PhysicsManager::Instance().CreatePxScene();
		ASSERT_CRASH(_pxRewindScenes[i] != nullptr)
	}

	PhysicsManager::Instance().AddPhysicsQueue(_roomId, _physicsQueue);

	_physicsQueue->Push(job::Job([this]()
		{
			_pxControllerManager = PxCreateControllerManager(*_pxScene);
			ASSERT_CRASH(_pxControllerManager != nullptr)
		}));

	// temp
	FloorRef floor = MakeShared<Floor>();
	AddActor(floor);

	BotRef bot = MakeShared<Bot>();
	AddBotOrMonster(bot);
}

void Room::Update()
{
	if (!_isReady) return;

	CaptureSnapshot();

	_physicsQueue->Push(job::Job([this]()
		{
			_pxScene->simulate(static_cast<float>(TICK_INTERVAL_S));
		}));

	_physicsQueue->Push(job::Job([this]()
		{
			_pxScene->fetchResults(true);
		}));

	_physicsQueue->Push(job::Job([this]()
		{
			for (auto& character : _botAndMonsters | views::values)
			{
				character->Update();
			}
		}));

	_physicsQueue->Push(job::Job([this]()
		{
			for (auto& player : _players | views::values)
			{
				player->Update();
			}
		}));

	MulticastSyncActor();
}

void Room::AddActor(ActorRef actor)
{
	actor->Init(GetRoomRef());

	_physicsQueue->Push(job::Job([this, actor]()
		{
			printf("Adding actor to scene: %u\n", actor->GetActorId());
			_pxScene->addActor(*actor->GetPxActor());
		}));

	_actors.insert({ actor->GetActorId(), actor });
}

void Room::RemoveActor(ActorRef actor)
{
	_physicsQueue->Push(job::Job([this, actor]()
		{
			_pxScene->removeActor(*actor->GetPxActor());
		}));

	_actors.erase(actor->GetActorId());
}

void Room::AddBotOrMonster(CharacterActorRef character)
{
	WRITE_LOCK

	character->Init(GetRoomRef());
	_botAndMonsters[character->GetActorId()] = character;
}

void Room::RemoveBotOrMonster(CharacterActorRef character)
{
	WRITE_LOCK
	_botAndMonsters.erase(character->GetActorId());
}

bool Room::Enter(PlayerRef player)
{
	WRITE_LOCK

	if (_players.size() > MAX_PLAYERS_PER_ROOM)
		return false;

	player->Init(GetRoomRef());
	player->SetPlayerIndex(static_cast<int32>(_players.size()));
	player->SetColor(_playerColors[player->GetPlayerIndex()]);
	_players[player->GetUserId()] = player;

	return true;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK
	_players.erase(player->GetUserId());
}

void Room::Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable, double timestamp)
{
	for (auto& player : _players | views::values)
	{
		uint32 userId = player->GetUserId();

		auto session = SessionManager::Instance().GetSessionByUserId(type, userId);
		if (!session) continue;

		if (type == ProtocolType::PROTOCOL_UDP && session->IsUdp() && reliable)
		{
			auto udpSession = static_pointer_cast<ReliableUdpSession>(session);
			udpSession->SendReliable(sendBuffer, timestamp);
		}
		else  // tcp or unreliable udp 
		{
			session->Send(sendBuffer);
		}
	}
}

void Room::MulticastSpawnActor()
{
	Protocol::S_SPAWN_ACTOR spawnPkt;
	double timestamp = TimeManager::Instance().GetServerTime();
	for (auto& actor : _actors | views::values)
	{
		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(actor->GetActorId());
		info->set_allocated_transform(actor->GetTransform());

		physx::PxVec3 color = actor->GetColor();
		info->set_r(color.x);
		info->set_g(color.y);
		info->set_b(color.z);
	}

	//temp
	for (auto& character : _botAndMonsters | views::values)
	{
		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(character->GetActorId());
		info->set_allocated_transform(character->GetTransform());

		physx::PxVec3 color = character->GetColor();
		info->set_r(color.x);
		info->set_g(color.y);
		info->set_b(color.z);
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		info->set_allocated_transform(player->GetTransform());

		physx::PxVec3 color = player->GetColor();
		info->set_r(color.x);
		info->set_g(color.y);
		info->set_b(color.z);
	}

	for (auto& player : _players | views::values)	// todo
	{
		spawnPkt.set_playeractorid(player->GetActorId());
		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(spawnPkt);

		auto session = static_pointer_cast<GameUdpSession>(SessionManager::Instance().GetSessionByUserId(ProtocolType::PROTOCOL_UDP, player->GetUserId()));
		session->SendReliable(sendBuffer, timestamp);
	}

	std::cout << "[UDP] Multicast : S_SPAWN_ACTOR\n";
}

void Room::MulticastSyncActor()
{
	Protocol::S_SYNC_ACTOR syncPkt;

	double timestamp = TimeManager::Instance().GetServerTime();
	syncPkt.set_timestamp(timestamp);

	for (auto& actor : _actors | views::values)
	{
		uint32 actorId = actor->GetActorId();
		if (IdManager::Instance().GetActorType(actorId) == ActorTypePrefix::Floor)
			continue;

		auto info = syncPkt.add_actorinfo();
		info->set_id(actorId);
		info->set_allocated_transform(actor->GetTransform());
	}

	//temp
	for (auto& character : _botAndMonsters | views::values)
	{
		Protocol::ActorInfo* info = syncPkt.add_actorinfo();
		info->set_id(character->GetActorId());
		info->set_allocated_transform(character->GetTransform());

		physx::PxVec3 color = character->GetColor();
		info->set_r(color.x);
		info->set_g(color.y);
		info->set_b(color.z);
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = syncPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		info->set_allocated_transform(player->GetTransform());
		info->set_sequence(player->GetLastSequence());
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(syncPkt);
	Multicast(ProtocolType::PROTOCOL_UDP, sendBuffer, true, timestamp);
}

PlayerRef Room::GetPlayerByUserId(uint32 userId)
{
	auto it = _players.find(userId);
	if (it != _players.end())
		return it->second;

	return nullptr;
}

Vector<uint32> Room::GetPlayerList()
{
	Vector<uint32> playerList;

	for (const auto& id : _players | views::keys)
	{
		playerList.push_back(id);
	}

	return playerList;
}

void Room::CaptureSnapshot()
{
	double timestamp = TimeManager::Instance().GetServerTime();
	Snapshot snapshot;

	{

		for (auto& [actorid, actor] : _actors)
		{
			SnapshotEntity entity;
			entity.actorId = actorid;
			entity.position = actor->GetPosition();
			entity.rotation = actor->GetRotation();
			entity.collider = actor->GetColliderInfo();
			entity.runtimeActor = actor.get();
			snapshot.push_back(entity);
		}

		for (auto& [actorid, character] : _botAndMonsters)
		{
			SnapshotEntity entity;
			entity.actorId = actorid;
			entity.position = character->GetPosition();
			entity.rotation = character->GetRotation();
			entity.collider = character->GetColliderInfo();
			entity.runtimeActor = character.get();
			snapshot.push_back(entity);
		}

		for (auto& player : _players | views::values)
		{
			SnapshotEntity entity;
			entity.actorId = player->GetActorId();
			entity.position = player->GetPosition();
			entity.rotation = player->GetRotation();
			entity.collider = player->GetColliderInfo();
			entity.runtimeActor = player.get();
			snapshot.push_back(entity);
		}
	}

	AddSnapshot(timestamp, snapshot);
}

void Room::AddSnapshot(double timestamp, Snapshot& snapshot)
{
	WRITE_LOCK

	auto it = std::upper_bound(
		_snapshotBuffer.begin(), _snapshotBuffer.end(), timestamp,
		[](double t, const std::pair<double, Snapshot>& pair) {
			return t < pair.first;
		});

	_snapshotBuffer.insert(it, make_pair(timestamp, snapshot));

	double cutoff = timestamp - 0.5f;
	_snapshotBuffer.erase(
		std::remove_if(_snapshotBuffer.begin(), _snapshotBuffer.end(),
			[cutoff](const auto& pair) {
				return pair.first < cutoff;
			}),
		_snapshotBuffer.end());

	//if (!_snapshotBuffer.empty())
	//{
	//	cout << "SnapshotBuffer Size = " << _snapshotBuffer.size() << endl;
	//	cout << "SnapshotBuffer Begin timestamp = " << _snapshotBuffer.begin()->first << endl;
	//}
}




Snapshot* Room::FindSnapshot(double timestamp)
{
	READ_LOCK

	auto it = std::lower_bound(
		_snapshotBuffer.begin(), _snapshotBuffer.end(), timestamp,
		[](const auto& pair, double t) { return pair.first < t; });

	cout << "param timestamp = " << timestamp << " , SnapshotBuffer begin timestamp = " << _snapshotBuffer.front().first << " , SnapshotBuffer begin timestamp = " << _snapshotBuffer.back().first << endl;

	if (it == _snapshotBuffer.end())
		return nullptr;

	return &it->second;
}

physx::PxScene* Room::BuildRewindScene(int32 playerIndex, Snapshot& snapshot)
{
	WRITE_LOCK
	if (!_pxRewindScenes[playerIndex]) 
		return nullptr;

	for (auto& entity : snapshot)
	{
		if (entity.collider.type == ColliderInfo::Type::Plane)
		{
			physx::PxRigidStatic* ground = PhysicsManager::Instance().CreatePlane(0.0f, 1.0f, 0.0f, 0.0f);
			ground->userData = entity.runtimeActor;
			_pxRewindScenes[playerIndex]->addActor(*ground);
			continue;
		}

		physx::PxRigidStatic* actor = PhysicsManager::Instance().CreateRigidStatic(entity.position, entity.rotation);
		actor->userData = entity.runtimeActor;

		physx::PxShape* shape = PhysicsManager::Instance().CreateShape(entity.collider);
		shape->setLocalPose(physx::PxTransform(entity.collider.localOffset, entity.collider.localRotation));
		actor->attachShape(*shape);
		_pxRewindScenes[playerIndex]->addActor(*actor);
	}

	return _pxRewindScenes[playerIndex];
}

void Room::ClearRewindScene(int32 playerIndex)
{
	physx::PxU32 actorCount = _pxRewindScenes[playerIndex]->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
	if (actorCount == 0)
		return;

	std::vector<physx::PxActor*> actors(actorCount);
	_pxRewindScenes[playerIndex]->getActors(physx::PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);

	for (auto actor : actors)
		actor->release();
}



