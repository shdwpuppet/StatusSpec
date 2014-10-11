/*
*  fovoverride.cpp
*  StatusSpec project
*
*  Copyright (c) 2014 thesupremecommander
*  BSD 2-Clause License
*  http://opensource.org/licenses/BSD-2-Clause
*
*/

#include "fovoverride.h"

FOVOverride::FOVOverride() {
	frameHook = 0;
	getFOVHook = 0;

	enabled = new ConVar("statusspec_fovoverride_enabled", "0", FCVAR_NONE, "enable FOV override", [](IConVar *var, const char *pOldValue, float flOldValue) { g_FOVOverride->ToggleEnabled(var, pOldValue, flOldValue); });
	fov = new ConVar("statusspec_fovoverride_fov", "90", FCVAR_NONE, "the FOV value used");
	zoomed = new ConVar("statusspec_fovoverride_zoomed", "0", FCVAR_NONE, "enable FOV override even when sniper rifle is zoomed");
}

void FOVOverride::FrameHook(ClientFrameStage_t curStage) {
	if (curStage == FRAME_NET_UPDATE_END) {
		if (HookGetFOV()) {
			Funcs::RemoveHook(frameHook);
			frameHook = 0;
		}
	}

	RETURN_META(MRES_IGNORED);
}

float FOVOverride::GetFOVOverride() {
	if (!zoomed->GetBool()) {
		Player player = (IClientEntity *) META_IFACEPTR(C_TFPlayer);

		if (player && player.CheckCondition(TFCond_Zoomed)) {
			RETURN_META_VALUE(MRES_IGNORED, 0.0f);
		}
	}

	RETURN_META_VALUE(MRES_SUPERCEDE, fov->GetFloat());
}

bool FOVOverride::HookGetFOV() {
	if (getFOVHook) {
		return true;
	}

	for (int i = 1; i <= MAX_PLAYERS; i++) {
		Player player = i;

		if (player) {
			getFOVHook = Funcs::AddGlobalHook_C_TFPlayer_GetFOV((C_TFPlayer *)player.GetEntity(), SH_MEMBER(this, &FOVOverride::GetFOVOverride), false);

			if (getFOVHook) {
				return true;
			}
		}
	}

	return false;
}

void FOVOverride::ToggleEnabled(IConVar *var, const char *pOldValue, float flOldValue) {
	if (enabled->GetBool()) {
		if (!HookGetFOV() && !frameHook) {
			frameHook = Funcs::AddHook_IBaseClientDLL_FrameStageNotify(Interfaces::pClientDLL, SH_MEMBER(this, &FOVOverride::FrameHook), true);
		}
	}
	else {
		if (getFOVHook) {
			if (Funcs::RemoveHook(getFOVHook)) {
				getFOVHook = 0;
			}
		}

		if (frameHook) {
			if (Funcs::RemoveHook(frameHook)) {
				frameHook = 0;
			}
		}
	}
}