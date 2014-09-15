/*
 *  playeroutlines.cpp
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "playeroutlines.h"

inline float ChangeScale(float currentValue, float currentMin, float currentMax, float newMin, float newMax) {
	float deltaScaler = ((newMax - newMin) / (currentMax - currentMin));
	float newDelta = ((currentValue - currentMin) * deltaScaler);
	float newValue = newMin + newDelta;

	return newValue;
}

inline int ColorRangeRestrict(int color) {
	if (color < 0) return 0;
	else if (color > 255) return 255;
	else return color;
}

inline bool IsInteger(const std::string &s) {
   if (s.empty() || !isdigit(s[0])) return false;

   char *p;
   strtoull(s.c_str(), &p, 10);

   return (*p == 0);
}

PlayerOutlines::PlayerOutlines() {
	colors["blu_low"].color = Color(88, 133, 162);
	colors["blu_low"].command = new ConCommand("statusspec_playeroutlines_color_blu_low", PlayerOutlines::ColorCommand, "the color used for outlines for BLU team players at low health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["blu_medium"].color = Color(88, 133, 162);
	colors["blu_medium"].command = new ConCommand("statusspec_playeroutlines_color_blu_medium", PlayerOutlines::ColorCommand, "the color used for outlines for BLU team players at medium health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["blu_full"].color = Color(88, 133, 162);
	colors["blu_full"].command = new ConCommand("statusspec_playeroutlines_color_blu_full", PlayerOutlines::ColorCommand, "the color used for outlines for BLU team players at full health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["blu_buff"].color = Color(88, 133, 162);
	colors["blu_buff"].command = new ConCommand("statusspec_playeroutlines_color_blu_buff", PlayerOutlines::ColorCommand, "the color used for outlines for BLU team players at max buffed health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["red_low"].color = Color(184, 56, 59);
	colors["red_low"].command = new ConCommand("statusspec_playeroutlines_color_red_low", PlayerOutlines::ColorCommand, "the color used for outlines for RED team players at low health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["red_medium"].color = Color(184, 56, 59);
	colors["red_medium"].command = new ConCommand("statusspec_playeroutlines_color_red_medium", PlayerOutlines::ColorCommand, "the color used for outlines for RED team players at medium health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["red_full"].color = Color(184, 56, 59);
	colors["red_full"].command = new ConCommand("statusspec_playeroutlines_color_red_full", PlayerOutlines::ColorCommand, "the color used for outlines for RED team players at full health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	colors["red_buff"].color = Color(184, 56, 59);
	colors["red_buff"].command = new ConCommand("statusspec_playeroutlines_color_red_buff", PlayerOutlines::ColorCommand, "the color used for outlines for RED team players at max buffed health", FCVAR_NONE, PlayerOutlines::GetCurrentColor);
	
	enabled = new ConVar("statusspec_playeroutlines_enabled", "0", FCVAR_NONE, "enable player outlines");
	health_adjusted_team_colors = new ConVar("statusspec_playeroutlines_health_adjusted_team_colors", "0", FCVAR_NONE, "adjusts team colors depending on health of players");
	team_colors = new ConVar("statusspec_playeroutlines_team_colors", "0", FCVAR_NONE, "override default health-based outline colors with team colors");
}

bool PlayerOutlines::IsEnabled() {
	return enabled->GetBool();
}

void PlayerOutlines::ProcessEntity(IClientEntity* entity) {
	if (!Entities::CheckClassBaseclass(entity->GetClientClass(), "DT_TFPlayer")) {
		return;
	}

	if (IsEnabled()) {
		Color glowColor = GetGlowColor(entity);

		float red = glowColor.r() / 255.0f;
		float green = glowColor.g() / 255.0f;
		float blue = glowColor.b() / 255.0f;
		float alpha = glowColor.a() / 255.0f;

		SetGlowEffect(entity, true, Vector(red, green, blue), alpha);
	}
	else {
		SetGlowEffect(entity, false);
	}
}

Color PlayerOutlines::GetGlowColor(IClientEntity *entity) {
	if (!Entities::CheckClassBaseclass(entity->GetClientClass(), "DT_TFPlayer")) {
		return Color(0, 0, 0, 0);
	}

	float red = 255.0f;
	float green = 255.0f;
	float blue = 255.0f;
	float alpha = 255.0f;

	if (!team_colors->GetBool()) {
		Funcs::CallFunc_C_TFPlayer_GetGlowEffectColor((C_TFPlayer *)entity, &red, &green, &blue);

		red *= 255.0f;
		green *= 255.0f;
		blue *= 255.0f;
	}
	else {
		TFTeam team = (TFTeam)*MAKE_PTR(int*, entity, Entities::pCTFPlayer__m_iTeamNum);

		if (!health_adjusted_team_colors->GetBool()) {
			if (team == TFTeam_Red) {
				red = colors["red_full"].color.r();
				green = colors["red_full"].color.g();
				blue = colors["red_full"].color.b();
				alpha = colors["red_full"].color.a();
			}
			else if (team == TFTeam_Blue) {
				red = colors["blu_full"].color.r();
				green = colors["blu_full"].color.g();
				blue = colors["blu_full"].color.b();
				alpha = colors["blu_full"].color.a();
			}
		}
		else {
			int health = *MAKE_PTR(int*, Interfaces::GetGameResources(), Entities::pCTFPlayerResource__m_iHealth[entity->entindex()]);
			int maxHealth = *MAKE_PTR(int*, Interfaces::GetGameResources(), Entities::pCTFPlayerResource__m_iMaxHealth[entity->entindex()]);

			// CTFPlayerResource isn't giving us proper values so let's calculate it manually
			int maxBuffedHealth = ((maxHealth / 5) * 3 / 2) * 5;

			// calculate this once instead of several times
			float halfHealth = maxHealth * 0.5f;

			if (team == TFTeam_Red) {
				if (health < 0) {
					// this should never happen

					red = colors["red_low"].color.r();
					green = colors["red_low"].color.g();
					blue = colors["red_low"].color.b();
					alpha = colors["red_low"].color.a();
				}
				else if (health >= 0 && health < halfHealth) {
					red = ChangeScale(health, 0, halfHealth, colors["red_low"].color.r(), colors["red_medium"].color.r());
					green = ChangeScale(health, 0, halfHealth, colors["red_low"].color.g(), colors["red_medium"].color.g());
					blue = ChangeScale(health, 0, halfHealth, colors["red_low"].color.b(), colors["red_medium"].color.b());
					alpha = ChangeScale(health, 0, halfHealth, colors["red_low"].color.a(), colors["red_medium"].color.a());
				}
				else if (health >= halfHealth && health < maxHealth) {
					red = ChangeScale(health, halfHealth, maxHealth, colors["red_medium"].color.r(), colors["red_full"].color.r());
					green = ChangeScale(health, halfHealth, maxHealth, colors["red_medium"].color.g(), colors["red_full"].color.g());
					blue = ChangeScale(health, halfHealth, maxHealth, colors["red_medium"].color.b(), colors["red_full"].color.b());
					alpha = ChangeScale(health, halfHealth, maxHealth, colors["red_medium"].color.a(), colors["red_full"].color.a());
				}
				else if (health >= maxHealth && health <= maxBuffedHealth) {
					red = ChangeScale(health, maxHealth, maxBuffedHealth, colors["red_full"].color.r(), colors["red_buff"].color.r());
					green = ChangeScale(health, maxHealth, maxBuffedHealth, colors["red_full"].color.g(), colors["red_buff"].color.g());
					blue = ChangeScale(health, maxHealth, maxBuffedHealth, colors["red_full"].color.b(), colors["red_buff"].color.b());
					alpha = ChangeScale(health, maxHealth, maxBuffedHealth, colors["red_full"].color.a(), colors["red_buff"].color.a());
				}
				else if (health >= maxBuffedHealth) {
					// our max buffed health above does not take into account special cases so we have to compensate

					red = colors["red_buff"].color.r();
					green = colors["red_buff"].color.g();
					blue = colors["red_buff"].color.b();
					alpha = colors["red_buff"].color.a();
				}
				else {
					// this DEFINITELY shouldn't ever happen

					return Color(0, 0, 0, 0);
				}
			}
			else if (team == TFTeam_Blue) {
				if (health < 0) {
					// this should never happen

					red = colors["blu_low"].color.r();
					green = colors["blu_low"].color.g();
					blue = colors["blu_low"].color.b();
					alpha = colors["blu_low"].color.a();
				}
				else if (health >= 0 && health < (maxHealth * 0.5f)) {
					red = ChangeScale(health, 0, maxHealth * 0.5f, colors["blu_low"].color.r(), colors["blu_medium"].color.r());
					green = ChangeScale(health, 0, maxHealth * 0.5f, colors["blu_low"].color.g(), colors["blu_medium"].color.g());
					blue = ChangeScale(health, 0, maxHealth * 0.5f, colors["blu_low"].color.b(), colors["blu_medium"].color.b());
					alpha = ChangeScale(health, 0, maxHealth * 0.5f, colors["blu_low"].color.a(), colors["blu_medium"].color.a());
				}
				else if (health >= (maxHealth * 0.5f) && health < maxHealth) {
					red = ChangeScale(health, maxHealth * 0.5f, maxHealth, colors["blu_medium"].color.r(), colors["blu_full"].color.r());
					green = ChangeScale(health, maxHealth * 0.5f, maxHealth, colors["blu_medium"].color.g(), colors["blu_full"].color.g());
					blue = ChangeScale(health, maxHealth * 0.5f, maxHealth, colors["blu_medium"].color.b(), colors["blu_full"].color.b());
					alpha = ChangeScale(health, maxHealth * 0.5f, maxHealth, colors["blu_medium"].color.a(), colors["blu_full"].color.a());
				}
				else if (health >= maxHealth && health <= maxBuffedHealth) {
					red = ChangeScale(health, maxHealth, maxBuffedHealth, colors["blu_full"].color.r(), colors["blu_buff"].color.r());
					green = ChangeScale(health, maxHealth, maxBuffedHealth, colors["blu_full"].color.g(), colors["blu_buff"].color.g());
					blue = ChangeScale(health, maxHealth, maxBuffedHealth, colors["blu_full"].color.b(), colors["blu_buff"].color.b());
					alpha = ChangeScale(health, maxHealth, maxBuffedHealth, colors["blu_full"].color.a(), colors["blu_buff"].color.a());
				}
				else if (health >= maxBuffedHealth) {
					// our max buffed health above does not take into account special cases so we have to compensate

					red = colors["blu_buff"].color.r();
					green = colors["blu_buff"].color.g();
					blue = colors["blu_buff"].color.b();
					alpha = colors["blu_buff"].color.a();
				}
				else {
					// this DEFINITELY shouldn't ever happen

					return Color(0, 0, 0, 0);
				}
			}
		}
	}

	return Color(red, green, blue, alpha);
}

void PlayerOutlines::SetGlowEffect(IClientEntity *entity, bool enabled, Vector color, float alpha) {
	EHANDLE entityHandle = dynamic_cast<C_BaseEntity *>(entity);

	if (enabled) {
		if (glows.find(entityHandle) == glows.end()) {
			glows[entityHandle] = new CGlowObject(entityHandle.Get(), color, alpha, true, true);
		}
		else {
			glows[entityHandle]->SetColor(color);
			glows[entityHandle]->SetAlpha(alpha);
		}
	}
	else {
		if (glows.find(entityHandle) != glows.end()) {
			delete glows[entityHandle];
			glows.erase(entityHandle);
		}
	}
}

void PlayerOutlines::ColorCommand(const CCommand &command) {
	if (strncmp(command.Arg(0), "statusspec_playeroutlines_color_", 32) == 0) {
		std::string mainCommand = command.Arg(0);
		std::string colorType = mainCommand.substr(32);

		if (g_PlayerOutlines->colors.find(colorType) != g_PlayerOutlines->colors.end()) {
			if (command.ArgC() == 1) {
				Warning("\"%s\" = %i %i %i %i\n", g_PlayerOutlines->colors[colorType].command->GetName(), g_PlayerOutlines->colors[colorType].color.r(), g_PlayerOutlines->colors[colorType].color.g(), g_PlayerOutlines->colors[colorType].color.b(), g_PlayerOutlines->colors[colorType].color.a());
				Msg(" - %s\n", g_PlayerOutlines->colors[colorType].command->GetHelpText());

				return;
			}
			else if (command.ArgC() >= 5 && IsInteger(command.Arg(1)) && IsInteger(command.Arg(2)) && IsInteger(command.Arg(3)) && IsInteger(command.Arg(4)))
			{
				g_PlayerOutlines->colors[colorType].color.SetColor(ColorRangeRestrict(std::stoi(command.Arg(1))), ColorRangeRestrict(std::stoi(command.Arg(2))), ColorRangeRestrict(std::stoi(command.Arg(3))), ColorRangeRestrict(std::stoi(command.Arg(4))));

				return;
			}
			else if (command.ArgC() >= 4 && IsInteger(command.Arg(1)) && IsInteger(command.Arg(2)) && IsInteger(command.Arg(3)))
			{
				g_PlayerOutlines->colors[colorType].color.SetColor(ColorRangeRestrict(std::stoi(command.Arg(1))), ColorRangeRestrict(std::stoi(command.Arg(2))), ColorRangeRestrict(std::stoi(command.Arg(3))), 255);

				return;
			}
			else {
				Warning("Usage: %s <red> <green> <blue> [alpha]\n", command.Arg(0));

				return;
			}
		}
	}

	Warning("Unrecognized command!\n");
}

int PlayerOutlines::GetCurrentColor(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]) {
	std::stringstream ss(partial);
	std::string command;
	std::getline(ss, command, ' ');

	if (command.compare(0, 32, "statusspec_playeroutlines_color_") == 0) {
		std::string colorType = command.substr(32);

		if (g_PlayerOutlines->colors.find(colorType) != g_PlayerOutlines->colors.end()) {
			V_snprintf(commands[0], sizeof(commands[0]), "%s %i %i %i", command.c_str(), g_PlayerOutlines->colors[colorType].color.r(), g_PlayerOutlines->colors[colorType].color.g(), g_PlayerOutlines->colors[colorType].color.b());

			return 1;
		}
	}

	return 0;
}