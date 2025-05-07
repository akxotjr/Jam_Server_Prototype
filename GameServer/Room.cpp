#include "pch.h"
#include "Room.h"
#include "Player.h"
#include <TimeManager.h>
#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "RoomManager.h"
#include "SessionManager.h"
#include "GameUdpSession.h"
#include "Values.h"

Room::Room()
{
	_roomId = IdManager::Instance().Generate(IdType::Room);
}

Room::~Room()
{
	_scene->release();
	_scene = nullptr;
}

void Room::Init()
{
	physx::PxPhysics* physics = RoomManager::Instance().GetPxPhysics();

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f); // TODO gravity
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);	// TODO
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	_scene = physics->createScene(sceneDesc);
	ASSERT_CRASH(_scene != nullptr)

	_controllerManager = PxCreateControllerManager(*_scene);
	ASSERT_CRASH(_controllerManager != nullptr)
}

void Room::Update()
{
	//float deltaTime = static_cast<float>(TimeManager::Instance().GetDeltaTime());
	//_scene->simulate(deltaTime);

	_scene->simulate(static_cast<float>(TICK_INTERVAL_S));
	_scene->fetchResults(true);	// temp

	for (auto& player : _players | views::values)
	{
		player->Update();
	}

	MulticastSyncActor();
}

void Room::AddActor(ActorRef actor)
{
	actor->Init(GetRoomRef());

	_scene->addActor(*actor->GetPxActor());
	_actors.insert({ actor->GetActorId(), actor });
}

void Room::RemoveActor(ActorRef actor)
{
	_scene->removeActor(*actor->GetPxActor());
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
		*info->mutable_transform() = actor->ToTransform();
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		*info->mutable_transform() = player->ToTransform();
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
		*info->mutable_transform() = actor->ToTransform();
	}

	for (auto& player : _players | views::values)
	{
		Protocol::ActorInfo* info = syncPkt.add_actorinfo();
		info->set_id(player->GetActorId());
		*info->mutable_transform() = player->ToTransform();
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



