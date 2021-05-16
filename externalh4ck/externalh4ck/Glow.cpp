#include "Glow.h"

void C_Glow::Think(int nIndex)
{
	if (!gModules.bGlow)
		return;

	DWORD dwPlayer = g_pMemory->ReadA<DWORD>(gAPI.dwClientModule + g_pParsedOffsets.dwEntityList + nIndex * 0x10);

	if (!dwPlayer || !gAPI.dwLocalPlayer)
		return;

	if (dwPlayer == gAPI.dwLocalPlayer)
		return;

	int iEnemyTeam = g_pMemory->ReadA<int>(dwPlayer + g_pParsedOffsets.m_iTeamNum);
	int iTeam = g_pMemory->ReadA<int>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_iTeamNum);
	bool bDormant = g_pMemory->ReadA<bool>(dwPlayer + g_pParsedOffsets.m_bDormant);
	int iEnemyHealth = g_pMemory->ReadA<int>(dwPlayer + g_pParsedOffsets.m_iHealth);

	if (iEnemyTeam == iTeam)
		return;

	if (iEnemyHealth <= 0)
		return;

	if (bDormant)
		return;

	g_pMemory->Read<int>(dwPlayer + g_pParsedOffsets.m_iGlowIndex, gAPI.iGlowIndex);
	int GlowObjectManager = g_pMemory->ReadA<int>(gAPI.dwClientModule + g_pParsedOffsets.dwGlowObjectManager);
	GlowObjectDefinition Glow = g_pMemory->ReadA<GlowObjectDefinition>(GlowObjectManager + (gAPI.iGlowIndex * 0x38));

	Glow.m_flGlowRed = 1.f;
	Glow.m_flGlowGreen = 1.f;
	Glow.m_flGlowBlue = 0.f;
	Glow.m_flGlowAlpha = 0.5f;
	Glow.m_bRenderWhenOccluded = true;
	Glow.m_bRenderWhenUnoccluded = false;

	g_pMemory->WriteA<GlowObjectDefinition>(GlowObjectManager + (gAPI.iGlowIndex * 0x38), Glow);
}