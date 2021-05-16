#include "Aimbot.h"

void C_Aimbot::Think()
{
	if (!gModules.bAimbot)
		return;

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		//ocasionally the aimbot will crash ( atleast i think its the aimbot ). 
		//idk doesnt happen often and couldnt be bothered to find out why.
		DWORD dwPlayer = GetBestPlayer();

		if (dwPlayer == NULL)
			return;

		if (dwPlayer == gAPI.dwLocalPlayer)
			return;

		bool IsDormant = g_pMemory->ReadA<bool>(dwPlayer + g_pParsedOffsets.m_bDormant);

		if (IsDormant)
			return;

		D3DXVECTOR3 vecLocalPunchAngles = g_pMemory->ReadA<D3DXVECTOR3>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_aimPunchAngle);
		int iShotsFired = g_pMemory->ReadA<int>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_iShotsFired);

		DWORD dwMatrixEnemy = g_pMemory->ReadA<DWORD>(dwPlayer + g_pParsedOffsets.m_dwBoneMatrix);
		matrix3x4 matrix = g_pMemory->ReadA<matrix3x4>(dwMatrixEnemy + 8 * 0x30);

		D3DXVECTOR3 vecEnemyHead = D3DXVECTOR3(matrix.matrix[0][3], matrix.matrix[1][3], matrix.matrix[2][3]);

		D3DXVECTOR3 vecAimAngle;
		VectorAngles((vecEnemyHead - gAPI.vecLocalEyePosition), vecAimAngle);
		ClampAngle(vecAimAngle);

		if (iShotsFired > 1)
			vecAimAngle -= vecLocalPunchAngles * 2.f;

		//g_pMemory->WriteA<D3DXVECTOR3>(gAPI.dwClientState + dwClientState_ViewAngles, vecAimAngle);

		D3DXVECTOR3 vecAngleDelta = vecAimAngle - gAPI.vecLocalViewAngles;
		ClampAngle(vecAngleDelta);

		/*D3DXVECTOR3 vecTransformedPos;
		WorldToScreen(vecEnemyHead, vecTransformedPos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		INPUT Input = { 0 };
		Input.type = INPUT_MOUSE;

		Input.mi.dx = (LONG)(vecTransformedPos.x - (GetSystemMetrics(SM_CXSCREEN) / 2) + (vecLocalPunchAngles.y * 2.f)) / 2;
		Input.mi.dy = (LONG)(vecTransformedPos.y - (GetSystemMetrics(SM_CYSCREEN) / 2) - (vecLocalPunchAngles.x * 2.f)) / 2;

		// set move cursor directly
		Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

		SendInput(1, &Input, sizeof(INPUT));*/

		g_pMemory->WriteA<D3DXVECTOR3>(gAPI.dwClientState + g_pParsedOffsets.dwClientState_ViewAngles, gAPI.vecLocalViewAngles + vecAngleDelta / gModules.iAimbotSmooth);
	}
}

DWORD C_Aimbot::GetBestPlayer()
{
	DWORD dwBest = NULL;
	float flDistToBest = 99999.f;

	for (int i = 1; i <= gAPI.pGlobalVars.maxClients; i++)
	{
		DWORD dwPlayer = g_pMemory->ReadA<DWORD>(gAPI.dwClientModule + g_pParsedOffsets.dwEntityList + i * 0x10);

		if (!dwPlayer || dwPlayer == gAPI.dwLocalPlayer)
			continue;

		int iTeam = g_pMemory->ReadA<int>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_iTeamNum);
		int iEnemyTeam = g_pMemory->ReadA<int>(dwPlayer + g_pParsedOffsets.m_iTeamNum);
		int iEnemyHealth = g_pMemory->ReadA<int>(dwPlayer + g_pParsedOffsets.m_iHealth);
		bool bEnemyDormant = g_pMemory->ReadA<bool>(dwPlayer + g_pParsedOffsets.m_bDormant);

		if (iEnemyTeam == iTeam || iEnemyHealth <= 0 || bEnemyDormant)
			continue;

		DWORD dwMatrixEnemy = g_pMemory->ReadA<DWORD>(dwPlayer + g_pParsedOffsets.m_dwBoneMatrix);
		matrix3x4 matrix = g_pMemory->ReadA<matrix3x4>(dwMatrixEnemy + 8 * 0x30);
		D3DXVECTOR3 vecEnemyHead = D3DXVECTOR3(matrix.matrix[0][3], matrix.matrix[1][3], matrix.matrix[2][3]);

		//hacky.
		rn::vector3 vector1 = rn::vector3(gAPI.vecLocalEyePosition.x, gAPI.vecLocalEyePosition.y, gAPI.vecLocalEyePosition.z);
		rn::vector3 vector2 = rn::vector3(vecEnemyHead.x, vecEnemyHead.y, vecEnemyHead.z);

		if (!CanHit(vector1, vector2))
			continue;

		//lol i dont even understand half of this math but who cares.
		float flFOV = GetFOV(gAPI.vecLocalViewAngles, gAPI.vecLocalEyePosition, vecEnemyHead);

		if (flFOV < flDistToBest && flFOV < gModules.flAimbotFov)
		{
			flDistToBest = flFOV;
			dwBest = dwPlayer;
		}
	}


	return dwBest;
}

bool C_Aimbot::CanHit(rn::vector3& origin, rn::vector3& dest)
{
	bool parsed = g_pBSP->load_map({ directory.begin(), directory.end() }, gAPI.szCurrentMapDirectory);
	return parsed ? g_pBSP->is_visible(origin, dest) : false;
}