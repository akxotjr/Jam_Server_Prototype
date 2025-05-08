#include "pch.h"
#include "UserManager.h"

uint32 UserManager::Login(string id, string pw)
{

    static uint32 userId = 1;  // DB에 저장되있는 userId 를 가져왔다고 가정


    return userId++; //temp
}
