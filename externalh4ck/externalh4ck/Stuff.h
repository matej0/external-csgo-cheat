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
#include "valve-bsp-parser/bsp_parser.hpp"
#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data

/*
there is a lot of hacky shit in this source. when i started, everything was fairly clean and nice but as time went on i cared less and less.
i tried to use hungarian notation as much as possible but some namings were kept intentionally lowercase to be consistent
with the json library and the bsp parser which arent mine ( credits to niels lohmann for json library and ReactiioN for bsp parsing ).
there are examples where the naming is kinda questionable.
the reason i stopped caring was because once i was done i wouldnt open the source code again and i didnt really plan on releasing it.
*/

extern rn::bsp_parser* g_pBSP;
#define M_PI 3.14159265359
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( M_PI ) ) )
constexpr std::string_view directory = "E:\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\csgo";
//ancient mem class from uc. thanks to whoever posted it.
class CMemory
{
private:
	HANDLE m_hProcess; // the HANDLE to the process to attach
	DWORD m_dwProcessId; // the Process id of the process to attach
	std::vector<MODULEENTRY32> m_Modules; // std::vector containing all the modules we grab from the process
public:

	bool Attach(const std::string& strProcessName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (hSnapshot == INVALID_HANDLE_VALUE) return false;


		PROCESSENTRY32 ProcEntry;
		ProcEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapshot, &ProcEntry))
		{
			if (!strcmp(ProcEntry.szExeFile, strProcessName.c_str()))
			{
				CloseHandle(hSnapshot);
				m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
				m_dwProcessId = ProcEntry.th32ProcessID;
				return true;
			}
		}
		else
		{
			CloseHandle(hSnapshot);
			return false;
		}

		while (Process32Next(hSnapshot, &ProcEntry))
		{
			if (!strcmp(ProcEntry.szExeFile, strProcessName.c_str()))
			{
				m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
				m_dwProcessId = ProcEntry.th32ProcessID;

				return true;
			}
		}

		// if we are here it means that the process has not been found or that there are no processes to scan for anymore
		// we can close the snapshot handle and return false.
		CloseHandle(hSnapshot);
		return false;
	}

	bool GrabModule(const std::string& strModuleName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);

		if (hSnapshot == INVALID_HANDLE_VALUE) return false;

		MODULEENTRY32 ModEntry;
		ModEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnapshot, &ModEntry))
		{
			if (!strcmp(ModEntry.szModule, strModuleName.c_str()))
			{
				CloseHandle(hSnapshot);
				m_Modules.push_back(ModEntry);
				return true;
			}
		}
		else
		{
			CloseHandle(hSnapshot);
			return false;
		}

		while (Module32Next(hSnapshot, &ModEntry))
		{
			if (!strcmp(ModEntry.szModule, strModuleName.c_str()))
			{
				CloseHandle(hSnapshot);
				m_Modules.push_back(ModEntry); 
				return true;
			}
		}
		CloseHandle(hSnapshot);
		return false;
	}

	// default constructor: wont attach to any process
	CMemory()
	{
		// init members
		m_hProcess = INVALID_HANDLE_VALUE;
		m_dwProcessId = 0;
		// just for safety
		m_Modules.clear();
	}
	// this constructor will attach to a specific process (default CS:GO)
	CMemory(const std::string& strProcessName = "csgo.exe")
	{
		// init members
		m_hProcess = INVALID_HANDLE_VALUE;
		m_dwProcessId = 0;
		m_Modules.clear();

		// attach and throw if the function failed so we can manage the fail
		if (!Attach(strProcessName))
			throw;
	}

	//found this shit somewhere on UC, thought it was an issue with rpm wpm wrappers when making glow with the struct, turns out my padding was wrong ( i tried to pad out the size
	//of the entity pointer as i thought it was useless externally but i decided to include it when it didnt seem to work.
	template<typename T>
	inline T ReadA(std::uintptr_t ptrAddress) const
	{
		static_assert(std::is_trivially_copyable<T>::value, "Invalid RPM/WPM type");

		T val = T();

		ReadProcessMemory(m_hProcess, (LPCVOID*)ptrAddress, &val, sizeof(T), NULL);
		return val;
	}

	template<typename T>
	inline bool WriteA(std::uintptr_t ptrAddress, T value) const
	{
		static_assert(std::is_trivially_copyable<T>::value, "Invalid RPM/WPM type");
		return WriteProcessMemory(m_hProcess, (LPVOID*)ptrAddress, &value, sizeof(T), NULL);
	}

	template<typename T>
	inline bool Read(DWORD dwAddress, T& Value)
	{
		return ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), &Value, sizeof(T), NULL);
	}

	template<typename T>
	inline bool Write(DWORD dwAddress, T Value) const
	{
		return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), &Value, sizeof(T), NULL);
	}

	// getters
	HANDLE GetHandle() { return m_hProcess; }
	DWORD GetProcId() { return m_dwProcessId; }
	std::vector<MODULEENTRY32> GetModules() { return m_Modules; }
};
extern CMemory* g_pMemory;

