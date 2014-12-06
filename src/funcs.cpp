/*
 *  funcs.cpp
 *  StatusSpec project
 *
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "funcs.h"

void StatusSpecUnloader::ReadyToUnload(SourceHook::Plugin plug) {};

SourceHook::Impl::CSourceHookImpl g_SourceHook;
SourceHook::ISourceHook *g_SHPtr = &g_SourceHook;
int g_PLID = 0;

SH_DECL_MANUALHOOK5_void(C_TFPlayer_CalcView, OFFSET_CALCVIEW, 0, 0, Vector &, QAngle &, float &, float &, float &);
SH_DECL_MANUALHOOK0(C_TFPlayer_GetFOV, OFFSET_GETFOV, 0, 0, float);
SH_DECL_MANUALHOOK3_void(C_TFPlayer_GetGlowEffectColor, OFFSET_GETGLOWEFFECTCOLOR, 0, 0, float *, float *, float *);
SH_DECL_MANUALHOOK0(C_TFPlayer_GetHealth, OFFSET_GETHEALTH, 0, 0, int);
SH_DECL_MANUALHOOK0(C_TFPlayer_GetMaxHealth, OFFSET_GETMAXHEALTH, 0, 0, int);
SH_DECL_MANUALHOOK0(C_TFPlayer_GetObserverMode, OFFSET_GETOBSERVERMODE, 0, 0, int);
SH_DECL_MANUALHOOK0(C_TFPlayer_GetObserverTarget, OFFSET_GETOBSERVERTARGET, 0, 0, C_BaseEntity *);
SH_DECL_HOOK1_void(IBaseClientDLL, FrameStageNotify, SH_NOATTRIB, 0, ClientFrameStage_t);
SH_DECL_HOOK1(IClientMode, DoPostScreenSpaceEffects, SH_NOATTRIB, 0, bool, const CViewSetup *);
SH_DECL_HOOK1(IGameEventManager2, FireEventClientSide, SH_NOATTRIB, 0, bool, IGameEvent *);
SH_DECL_HOOK4(IMaterialSystem, FindMaterial, SH_NOATTRIB, 0, IMaterial *, char const *, const char *, bool, const char *);
SH_DECL_HOOK3_void(IPanel, SendMessage, SH_NOATTRIB, 0, VPANEL, KeyValues *, VPANEL);
SH_DECL_HOOK3_void(IPanel, SetPos, SH_NOATTRIB, 0, VPANEL, int, int);
SH_DECL_HOOK2(IVEngineClient, GetPlayerInfo, SH_NOATTRIB, 0, bool, int, player_info_t *);

inline bool DataCompare(const BYTE* pData, const BYTE* bSig, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bSig)
	{
		if (*szMask == 'x' && *pData != *bSig)
			return false;
	}

	return (*szMask) == NULL;
}

inline DWORD FindPattern(DWORD dwAddress, DWORD dwSize, BYTE* pbSig, const char* szMask)
{
	for (DWORD i = NULL; i < dwSize; i++)
	{
		if (DataCompare((BYTE*)(dwAddress + i), pbSig, szMask))
			return (DWORD)(dwAddress + i);
	}

	return 0;
}

inline GLPI_t GetGLPIFunc() {
#if defined _WIN32
	static DWORD pointer = NULL;
	if (!pointer)
		pointer = FindPattern((DWORD)GetHandleOfModule(_T("client")), CLIENT_MODULE_SIZE, (PBYTE)GETLOCALPLAYERINDEX_SIG, GETLOCALPLAYERINDEX_MASK);
	return (GLPI_t)(pointer);
#else
	return nullptr;
#endif
}

inline SMI_t GetSMIFunc() {
#if defined _WIN32
	static DWORD pointer = NULL;
	if (!pointer)
		pointer = FindPattern((DWORD)GetHandleOfModule(_T("client")), CLIENT_MODULE_SIZE, (PBYTE)SETMODELINDEX_SIG, SETMODELINDEX_MASK);
	return (SMI_t)(pointer);
#else
	return nullptr;
#endif
}

inline SMP_t GetSMPFunc() {
#if defined _WIN32
	static DWORD pointer = NULL;
	if (!pointer)
		pointer = FindPattern((DWORD)GetHandleOfModule(_T("client")), CLIENT_MODULE_SIZE, (PBYTE)SETMODELPOINTER_SIG, SETMODELPOINTER_MASK);
	return (SMP_t)(pointer);
#else
	return nullptr;
#endif
}

inline SPT_t GetSPTFunc() {
#if defined _WIN32
	static DWORD pointer = NULL;
	if (!pointer)
		pointer = FindPattern((DWORD)GetHandleOfModule(_T("client")), CLIENT_MODULE_SIZE, (PBYTE)SETPRIMARYTARGET_SIG, SETPRIMARYTARGET_MASK);
	return (SPT_t)(pointer);
#else
	return nullptr;
#endif
}

int Funcs::setModelLastHookRegistered = 0;
std::map<int, std::function<void(C_BaseEntity *, const model_t *&)>> Funcs::setModelHooks;

GLPI_t Funcs::getLocalPlayerIndexOriginal = nullptr;
SMI_t Funcs::setModelIndexOriginal = nullptr;
SMP_t Funcs::setModelPointerOriginal = nullptr;

bool Funcs::AddDetour(void *target, void *detour, void *&original) {
	MH_STATUS addHookResult = MH_CreateHook(target, detour, &original);

	if (addHookResult != MH_OK) {
		return false;
	}

	MH_STATUS enableHookResult = MH_EnableHook(target);

	return (enableHookResult == MH_OK);
}

bool Funcs::AddDetour_GetLocalPlayerIndex(GLPI_t detour) {
	void *original;

	if (AddDetour(GetGLPIFunc(), detour, original)) {
		getLocalPlayerIndexOriginal = reinterpret_cast<GLPI_t>(original);
		return true;
	}

	return false;
}

bool Funcs::AddDetour_C_BaseEntity_SetModelIndex(SMIH_t detour) {
	void *original;

	if (AddDetour(GetSMIFunc(), detour, original)) {
		setModelIndexOriginal = reinterpret_cast<SMI_t>(original);
		return true;
	}

	return false;
}

bool Funcs::AddDetour_C_BaseEntity_SetModelPointer(SMPH_t detour) {
	void *original;

	if (AddDetour(GetSMPFunc(), detour, original)) {
		setModelPointerOriginal = reinterpret_cast<SMP_t>(original);
		return true;
	}

	return false;
}

int Funcs::AddGlobalHook_C_TFPlayer_GetFOV(C_TFPlayer *instance, fastdelegate::FastDelegate0<float> hook, bool post) {
	return SH_ADD_MANUALHOOK(C_TFPlayer_GetFOV, instance, hook, post);
}

int Funcs::AddHook_C_BaseEntity_SetModel(std::function<void(C_BaseEntity *, const model_t *&)> hook) {
	setModelHooks[++setModelLastHookRegistered] = hook;

	if (setModelHooks.size() > 0) {
		AddDetour_C_BaseEntity_SetModelIndex(Detour_C_BaseEntity_SetModelIndex);
		AddDetour_C_BaseEntity_SetModelPointer(Detour_C_BaseEntity_SetModelPointer);
	}

	return setModelLastHookRegistered;
}

int Funcs::AddHook_IBaseClientDLL_FrameStageNotify(IBaseClientDLL *instance, fastdelegate::FastDelegate1<ClientFrameStage_t> hook, bool post) {
	return SH_ADD_HOOK(IBaseClientDLL, FrameStageNotify, instance, hook, post);
}

int Funcs::AddHook_IClientMode_DoPostScreenSpaceEffects(IClientMode *instance, fastdelegate::FastDelegate1<const CViewSetup *, bool> hook, bool post) {
	return SH_ADD_HOOK(IClientMode, DoPostScreenSpaceEffects, instance, hook, post);
}

int Funcs::AddHook_IGameEventManager2_FireEventClientSide(IGameEventManager2 *instance, fastdelegate::FastDelegate1<IGameEvent *, bool> hook, bool post) {
	return SH_ADD_HOOK(IGameEventManager2, FireEventClientSide, instance, hook, post);
}

int Funcs::AddHook_IMaterialSystem_FindMaterial(IMaterialSystem *instance, fastdelegate::FastDelegate4<char const *, const char *, bool, const char *, IMaterial *> hook, bool post) {
	return SH_ADD_HOOK(IMaterialSystem, FindMaterial, instance, hook, post);
}

int Funcs::AddHook_IPanel_SendMessage(vgui::IPanel *instance, fastdelegate::FastDelegate3<vgui::VPANEL, KeyValues *, vgui::VPANEL> hook, bool post) {
	return SH_ADD_HOOK(IPanel, SendMessage, instance, hook, post);
}

int Funcs::AddHook_IPanel_SetPos(vgui::IPanel *instance, fastdelegate::FastDelegate3<vgui::VPANEL, int, int> hook, bool post) {
	return SH_ADD_HOOK(IPanel, SetPos, instance, hook, post);
}

int Funcs::AddHook_IVEngineClient_GetPlayerInfo(IVEngineClient *instance, fastdelegate::FastDelegate2<int, player_info_t *, bool> hook, bool post) {
	return SH_ADD_HOOK(IVEngineClient, GetPlayerInfo, instance, hook, post);
}

int Funcs::CallFunc_GetLocalPlayerIndex() {
	if (getLocalPlayerIndexOriginal) {
		return getLocalPlayerIndexOriginal();
	}
	else {
		return GetGLPIFunc()();
	}
}

void Funcs::CallFunc_C_BaseEntity_SetModelIndex(C_BaseEntity *instance, int index) {
	if (setModelIndexOriginal) {
		setModelIndexOriginal(instance, index);
	}
	else {
		GetSMIFunc()(instance, index);
	}
}

void Funcs::CallFunc_C_BaseEntity_SetModelPointer(C_BaseEntity *instance, const model_t *pModel) {
	if (setModelPointerOriginal) {
		setModelPointerOriginal(instance, pModel);
	}
	else {
		GetSMPFunc()(instance, pModel);
	}
}

void Funcs::CallFunc_C_HLTVCamera_SetPrimaryTarget(C_HLTVCamera *instance, int nEntity) {
	GetSPTFunc()(instance, nEntity);
}

float Funcs::CallFunc_C_TFPlayer_GetFOV(C_TFPlayer *instance) {
	return SH_MCALL(instance, C_TFPlayer_GetFOV)();
}

void Funcs::CallFunc_C_TFPlayer_GetGlowEffectColor(C_TFPlayer *instance, float *r, float *g, float *b) {
	SH_MCALL(instance, C_TFPlayer_GetGlowEffectColor)(r, g, b);
}

int Funcs::CallFunc_C_TFPlayer_GetHealth(C_TFPlayer *instance) {
	return SH_MCALL(instance, C_TFPlayer_GetHealth)();
}

int Funcs::CallFunc_C_TFPlayer_GetMaxHealth(C_TFPlayer *instance) {
	return SH_MCALL(instance, C_TFPlayer_GetMaxHealth)();
}

int Funcs::CallFunc_C_TFPlayer_GetObserverMode(C_TFPlayer *instance) {
	return SH_MCALL(instance, C_TFPlayer_GetObserverMode)();
}

C_BaseEntity *Funcs::CallFunc_C_TFPlayer_GetObserverTarget(C_TFPlayer *instance) {
	return SH_MCALL(instance, C_TFPlayer_GetObserverTarget)();
}

bool Funcs::CallFunc_IVEngineClient_GetPlayerInfo(IVEngineClient *instance, int ent_num, player_info_t *pinfo) {
	return SH_CALL(instance, &IVEngineClient::GetPlayerInfo)(ent_num, pinfo);
}

void Funcs::Detour_C_BaseEntity_SetModelIndex(C_BaseEntity *instance, void *, int index) {
	const model_t *model = Interfaces::pModelInfoClient->GetModel(index);

	for (auto iterator = setModelHooks.begin(); iterator != setModelHooks.end(); ++iterator) {
		iterator->second(instance, model);
	}

	int newIndex = Interfaces::pModelInfoClient->GetModelIndex(Interfaces::pModelInfoClient->GetModelName(model));

	Funcs::CallFunc_C_BaseEntity_SetModelIndex(instance, newIndex);
}

void Funcs::Detour_C_BaseEntity_SetModelPointer(C_BaseEntity *instance, void *, const model_t *pModel) {
	for (auto iterator = setModelHooks.begin(); iterator != setModelHooks.end(); ++iterator) {
		iterator->second(instance, pModel);
	}

	Funcs::CallFunc_C_BaseEntity_SetModelPointer(instance, pModel);
}

bool Funcs::RemoveDetour_GetLocalPlayerIndex() {
	if (RemoveDetour(GetGLPIFunc())) {
		getLocalPlayerIndexOriginal = nullptr;
		return true;
	}

	return false;
}

bool Funcs::RemoveDetour_C_BaseEntity_SetModelIndex() {
	if (RemoveDetour(GetSMIFunc())) {
		setModelIndexOriginal = nullptr;
		return true;
	}

	return false;
}

bool Funcs::RemoveDetour_C_BaseEntity_SetModelPointer() {
	if (RemoveDetour(GetSMPFunc())) {
		setModelPointerOriginal = nullptr;
		return true;
	}

	return false;
}

bool Funcs::RemoveDetour(void *target) {
	MH_STATUS disableHookResult = MH_DisableHook(target);

	if (disableHookResult != MH_OK) {
		return false;
	}

	MH_STATUS removeHookResult = MH_RemoveHook(target);

	return (removeHookResult == MH_OK);
}

bool Funcs::RemoveHook(int hookID) {
	return SH_REMOVE_HOOK_ID(hookID);
}

void Funcs::RemoveHook_C_BaseEntity_SetModel(int hookID) {
	setModelHooks.erase(hookID);

	if (setModelHooks.size() == 0) {
		RemoveDetour_C_BaseEntity_SetModelIndex();
		RemoveDetour_C_BaseEntity_SetModelPointer();
	}
}

bool Funcs::Load() {
	MH_STATUS minHookResult = MH_Initialize();

	return (minHookResult == MH_OK || minHookResult == MH_ERROR_ALREADY_INITIALIZED);
}

bool Funcs::Unload() {
	g_SourceHook.UnloadPlugin(g_PLID, new StatusSpecUnloader());
	MH_STATUS minHookResult = MH_Uninitialize();

	return (minHookResult == MH_OK || minHookResult == MH_ERROR_NOT_INITIALIZED);
}

bool Funcs::Pause() {
	g_SourceHook.PausePlugin(g_PLID);
	MH_STATUS minHookResult = MH_DisableHook(MH_ALL_HOOKS);

	return (minHookResult == MH_OK || minHookResult == MH_ERROR_DISABLED);
}

bool Funcs::Unpause() {
	g_SourceHook.UnpausePlugin(g_PLID);
	MH_STATUS minHookResult = MH_EnableHook(MH_ALL_HOOKS);

	return (minHookResult == MH_OK || minHookResult == MH_ERROR_ENABLED);
}