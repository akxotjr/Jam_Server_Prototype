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

Room::Room()
{
	_roomId = IdManager::Instance().Generate(IdType::Room);
	_physicsQueue = std::make_shared<core::thread::LockQueue<job::Job>>();
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

	_pxRewindScene = PhysicsManager::Instance().CreatePxScene();
	ASSERT_CRASH(_pxRewindScene != nullptr)

	PhysicsManager::Instance().AddPhysicsQueue(_roomId, _physicsQueue);

	// temp
	shared_ptr<Floor> floor = MakeShared<Floor>();
	AddActor(floor);

	_physicsQueue->Push(job::Job([this]()
		{
			_pxControllerManager = PxCreateControllerManager(*_pxScene);
			ASSERT_CRASH(_pxControllerManager != nullptr)
		}));
}

void Room::Update()
{
	if (!_isReady) return;

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
			for (auto& player : _players | views::values)
			{
				player->Update();
			}
		}));

	CaptureSnapshot();

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

bool Room::Enter(PlayerRef player)
{
	WRITE_LOCK

	if (_players.size() > MAX_ADMISSION)
		return false;

	player->Init(GetRoomRef());
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
		//*info->mutable_transform() = actor->GetTransform();
		info->set_allocated_transform(actor->GetTransform());
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		//*info->mutable_transform() = player->GetTransform();
		info->set_allocated_transform(player->GetTransform());
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
		WRITE_LOCK

		for (auto& [id, actor] : _actors)
		{
			SnapshotEntity entity;
			entity.actorId = id;
			entity.position = actor->GetPosition();
			entity.rotation = actor->GetRotation();
			entity.collider = actor->GetColliderInfo();

			snapshot.push_back(entity);
		}
	}

	AddSnapshot(timestamp, snapshot);
}

void Room::AddSnapshot(double timestamp, Snapshot& snapshot)
{
	WRITE_LOCK

	_snapshotBuffer.push_back(make_pair(timestamp, snapshot));

	double cutoff = timestamp - 0.5f;

	_snapshotBuffer.erase(
		std::remove_if(_snapshotBuffer.begin(), _snapshotBuffer.end(),
			[cutoff](const std::pair<double, Snapshot>& pair) {
				return pair.first < cutoff;
			}),
		_snapshotBuffer.end());
}



Snapshot* Room::FindSnapshot(double timestamp)
{
	READ_LOCK

	auto it = std::lower_bound(
		_snapshotBuffer.begin(), _snapshotBuffer.end(), timestamp,
		[](const auto& pair, int t) { return pair.first < t; });

	if (it == _snapshotBuffer.begin() || it == _snapshotBuffer.end())
		return nullptr;

	return &it->second;
}

physx::PxScene* Room::BuildRewindScene(Snapshot& snapshot)
{
	for (auto& entity : snapshot)
	{
		physx::PxRigidStatic* actor = PhysicsManager::Instance().CreateRigidStatic(entity.position, entity.rotation);
		physx::PxShape* shape = PhysicsManager::Instance().CreateShape(entity.collider);
		shape->setLocalPose(physx::PxTransform(entity.collider.localOffset, entity.collider.localRotation));

		actor->attachShape(*shape);
		_pxRewindScene->addActor(*actor);
	}

	return _pxRewindScene;
}

void Room::ClearRewindScene()
{
	physx::PxU32 actorCount = _pxRewindScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
	if (actorCount == 0)
		return;

	std::vector<physx::PxActor*> actors(actorCount);
	_pxRewindScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);

	for (auto actor : actors)
		actor->release();
}



