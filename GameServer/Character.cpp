#include "pch.h"
#include "Character.h"
#include "IdManager.h"

Character::Character()
{
	_info = new Protocol::CharacterInfo();
}

Character::~Character()
{
	//delete _info;
}

void Character::Init()
{
	Super::Init();
	{
		WRITE_LOCK
		_velocity = _direction * _speed;

		_info->set_id(GIdManager.Generate(IdType::Actor));
		_info->set_name("character" + to_string(_info->id()));
	
		Protocol::CharacterMovementInfo* movementInfo = new Protocol::CharacterMovementInfo();
		movementInfo->set_positionx(_position.x);
		movementInfo->set_positiony(_position.y);
		movementInfo->set_velocityx(_velocity.x);
		movementInfo->set_velocityy(_velocity.y);
		movementInfo->set_speed(_speed);   

		_info->set_allocated_movementinfo(movementInfo);
	}

}

void Character::Update()
{
	Super::Update();
}

void Character::SetInfo(Protocol::CharacterInfo* info)
{
	_info = info;

	_position = Vec2(info->movementinfo().positionx(), info->movementinfo().positiony());
	_velocity = Vec2(info->movementinfo().velocityx(), info->movementinfo().velocityy());
}

Protocol::CharacterInfo* Character::GetInfo()
{
	auto movementInfo = _info->mutable_movementinfo();

	movementInfo->set_positionx(_position.x);
	movementInfo->set_positiony(_position.y);
	movementInfo->set_velocityx(_velocity.x);
	movementInfo->set_velocityy(_velocity.y);

	return _info;
}


void Character::SendSyncToServer()
{

}
