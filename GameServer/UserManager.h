#pragma once


class UserManager
{
	DECLARE_SINGLETON(UserManager)

public:
	void	Init();

	uint64	GetUserId(string id, string pw);
};

