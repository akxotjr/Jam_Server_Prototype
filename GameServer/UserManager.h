#pragma once


class UserManager
{
	DECLARE_SINGLETON(UserManager)

public:
	void	Init();

	uint32	Login(string id, string pw);	// return userId
	bool	RegisterAccount();

	uint64	GetUserId(string id, string pw);
};

