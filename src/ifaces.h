/**
 *  ifaces.h
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#pragma once

#if defined(WIN32)
#define strtoull _strtoui64
#endif

#include "stdafx.h"

#define CLIENT_DLL
#define VERSION_SAFE_STEAM_API_INTERFACES

#include "tier3/tier3.h"
#include "steam/steam_api.h"
#include "cdll_int.h"
#include "icliententitylist.h"
#include "vgui/IVGui.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "igameresources.h"
#include "filesystem.h"

#ifdef _POSIX
#define GetFuncAddress(pAddress, szFunction) dlsym(pAddress, szFunction)
#define GetHandleOfModule(szModuleName) dlopen(szModuleName".so", RTLD_NOLOAD)
#else
#define GetFuncAddress(pAddress, szFunction) ::GetProcAddress((HMODULE)pAddress, szFunction)
#define GetHandleOfModule(szModuleName) GetModuleHandleA(szModuleName".dll")
#endif

class Interfaces {
	public:
		static bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
		static void Unload();
		static IBaseClientDLL* pClientDLL;
		static IClientEntityList* pClientEntityList;
		static IVEngineClient* pEngineClient;
		static CSteamAPIContext* pSteamAPIContext; 
		static IGameResources* GetGameResources();
	private:
		static CDllDemandLoader *pClientModule;
};