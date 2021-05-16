#pragma once
#include "Stuff.h"
#include "DontLookAtMe.h"
class C_Aimbot
{
public:
	DWORD GetBestPlayer();
	void Think();
	bool CanHit(rn::vector3& origin, rn::vector3& dest);
	
};
extern C_Aimbot g_Aimbot;