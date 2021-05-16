#include "Stuff.h"
#include "DontLookAtMe.h"
#include "valve-bsp-parser/bsp_parser.hpp"
#include "Aimbot.h"
#include "Radar.h"
#include "Glow.h"
#include "SkinChanger.h"

CMemory* g_pMemory;
rn::bsp_parser* g_pBSP;
C_Aimbot g_Aimbot;
C_Radar g_Radar;
C_Glow g_Glow;
C_SkinChanger g_SkinChanger;
CKeybinds gModules;
CAPI gAPI;
CParsedOffsets g_pParsedOffsets;

void KeybindLogic()
{
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		gModules.bAimbot = !gModules.bAimbot;
		printf("Aimbot: %s\n", gModules.bAimbot ? "ON" : "OFF"); //i always tell people not to use c functions in c++ and then i do it, couldnt be bothered to type this shit out with cout
		Beep(gModules.bAimbot ? 500 : 300, 120); //simple beep, lets you to know if feature is on or off, prolly dont even need to print this crap to console.
	}

	if (GetAsyncKeyState(VK_F2) & 1)
	{
		gModules.bGlow = !gModules.bGlow;
		printf("Glow: %s\n", gModules.bGlow ? "ON" : "OFF");
		Beep(gModules.bGlow ? 500 : 300, 120);
	}

	if (GetAsyncKeyState(VK_F3) & 1)
	{
		gModules.bRadar = !gModules.bRadar;
		printf("Radar: %s\n", gModules.bRadar ? "ON" : "OFF");
		Beep(gModules.bRadar ? 500 : 300, 120);
	}

	if (GetAsyncKeyState(VK_F6) & 1)
	{
		gModules.bSkinchanger = !gModules.bSkinchanger;
		printf("Skinchanger: %s\n", gModules.bSkinchanger ? "ON" : "OFF");
		Beep(gModules.bSkinchanger ? 500 : 300, 120);
	}

	if (GetAsyncKeyState(VK_LEFT) & 1)
	{
		std::cout << "Aimbot FOV: ";
		std::cin >> gModules.flAimbotFov;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		std::cout << "Aimbot smoothing: ";
		std::cin >> gModules.iAimbotSmooth;
	}
}

int main()
{
	ReasonGodWontLetMeIntoHeaven::ParseOffsets();

	g_pMemory = new CMemory("csgo.exe");
	g_pBSP = new rn::bsp_parser(); //register the bsp parser, thanks ReactiioN or however u spell it

	if (!g_pMemory->GrabModule("client.dll"))
	{
		std::cout << "Failed to find client.dll." << std::endl;
		return 0;
	}

	if (!g_pMemory->GrabModule("engine.dll"))
	{
		std::cout << "Failed to find engine.dll." << std::endl;
		return 0;
	}
	
	for (UINT i = 0; i < g_pMemory->GetModules().size(); i++)
	{
		//i hate strcmp but this is called only once so whatevz
		if (!strcmp(g_pMemory->GetModules().at(i).szModule, "client.dll"))
		{
			std::cout << "Found client.\n";
			gAPI.dwClientModule = reinterpret_cast<DWORD>(g_pMemory->GetModules().at(i).modBaseAddr);
		}
		else if (!strcmp(g_pMemory->GetModules().at(i).szModule, "engine.dll"))
		{
			std::cout << "Found engine.\n";
			gAPI.dwEngineModule = reinterpret_cast<DWORD>(g_pMemory->GetModules().at(i).modBaseAddr);
		}
	}

	for (;;)
	{
		KeybindLogic();

		if (gAPI.dwClientModule && gAPI.dwEngineModule)
		{
			g_pMemory->Read<GlobalVarsBase>(gAPI.dwEngineModule + g_pParsedOffsets.dwGlobalVars, gAPI.pGlobalVars);
			g_pMemory->Read<DWORD>(gAPI.dwClientModule + g_pParsedOffsets.dwLocalPlayer, gAPI.dwLocalPlayer);
			g_pMemory->Read<DWORD>(gAPI.dwEngineModule + g_pParsedOffsets.dwClientState, gAPI.dwClientState);

			if (gAPI.dwClientState)
			{
				g_pMemory->Read<char[256]>(gAPI.dwClientState + g_pParsedOffsets.dwClientState_Map, gAPI.szCurrentMap);
				g_pMemory->Read<char[256]>(gAPI.dwClientState + g_pParsedOffsets.dwClientState_MapDirectory, gAPI.szCurrentMapDirectory);

				if (gAPI.dwLocalPlayer)
				{
					g_pMemory->Read<D3DXMATRIX>(gAPI.dwClientModule + g_pParsedOffsets.dwViewMatrix, gAPI.dwLocalViewMatrix);
					g_pMemory->Read<D3DXVECTOR3>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_vecOrigin, gAPI.vecLocalOrigin);
					gAPI.vecLocalEyePosition = gAPI.vecLocalOrigin + g_pMemory->ReadA<D3DXVECTOR3>(gAPI.dwLocalPlayer + g_pParsedOffsets.m_vecViewOffset);
					g_pMemory->Read<D3DXVECTOR3>(gAPI.dwClientState + g_pParsedOffsets.dwClientState_ViewAngles, gAPI.vecLocalViewAngles);

					for (int i = 1; i <= gAPI.pGlobalVars.maxClients; i++)
					{
						g_Radar.Think(i);
						g_Glow.Think(i);
					}

					g_SkinChanger.Think();
					g_Aimbot.Think();
				}
			}
		}

		Sleep(10);
	}

	CloseHandle(g_pMemory->GetHandle());
	delete g_pMemory;
	delete g_pBSP;
	return 0;
}

/*g_pMemory->Write<float>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0x4), 255);
g_pMemory->Write<float>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0x8), 255);
g_pMemory->Write<float>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0xC), 0);
g_pMemory->Write<float>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0x10), 100);
g_pMemory->Write<bool>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0x24), true);
g_pMemory->Write<bool>(gAPI.dwGlowObjectManager + (gAPI.iGlowIndex * 0x38 + 0x5), false);*/