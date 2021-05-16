#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <deque>
#include <vector>
#include <cmath>
#include <math.h>
#include <WinBase.h>
#include <TlHelp32.h>
#include <iomanip>
#include <d3d9.h>
#include <d3dx9.h>
#include <experimental/filesystem>
#include <fstream>
#include <string_view>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include "..\single_include\nlohmann\json.hpp"
constexpr std::string_view offsets_url = "https://raw.githubusercontent.com/frk1/hazedumper/master/csgo.json";
using json = nlohmann::json;

struct CParsedOffsets
{
	std::ptrdiff_t dwClientState = NULL;
	std::ptrdiff_t dwLocalPlayer = NULL;
	std::ptrdiff_t dwGlowObjectManager = NULL;
	std::ptrdiff_t dwEntityList = NULL;
	std::ptrdiff_t dwGlobalVars = NULL;
	std::ptrdiff_t dwClientState_Map = NULL;
	std::ptrdiff_t dwClientState_MapDirectory = NULL;
	std::ptrdiff_t dwClientState_ViewAngles = NULL;
	std::ptrdiff_t dwViewMatrix = NULL;
	std::ptrdiff_t clientstate_delta_ticks = NULL;
	std::ptrdiff_t m_dwBoneMatrix = NULL;
	std::ptrdiff_t m_iHealth = NULL;
	std::ptrdiff_t m_iTeamNum = NULL;
	std::ptrdiff_t m_iGlowIndex = NULL;
	std::ptrdiff_t m_vecOrigin = NULL;
	std::ptrdiff_t m_vecViewOffset = NULL;
	std::ptrdiff_t m_hMyWeapons = NULL;
	std::ptrdiff_t m_hActiveWeapon = NULL;
	std::ptrdiff_t m_iItemDefinitionIndex = NULL;
	std::ptrdiff_t m_nFallbackPaintKit = NULL;
	std::ptrdiff_t m_iItemIDHigh = NULL;
	std::ptrdiff_t m_bDormant = NULL;
	std::ptrdiff_t m_bSpotted = NULL;
	std::ptrdiff_t m_aimPunchAngle = NULL;
	std::ptrdiff_t m_iShotsFired = NULL;
};
extern CParsedOffsets g_pParsedOffsets;

namespace ReasonGodWontLetMeIntoHeaven
{
	void ParseOffsets();
}