struct CKeybinds
{
	bool bGlow = false;
	bool bAimbot = false;
	bool bRCS = false;
	bool bRadar = false;
	bool bSkinchanger = false;
	float flAimbotFov = 0.7f;
	float iAimbotSmooth = 10.f;
};
extern CKeybinds gModules;

typedef struct
{
	float matrix[3][4];
} matrix3x4;

enum ItemDefinitionIndex
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5_SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

struct GlobalVarsBase
{
	float realtime;
	int framecount;
	float absoluteframetime;
	float absoluteframestarttimestddev;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

struct CAPI
{
	DWORD dwClientModule = NULL;
	DWORD dwEngineModule = NULL;
	DWORD dwClientState = NULL;
	DWORD dwLocalPlayer = NULL;
	DWORD dwEntityList = NULL;
	D3DXMATRIX dwLocalViewMatrix;
	int iGlowIndex = NULL;
	char szCurrentMap[256];
	char szCurrentMapDirectory[256];
	D3DXVECTOR3 vecLocalOrigin;
	D3DXVECTOR3 vecLocalEyePosition; 
	D3DXVECTOR3 vecLocalViewAngles; 
	GlobalVarsBase pGlobalVars;
};
extern CAPI gAPI;

inline void ClampAngle(D3DXVECTOR3& qaAng)
{
	while (qaAng.x > 89)
		qaAng.x -= 180;

	while (qaAng.x < -89)
		qaAng.x += 180;

	while (qaAng.y > 180)
		qaAng.y -= 360;

	while (qaAng.y < -180)
		qaAng.y += 360;

	while (qaAng.z != 0)
		qaAng.z = 0;
}

inline void MakeVector(D3DXVECTOR3 angle, D3DXVECTOR3& vector)
{
	float pitch, yaw, tmp;
	pitch = float(angle[0] * M_PI / 180);
	yaw = float(angle[1] * M_PI / 180);
	tmp = float(cos(pitch));
	vector[0] = float(-tmp * -cos(yaw));
	vector[1] = float(sin(yaw)*tmp);
	vector[2] = float(-sin(pitch));
}

inline void VectorAngles(const float *forward, float *angles)
{
	float    tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

inline float GetFOV(D3DXVECTOR3 angle, D3DXVECTOR3 src, D3DXVECTOR3 dst)
{
	//hacky stone age shit that can be found in every cheat ever made (literally)
	D3DXVECTOR3 ang, aim;
	float mag, u_dot_v;
	VectorAngles((dst - src), ang);
	ClampAngle(ang);

	MakeVector(angle, aim);
	MakeVector(ang, ang);

	mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
	u_dot_v = (aim.x * ang.x + aim.y * ang.y + aim.z * ang.z);

	float result = RAD2DEG(acos(u_dot_v / (pow(mag, 2))));

	if (!isfinite(result) || isinf(result) || isnan(result))
		result = 0.0f;

	return result;
}

static bool ScreenTransform(const D3DXVECTOR3& point, D3DXVECTOR3& screen)
{
	const D3DXMATRIX& w2sMatrix = gAPI.dwLocalViewMatrix;
	screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
	screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
	screen.z = 0.0f;

	float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}

static bool WorldToScreen(const D3DXVECTOR3 &origin, D3DXVECTOR3 &screen, int w, int h)
{
	if (!ScreenTransform(origin, screen)) {
		screen.x = (w / 2.0f) + (screen.x * w) / 2;
		screen.y = (h / 2.0f) - (screen.y * h) / 2;

		return true;
	}
	return false;
}

struct GlowObjectDefinition
{
	void*              m_pEntity;				//0000
	float              m_flGlowRed;				//0004
	float              m_flGlowGreen;			//0008
	float              m_flGlowBlue;			//000C
	float              m_flGlowAlpha;			//0010
	unsigned char      unk0[0x10];				//0014 - padding stuff like this works but idk
	bool               m_bRenderWhenOccluded;	//0024
	bool               m_bRenderWhenUnoccluded;	//0025
	unsigned char      unk2[0x12];				//0026
};									

