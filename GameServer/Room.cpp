#include "pch.h"
#include "Room.h"
#include "Player.h"
#include <TimeManager.h>
#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "RoomManager.h"
#include "SessionManager.h"

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
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);	// TODO
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	_scene = physics->createScene(sceneDesc);
	if (_scene == nullptr)
		CRASH("PxScene is nullptr.")
}

void Room::Update()
{
	float deltaTime = static_cast<float>(TimeManager::Instance().GetDeltaTime());
	_scene->simulate(deltaTime);

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

	_players[player->GetActorId()] = player;

	return true;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK
	_players.erase(player->GetActorId());
}

void Room::Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable, double timestamp)
{
	for (auto& player : _players | views::values)
	{
		uint32 userId = player->GetUserId();

		auto session = SessionManager::Instance().GetSessionByUserId(type, userId);

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

	for (auto& actor : _actors | views::values)
	{
		if (actor->IsDirty()) continue;

		Protocol::ActorInfo* info = spawnPkt.add_actorinfo();
		info->set_id(actor->GetActorId());
		*info->mutable_transform() = actor->ToTransform();
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(spawnPkt);
	Multicast(ProtocolType::PROTOCOL_UDP, sendBuffer, true);
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

	auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(syncPkt);
	Multicast(ProtocolType::PROTOCOL_UDP, sendBuffer, true, timestamp);
}



