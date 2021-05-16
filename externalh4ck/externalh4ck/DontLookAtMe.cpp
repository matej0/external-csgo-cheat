#include "DontLookAtMe.h"
//dumb fucking c++ and its goddamn fucking linker errors fucking fuck fuck fuck.

void ReasonGodWontLetMeIntoHeaven::ParseOffsets()
{
	std::experimental::filesystem::path executable_path = std::experimental::filesystem::current_path() / "csgo.json";
	std::experimental::filesystem::remove(executable_path);

	HRESULT lResult = URLDownloadToFileA(NULL, std::string(offsets_url).c_str(), std::string(executable_path.string()).c_str(), NULL, NULL);
	printf("Downloading...\n");

	if (lResult != S_OK)
	{
		printf("Something went wrong!\n");
		return;
	}

	printf("Finished!\n");
	Sleep(300);
	system("cls");

	std::ifstream stream("csgo.json");
	json offsets = json::parse(stream);
	json signatures = offsets["signatures"];
	json netvars = offsets["netvars"];

	g_pParsedOffsets.dwClientState = *signatures.find("dwClientState");
	g_pParsedOffsets.dwLocalPlayer = *signatures.find("dwLocalPlayer");
	g_pParsedOffsets.dwGlowObjectManager = *signatures.find("dwGlowObjectManager");
	g_pParsedOffsets.dwEntityList = *signatures.find("dwEntityList");
	g_pParsedOffsets.dwGlobalVars = *signatures.find("dwGlobalVars");
	g_pParsedOffsets.dwClientState_Map = *signatures.find("dwClientState_Map");
	g_pParsedOffsets.dwClientState_MapDirectory = *signatures.find("dwClientState_MapDirectory");
	g_pParsedOffsets.dwClientState_ViewAngles = *signatures.find("dwClientState_ViewAngles");
	g_pParsedOffsets.dwViewMatrix = *signatures.find("dwViewMatrix");
	g_pParsedOffsets.clientstate_delta_ticks = *signatures.find("clientstate_delta_ticks");

	g_pParsedOffsets.m_dwBoneMatrix = *netvars.find("m_dwBoneMatrix");
	g_pParsedOffsets.m_iHealth = *netvars.find("m_iHealth");
	g_pParsedOffsets.m_iTeamNum = *netvars.find("m_iTeamNum");
	g_pParsedOffsets.m_iGlowIndex = *netvars.find("m_iGlowIndex");
	g_pParsedOffsets.m_vecOrigin = *netvars.find("m_vecOrigin");
	g_pParsedOffsets.m_vecViewOffset = *netvars.find("m_vecViewOffset");
	g_pParsedOffsets.m_hMyWeapons = *netvars.find("m_hMyWeapons");
	g_pParsedOffsets.m_hActiveWeapon = *netvars.find("m_hActiveWeapon");
	g_pParsedOffsets.m_iItemDefinitionIndex = *netvars.find("m_iItemDefinitionIndex");
	g_pParsedOffsets.m_nFallbackPaintKit = *netvars.find("m_nFallbackPaintKit");
	g_pParsedOffsets.m_iItemIDHigh = *netvars.find("m_iItemIDHigh");
	g_pParsedOffsets.m_bDormant = *signatures.find("m_bDormant");
	g_pParsedOffsets.m_bSpotted = *netvars.find("m_bSpotted");
	g_pParsedOffsets.m_aimPunchAngle = *netvars.find("m_aimPunchAngle");
	g_pParsedOffsets.m_iShotsFired = *netvars.find("m_iShotsFired");

	std::cout << "LocalPlayer: " << g_pParsedOffsets.dwLocalPlayer << std::endl;
	std::cout << "ClientState: " << g_pParsedOffsets.dwClientState << std::endl;

	stream.close();
	std::experimental::filesystem::remove(executable_path);
}
