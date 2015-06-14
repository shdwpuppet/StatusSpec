/*
 *  camerasmooths.h
 *  StatusSpec project
 *
 *  Copyright (c) 2014-2015 Forward Command Post
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#pragma once

#include "convar.h"

#include "../modules.h"

class ConVar;

class CameraSmooths : public Module {
public:
	CameraSmooths(std::string name);

	static bool CheckDependencies(std::string name);
private:
	int inToolModeHook;
	int isThirdPersonCameraHook;
	int setupEngineViewHook;
	bool smoothEnding;
	int smoothEndMode;
	int smoothEndTarget;
	bool smoothInProgress;
	QAngle smoothLastAngles;
	Vector smoothLastOrigin;
	float smoothLastTime;

	bool InToolModeOverride();
	bool IsThirdPersonCameraOverride();
	bool SetupEngineViewOverride(Vector &origin, QAngle &angles, float &fov);

	class HLTVCameraOverride;

	ConVar *enabled;
	ConVar *max_angle_difference;
	ConVar *max_distance;
	ConVar *move_speed;
	void ToggleEnabled(IConVar *var, const char *pOldValue, float flOldValue);
};