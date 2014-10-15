/*
 *  statusicons.cpp
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "statusicons.h"

StatusIcons::StatusIcons() {
	frameHook = 0;

	delta_x = new ConVar("statusspec_statusicons_delta_x", "15", FCVAR_NONE, "change in the x direction for each icon");
	delta_y = new ConVar("statusspec_statusicons_delta_y", "0", FCVAR_NONE, "change in the y direction for each icon");
	enabled = new ConVar("statusspec_statusicons_enabled", "0", FCVAR_NONE, "enable status icons", [](IConVar *var, const char *pOldValue, float flOldValue) { g_StatusIcons->ToggleEnabled(var, pOldValue, flOldValue); });
}

void StatusIcons::FrameHook(ClientFrameStage_t curStage) {
	if (curStage == FRAME_NET_UPDATE_END) {
		if (Interfaces::GetClientMode() && Interfaces::GetClientMode()->GetViewport()) {
			vgui::VPANEL viewport = Interfaces::GetClientMode()->GetViewport()->GetVPanel();

			for (int i = 0; i < g_pVGuiPanel->GetChildCount(viewport); i++) {
				vgui::VPANEL specgui = g_pVGuiPanel->GetChild(viewport, i);

				if (strcmp(g_pVGuiPanel->GetName(specgui), "specgui") == 0) {
					for (int i = 0; i < g_pVGuiPanel->GetChildCount(specgui); i++) {
						vgui::VPANEL playerPanel = g_pVGuiPanel->GetChild(specgui, i);

						if (strcmp(g_pVGuiPanel->GetClassName(playerPanel), "CTFPlayerPanel") == 0) {
							for (int i = 0; i < g_pVGuiPanel->GetChildCount(playerPanel); i++) {
								vgui::VPANEL statusIconsVPanel = g_pVGuiPanel->GetChild(playerPanel, i);

								if (strcmp(g_pVGuiPanel->GetName(statusIconsVPanel), "StatusIcons") == 0) {
									vgui::EditablePanel *statusIcons = dynamic_cast<vgui::EditablePanel *>(g_pVGuiPanel->GetPanel(statusIconsVPanel, "ClientDLL"));

									if (statusIcons) {
										statusIcons->SetEnabled(true);
										statusIcons->SetVisible(true);

										DisplayIcons(playerPanel);
									}

									break;
								}
							}
						}
					}

					break;
				}
			}
		}
	}
}

void StatusIcons::ClearIcons(vgui::VPANEL statusIconsPanel) {
	while (g_pVGuiPanel->GetChildCount(statusIconsPanel) != 0) {
		vgui::VPANEL child = g_pVGuiPanel->GetChild(statusIconsPanel, 0);
		g_pVGuiPanel->DeletePanel(child);
	}
}

void StatusIcons::DisableHUD() {
	if (Interfaces::GetClientMode() && Interfaces::GetClientMode()->GetViewport()) {
		vgui::VPANEL viewport = Interfaces::GetClientMode()->GetViewport()->GetVPanel();

		for (int i = 0; i < g_pVGuiPanel->GetChildCount(viewport); i++) {
			vgui::VPANEL specgui = g_pVGuiPanel->GetChild(viewport, i);

			if (strcmp(g_pVGuiPanel->GetName(specgui), "specgui") == 0) {
				for (int i = 0; i < g_pVGuiPanel->GetChildCount(specgui); i++) {
					vgui::VPANEL playerPanel = g_pVGuiPanel->GetChild(specgui, i);

					if (strcmp(g_pVGuiPanel->GetClassName(playerPanel), "CTFPlayerPanel") == 0) {
						for (int i = 0; i < g_pVGuiPanel->GetChildCount(playerPanel); i++) {
							vgui::VPANEL statusIconsVPanel = g_pVGuiPanel->GetChild(playerPanel, i);

							if (strcmp(g_pVGuiPanel->GetName(statusIconsVPanel), "StatusIcons") == 0) {
								ClearIcons(statusIconsVPanel);

								break;
							}
						}
					}
				}

				break;
			}
		}
	}
}

void StatusIcons::DisplayIcon(vgui::EditablePanel *panel, const char *iconTexture) {
	if (panel) {
		int offset = panel->GetChildCount();

		vgui::EditablePanel *iconContainer = new vgui::EditablePanel(panel, "StatusIcon");
		vgui::ImagePanel *icon = new vgui::ImagePanel(iconContainer, "StatusIconImage");

		iconContainer->LoadControlSettings("Resource/UI/StatusIcon.res");

		iconContainer->SetEnabled(true);
		iconContainer->SetVisible(true);

		int deltaX = g_pVGuiSchemeManager->GetProportionalScaledValue(delta_x->GetInt());
		int deltaY = g_pVGuiSchemeManager->GetProportionalScaledValue(delta_y->GetInt());

		iconContainer->SetPos(offset * deltaX, offset * deltaY);

		icon->SetEnabled(true);
		icon->SetVisible(true);

		std::string iconPath = "../";
		iconPath += iconTexture;

		icon->SetImage(iconPath.c_str());
	}
}

void StatusIcons::DisplayIcons(vgui::VPANEL playerPanel) {
	if (strcmp(g_pVGuiPanel->GetClassName(playerPanel), "CTFPlayerPanel") == 0) {
		vgui::EditablePanel *panel = dynamic_cast<vgui::EditablePanel *>(g_pVGuiPanel->GetPanel(playerPanel, "ClientDLL"));

		if (panel) {
			KeyValues *dialogVariables = panel->GetDialogVariables();

			if (dialogVariables) {
				const char *name = dialogVariables->GetString("playername");

				for (int i = 1; i <= MAX_PLAYERS; i++) {
					Player player = i;

					if (player && strcmp(player.GetName(), name) == 0) {
						for (int i = 0; i < g_pVGuiPanel->GetChildCount(playerPanel); i++) {
							vgui::VPANEL statusIconsVPanel = g_pVGuiPanel->GetChild(playerPanel, i);

							if (strcmp(g_pVGuiPanel->GetName(statusIconsVPanel), "StatusIcons") == 0) {
								ClearIcons(statusIconsVPanel);

								vgui::EditablePanel *statusIcons = dynamic_cast<vgui::EditablePanel *>(g_pVGuiPanel->GetPanel(statusIconsVPanel, "ClientDLL"));

								if (statusIcons) {
									TFTeam team = player.GetTeam();

									if (player.CheckCondition(TFCond_Ubercharged)) {
										DisplayIcon(statusIcons, TEXTURE_UBERCHARGE);
									}

									if (player.CheckCondition(TFCond_Kritzkrieged)) {
										DisplayIcon(statusIcons, TEXTURE_CRITBOOST);
									}

									if (player.CheckCondition(TFCond_MegaHeal)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_MEGAHEALRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_MEGAHEALBLU);
										}
									}

									if (player.CheckCondition(TFCond_UberBulletResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDRED);
											DisplayIcon(statusIcons, TEXTURE_BULLETRESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDBLU);
											DisplayIcon(statusIcons, TEXTURE_BULLETRESISTBLU);
										}
									}
									else if (player.CheckCondition(TFCond_SmallBulletResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_BULLETRESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_BULLETRESISTBLU);
										}
									}

									if (player.CheckCondition(TFCond_UberBlastResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDRED);
											DisplayIcon(statusIcons, TEXTURE_BLASTRESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDBLU);
											DisplayIcon(statusIcons, TEXTURE_BLASTRESISTBLU);
										}
									}
									else if (player.CheckCondition(TFCond_SmallBlastResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_BLASTRESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_BLASTRESISTBLU);
										}
									}

									if (player.CheckCondition(TFCond_UberFireResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDRED);
											DisplayIcon(statusIcons, TEXTURE_FIRERESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_RESISTSHIELDBLU);
											DisplayIcon(statusIcons, TEXTURE_FIRERESISTBLU);
										}
									}
									else if (player.CheckCondition(TFCond_SmallFireResist)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_FIRERESISTRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_FIRERESISTBLU);
										}
									}

									if (player.CheckCondition(TFCond_Buffed)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_BUFFBANNERRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_BUFFBANNERBLU);
										}
									}

									if (player.CheckCondition(TFCond_DefenseBuffed)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_BATTALIONSBACKUPRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_BATTALIONSBACKUPBLU);
										}
									}

									if (player.CheckCondition(TFCond_RegenBuffed)) {
										if (team == TFTeam_Red) {
											DisplayIcon(statusIcons, TEXTURE_CONCHERORRED);
										}
										else if (team == TFTeam_Blue) {
											DisplayIcon(statusIcons, TEXTURE_CONCHERORBLU);
										}
									}

									if (player.CheckCondition(TFCond_Jarated)) {
										DisplayIcon(statusIcons, TEXTURE_JARATE);
									}

									if (player.CheckCondition(TFCond_Milked)) {
										DisplayIcon(statusIcons, TEXTURE_MADMILK);
									}

									if (player.CheckCondition(TFCond_MarkedForDeath) || player.CheckCondition(TFCond_MarkedForDeathSilent)) {
										DisplayIcon(statusIcons, TEXTURE_MARKFORDEATH);
									}

									if (player.CheckCondition(TFCond_Bleeding)) {
										DisplayIcon(statusIcons, TEXTURE_BLEEDING);
									}

									if (player.CheckCondition(TFCond_OnFire)) {
										DisplayIcon(statusIcons, TEXTURE_FIRE);
									}
								}

								break;
							}
						}

						break;
					}
				}
			}
		}
	}
}

void StatusIcons::ToggleEnabled(IConVar *var, const char *pOldValue, float flOldValue) {
	if (enabled->GetBool()) {
		if (!frameHook) {
			frameHook = Funcs::AddHook_IBaseClientDLL_FrameStageNotify(Interfaces::pClientDLL, SH_MEMBER(this, &StatusIcons::FrameHook), true);
		}
	}
	else {
		DisableHUD();

		if (frameHook) {
			if (Funcs::RemoveHook(frameHook)) {
				frameHook = 0;
			}
		}
	}
}