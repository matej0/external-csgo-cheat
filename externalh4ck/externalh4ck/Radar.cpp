#include "Radar.h"

void C_Radar::Think(int nIndex)
{
	if (!gModules.bRadar)
		return;

	DWORD dwPlayer = g_pMemory->ReadA<DWORD>(gAPI.dwClientModule + g_pParsedOffsets.dwEntityList + nIndex * 0x10);

	if (!dwPlayer || !gAPI.dwLocalPlayer)
		return;

	if (dwPlayer == gAPI.dwLocalPlayer)
		return;

	bool bIsDormant = g_pMemory->ReadA<bool>(dwPlayer + g_pParsedOffsets.m_bDormant);

	if (bIsDormant)
		return;

	g_pMemory->WriteA<bool>(dwPlayer + g_pParsedOffsets.m_bSpotted, true);
}