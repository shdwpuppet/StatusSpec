/*
 *  loadouticons.cpp
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "loadouticons.h"

LoadoutIcons *g_LoadoutIcons;

#define SHOW_SLOT_ICON(slot) \
	if (loadoutInfo[i].slot != -1) { \
		if (loadoutInfo[i].activeWeaponSlot.compare(#slot) == 0) { \
			Paint::DrawTexture(itemIconTextures[loadoutInfo[i].slot], iconsWide, 0, iconSize, iconSize, filter_active_color); \
		} \
		else { \
			Paint::DrawTexture(itemIconTextures[loadoutInfo[i].slot], iconsWide, 0, iconSize, iconSize, filter_nonactive_color); \
		} \
	} \
	 \
	iconsWide += iconSize;

Color LoadoutIcons::filter_active_color(255, 255, 255, 255);
Color LoadoutIcons::filter_nonactive_color(127, 127, 127, 255);
std::map<int, std::string> LoadoutIcons::itemIconTextures;
ItemSchema* LoadoutIcons::itemSchema = NULL;

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

ConVar LoadoutIcons::enabled("statusspec_loadouticons_enabled", "0", FCVAR_NONE, "enable loadout icons", LoadoutIcons::ToggleState);
ConCommand LoadoutIcons::filter_active("statusspec_loadouticons_filter_active", LoadoutIcons::SetFilter, "set the RGBA filter applied to the icon for an active item", FCVAR_NONE, LoadoutIcons::GetCurrentFilter);
ConCommand LoadoutIcons::filter_nonactive("statusspec_loadouticons_filter_nonactive", LoadoutIcons::SetFilter, "set the RGBA filter applied to the icon for a nonactive item", FCVAR_NONE, LoadoutIcons::GetCurrentFilter);
ConVar LoadoutIcons::nonloadout("statusspec_loadouticons_nonloadout", "0", FCVAR_NONE, "enable loadout icons for nonloadout items");

LoadoutIcons::LoadoutIcons() {
	if (!itemSchema) {
		itemSchema = new ItemSchema();
	}
}

void LoadoutIcons::InterceptMessage(vgui::VPANEL vguiPanel, KeyValues *params, vgui::VPANEL ifromPanel) {
	std::string originPanelName = g_pVGuiPanel->GetName(ifromPanel);

	if (originPanelName.substr(0, 11).compare("playerpanel") == 0 && strcmp(params->GetName(), "DialogVariables") == 0) {
		const char *playerName = params->GetString("playername", NULL);
		
		if (playerName) {
			int maxEntity = Interfaces::pClientEntityList->GetHighestEntityIndex();
		
			for (int i = 0; i < maxEntity; i++) {
				IClientEntity *entity = Interfaces::pClientEntityList->GetClientEntity(i);
			
				if (entity == NULL || !Interfaces::GetGameResources()->IsConnected(i)) {
					continue;
				}
			
				if (strcmp(playerName, Interfaces::GetGameResources()->GetPlayerName(i)) == 0) {
					playerPanels[originPanelName] = i;

					break;
				}
			}
		}
	}
}

void LoadoutIcons::Paint(vgui::VPANEL vguiPanel) {
	const char *panelName = g_pVGuiPanel->GetName(vguiPanel);
	
	if (strcmp(panelName, "loadouticons") == 0) {
		vgui::VPANEL playerPanel = g_pVGuiPanel->GetParent(vguiPanel);
		const char *playerPanelName = g_pVGuiPanel->GetName(playerPanel);
		
		if (playerPanels.find(playerPanelName) == playerPanels.end()) {
			return;
		}
		
		int i = playerPanels[playerPanelName];
		
		int iconsWide, iconsTall;
		
		g_pVGuiPanel->GetSize(vguiPanel, iconsWide, iconsTall);
		
		int iconSize = iconsTall;
		iconsWide = 0;

		if (loadoutInfo[i].tfclass == TFClass_Engineer) {
			SHOW_SLOT_ICON(primary);
			SHOW_SLOT_ICON(secondary);
			SHOW_SLOT_ICON(melee);
			SHOW_SLOT_ICON(pda);
			
			if (nonloadout.GetBool()) {
				SHOW_SLOT_ICON(pda2);
				SHOW_SLOT_ICON(building);
			}
		}
		else if (loadoutInfo[i].tfclass == TFClass_Spy) {
			SHOW_SLOT_ICON(secondary);
			SHOW_SLOT_ICON(building);
			SHOW_SLOT_ICON(melee);
			
			if (nonloadout.GetBool()) {
				SHOW_SLOT_ICON(pda);
			}
			
			SHOW_SLOT_ICON(pda2);
		}
		else {
			SHOW_SLOT_ICON(primary);
			SHOW_SLOT_ICON(secondary);
			SHOW_SLOT_ICON(melee);
		}
	}
}

void LoadoutIcons::Update() {
	loadoutInfo.clear();

	int maxEntity = Interfaces::pClientEntityList->GetHighestEntityIndex();

	for (int i = 0; i < maxEntity; i++) {
		IClientEntity *entity = Interfaces::pClientEntityList->GetClientEntity(i);
		
		if (!entity || !Entities::CheckClassBaseclass(entity->GetClientClass(), "DT_EconEntity")) {
			continue;
		}

		int itemDefinitionIndex = *MAKE_PTR(int*, entity, Entities::pCEconEntity__m_iItemDefinitionIndex);

		int player = ENTITY_INDEX_FROM_ENTITY_OFFSET(entity, Entities::pCEconEntity__m_hOwnerEntity);
		IClientEntity *playerEntity = Interfaces::pClientEntityList->GetClientEntity(player);
		TFClassType tfclass = (TFClassType) *MAKE_PTR(int*, playerEntity, Entities::pCTFPlayer__m_iClass);
		int activeWeapon = ENTITY_INDEX_FROM_ENTITY_OFFSET(playerEntity, Entities::pCTFPlayer__m_hActiveWeapon);

		const char *itemSlot = itemSchema->GetItemKeyData(itemDefinitionIndex, "item_slot");
			
		KeyValues *classUses = itemSchema->GetItemKey(itemDefinitionIndex, "used_by_classes");
		if (classUses) {
			const char *classUse = classUses->GetString(tfclassNames[tfclass].c_str(), "");

			if (std::find(std::begin(itemSlots), std::end(itemSlots), classUse) != std::end(itemSlots)) {
				itemSlot = classUse;
			}
		}

		if (activeWeapon == i) {
			loadoutInfo[player].activeWeaponSlot = itemSlot;
		}
			
		if (strcmp(itemSlot, "primary") == 0) {
			loadoutInfo[player].primary = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "secondary") == 0) {
			loadoutInfo[player].secondary = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "melee") == 0) {
			loadoutInfo[player].melee = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "pda") == 0) {
			loadoutInfo[player].pda = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "pda2") == 0) {
			loadoutInfo[player].pda2 = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "building") == 0) {
			loadoutInfo[player].building = itemDefinitionIndex;
		}
		else if (strcmp(itemSlot, "head") == 0 || strcmp(itemSlot, "misc") == 0) {
			for (int slot = 0; slot < 3; slot++) {
				if (loadoutInfo[player].cosmetic[slot] == -1) {
					loadoutInfo[player].cosmetic[slot] = itemDefinitionIndex;
					break;
				}
			}
		}
		else if (strcmp(itemSlot, "action") == 0) {
			loadoutInfo[player].action = itemDefinitionIndex;
		}
				
		const char *itemIcon = itemSchema->GetItemKeyData(itemDefinitionIndex, "image_inventory");
		Paint::InitializeTexture(itemIcon);
		itemIconTextures[itemDefinitionIndex] = itemIcon;
	}
	
	for (auto iterator = loadoutInfo.begin(); iterator != loadoutInfo.end(); iterator++) {
		int player = iterator->first;
		IClientEntity *playerEntity = Interfaces::pClientEntityList->GetClientEntity(player);
			
		if (!playerEntity) {
			continue;
		}
		
		TFClassType tfclass = (TFClassType) *MAKE_PTR(int*, playerEntity, Entities::pCTFPlayer__m_iClass);
		int activeWeapon = ENTITY_INDEX_FROM_ENTITY_OFFSET(playerEntity, Entities::pCTFPlayer__m_hActiveWeapon);
		
		loadoutInfo[player].tfclass = tfclass;

		for (int i = 0; i < MAX_WEAPONS; i++) {
			int weapon = ENTITY_INDEX_FROM_ENTITY_OFFSET(playerEntity, Entities::pCTFPlayer__m_hMyWeapons[i]);
			IClientEntity *weaponEntity = Interfaces::pClientEntityList->GetClientEntity(weapon);
			
			if (!weaponEntity || !Entities::CheckClassBaseclass(weaponEntity->GetClientClass(), "DT_EconEntity")) {
				continue;
			}
				
			int itemDefinitionIndex = *MAKE_PTR(int*, weaponEntity, Entities::pCEconEntity__m_iItemDefinitionIndex);
			
			const char *itemSlot = itemSchema->GetItemKeyData(itemDefinitionIndex, "item_slot");
				
			KeyValues *classUses = itemSchema->GetItemKey(itemDefinitionIndex, "used_by_classes");
			if (classUses) {
				const char *classUse = classUses->GetString(tfclassNames[tfclass].c_str(), "");

				if (std::find(std::begin(itemSlots), std::end(itemSlots), classUse) != std::end(itemSlots)) {
					itemSlot = classUse;
				}
			}

			if (activeWeapon == i) {
				loadoutInfo[player].activeWeaponSlot = itemSlot;
			}
				
			if (strcmp(itemSlot, "primary") == 0) {
				loadoutInfo[player].primary = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "secondary") == 0) {
				loadoutInfo[player].secondary = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "melee") == 0) {
				loadoutInfo[player].melee = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "pda") == 0) {
				loadoutInfo[player].pda = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "pda2") == 0) {
				loadoutInfo[player].pda2 = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "building") == 0) {
				loadoutInfo[player].building = itemDefinitionIndex;
			}
			else if (strcmp(itemSlot, "head") == 0 || strcmp(itemSlot, "misc") == 0) {
				for (int slot = 0; slot < 3; slot++) {
					if (loadoutInfo[player].cosmetic[slot] == -1) {
						loadoutInfo[player].cosmetic[slot] = itemDefinitionIndex;
						break;
					}
				}
			}
			else if (strcmp(itemSlot, "action") == 0) {
				loadoutInfo[player].action = itemDefinitionIndex;
			}
		}
	}
}

int LoadoutIcons::GetCurrentFilter(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]) {
	std::stringstream ss(partial);
	std::string command;
	std::getline(ss, command, ' ');

	if (stricmp(command.c_str(), "statusspec_loadouticons_filter_active") == 0) {
		V_snprintf(commands[0], COMMAND_COMPLETION_ITEM_LENGTH, "%s %i %i %i %i", command.c_str(), filter_active_color.r(), filter_active_color.g(), filter_active_color.b(), filter_active_color.a());
		
		return 1;
	}
	else if (stricmp(command.c_str(), "statusspec_loadouticons_filter_nonactive") == 0) {
		V_snprintf(commands[0], COMMAND_COMPLETION_ITEM_LENGTH, "%s %i %i %i %i", command.c_str(), filter_nonactive_color.r(), filter_nonactive_color.g(), filter_nonactive_color.b(), filter_nonactive_color.a());

		return 1;
	}
	else {
		return 0;
	}
}

void LoadoutIcons::SetFilter(const CCommand &command) {
	if (command.ArgC() < 4 || !IsInteger(command.Arg(1)) || !IsInteger(command.Arg(2)) || !IsInteger(command.Arg(3)) || !IsInteger(command.Arg(4)))
	{
		Warning("Usage: %s <red> <green> <blue> <alpha>\n", command.Arg(0));
		return;
	}

	int red = ColorRangeRestrict(std::stoi(command.Arg(1)));
	int green = ColorRangeRestrict(std::stoi(command.Arg(2)));
	int blue = ColorRangeRestrict(std::stoi(command.Arg(3)));
	int alpha = ColorRangeRestrict(std::stoi(command.Arg(4)));

	if (stricmp(command.Arg(0), "statusspec_loadouticons_filter_active")) {
		filter_active_color.SetColor(red, green, blue, alpha);
		Msg("Set active loadout item icon filter to rgba(%i, %i, %i, %i).\n", red, green, blue, alpha);
	}
	else if (stricmp(command.Arg(0), "statusspec_loadouticons_filter_nonactive")) {
		filter_nonactive_color.SetColor(red, green, blue, alpha);
		Msg("Set nonactive loadout item icon filter to rgba(%i, %i, %i, %i).\n", red, green, blue, alpha);
	}
	else {
		Warning("Unrecognized command!\n");
	}
}

void LoadoutIcons::ToggleState(IConVar *var, const char *pOldValue, float flOldValue) {
	if (enabled.GetBool() && !g_LoadoutIcons) {
		g_LoadoutIcons = new LoadoutIcons();
	}
	else if (!enabled.GetBool() && g_LoadoutIcons) {
		delete g_LoadoutIcons;
	}
}