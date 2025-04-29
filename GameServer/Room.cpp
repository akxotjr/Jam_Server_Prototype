#include "pch.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "Room.h"
#include "Character.h"
#include "Player.h"
#include <TimeManager.h>
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "SessionManager.h"

Room::Room()
{
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

	MulticastActorSync();
}

void Room::AddActor(ActorRef actor)
{
	actor->Init(GetRoomRef());

	_scene->addActor(*actor->GetPxActor());
	_actors.insert({ actor->GetId(), actor });
}

void Room::RemoveActor(ActorRef actor)
{
	_scene->removeActor(*actor->GetPxActor());
	_actors.erase(actor->GetId());
}

void Room::Enter(PlayerRef player)
{
	WRITE_LOCK
	_players[player->GetInfo()->id()] = player;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK
	_players.erase(player->GetInfo()->id());
}

void Room::Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable)
{
	for (auto& player : _players | views::values)
	{
		uint32 userId = player->GetUserId();

		auto session = SessionManager::Instance().GetSessionByUserId(type, userId);

		if (type == ProtocolType::PROTOCOL_TCP && session->IsTcp())
		{
			session->Send(sendBuffer);
		}
		else if (type == ProtocolType::PROTOCOL_UDP && session->IsUdp())
		{
			auto udpSession = static_pointer_cast<ReliableUdpSession>(session);
			if (reliable)
			{
				double timestamp = TimeManager::Instance().GetServerTime();
				udpSession->SendReliable(sendBuffer, timestamp);
			}
			else
			{
				udpSession->Send(sendBuffer);
			}
		}
	}
}

void Room::MulticastActorSync()
{
	
}

//void Room::AddCharacter(CharacterRef character)
//{
//	WRITE_LOCK
//	_characters[character->GetInfo()->id()] = character;
//}
//
//void Room::RemoveCharacter(CharacterRef character)
//{
//	WRITE_LOCK
//	_characters.erase(character->GetInfo()->id());
//}

void Room::BroadCastCharacterSync()
{
	double timestamp = TimeManager::Instance().GetServerTime();

	for (auto& [id, player] : _players)
	{
		auto gameTcpSession = player->GetOwnerSession();
		auto gameUdpSession = static_pointer_cast<GameUdpSession>(GSessionManager.GetSessionById(ProtocolType::PROTOCOL_UDP, gameTcpSession->GetId()));

		if (gameUdpSession == nullptr) 
			return;

		Protocol::S_CHARACTER_SYNC pkt;

		pkt.set_timestamp(timestamp);

		for (auto& [i, character] : _characters)
		{
			Protocol::CharacterInfo* info = pkt.add_characterinfo();
			info->CopyFrom(*character->GetInfo()); 
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(pkt);
		gameUdpSession->SendReliable(sendBuffer, timestamp);
	}
}

void Room::BroadcastSpawnActor()
{
	for (auto& [id, player] : _players)
	{
		if (!player)
		{
			continue;
		}

		auto tcpSession = player->GetOwnerSession();
		auto udpSession = static_pointer_cast<GameUdpSession>(GSessionManager.GetSessionById(ProtocolType::PROTOCOL_UDP, tcpSession->GetId()));


		Protocol::S_SPAWN_ACTOR pkt;

		uint32 id = player->GetInfo()->id();
		pkt.set_playerid(id);

		double timestamp = TimeManager::Instance().GetServerTime();

		for (auto& [i, character] : _characters)
		{
			Protocol::CharacterInfo* info = pkt.add_characterinfo();
			info->CopyFrom(*character->GetInfo());
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(pkt);
		udpSession->SendReliable(sendBuffer, timestamp);
	}
	std::cout << "[UDP] Broadcast : S_SPAWN_ACTOR\n";
}



