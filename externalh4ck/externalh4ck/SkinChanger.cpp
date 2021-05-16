#include "SkinChanger.h"


//how 2 use this skinchanger:
//enable it on spawn, if you want to apply a skin to your primary, toss out your secondary then pick it up later. 
//if the skin doesnt apply after a few seconds give up and try again later XD


void C_SkinChanger::Think()
{
	if (!gModules.bSkinchanger)
		return;

	//fullupdate
	if (GetAsyncKeyState(VK_F7) & 1)
	{
		FullUpdate();
	}

	if (!gAPI.dwLocalPlayer)
		return;

	DWORD dwActiveWeaponHandle = g_pMemory->ReadA<DWORD>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_hActiveWeapon);
	int iActiveWeapon = dwActiveWeaponHandle & 0xFFF;
	DWORD pWeapon = g_pMemory->ReadA<DWORD>(gAPI.dwClientModule + g_pParsedOffsets.dwEntityList + (iActiveWeapon - 1) * 0x10);

	if (!pWeapon)
		return;

	short siItemDefinitionIndex = g_pMemory->ReadA<short>(pWeapon + g_pParsedOffsets.m_iItemDefinitionIndex);
	int nFallbackPaintKit = g_pMemory->ReadA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit);
	g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_iItemIDHigh, -1);

	if (siItemDefinitionIndex == WEAPON_DEAGLE)
	{
		if (nFallbackPaintKit != 962)
		{
			g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit, 962);
			FullUpdate();
		}
	}
	else if (siItemDefinitionIndex == WEAPON_AK47)
	{
		if (nFallbackPaintKit != 1035) //801 = asiimov, 1035 = black slate
		{
			g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit, 1035);
			FullUpdate();
		}
	}
	else if (siItemDefinitionIndex == WEAPON_AWP)
	{
		if (nFallbackPaintKit != 279)
		{
			g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit, 279);
			FullUpdate();
		}
	}
	else if (siItemDefinitionIndex == WEAPON_GLOCK)
	{
		if (nFallbackPaintKit != 38)
		{
			g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit, 38);
			FullUpdate();
		}
	}
	else if (siItemDefinitionIndex == WEAPON_USP_SILENCER)
	{
		if (nFallbackPaintKit != 313)
		{
			g_pMemory->WriteA<int>(pWeapon + g_pParsedOffsets.m_nFallbackPaintKit, 313);
			FullUpdate();
		}
	}
}

void C_SkinChanger::FullUpdate()
{
	g_pMemory->WriteA<int>(gAPI.dwClientState + g_pParsedOffsets.clientstate_delta_ticks, -1);
}