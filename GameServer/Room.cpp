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

	PhysicsManager::Instance().AddPhysicsQueue(_roomId, _physicsQueue);

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

	MulticastSyncActor();
}

void Room::AddActor(ActorRef actor)
{
	actor->Init(GetRoomRef());

	_physicsQueue->Push(job::Job([this, actor]()
		{
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
		auto info = syncPkt.add_actorinfo();
		info->set_id(actor->GetActorId());
		//*info->mutable_transform() = actor->GetTransform();
		info->set_allocated_transform(actor->GetTransform());
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = syncPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		info->set_allocated_transform(player->GetTransform());
		//*info->mutable_transform() = player->GetTransform();
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



