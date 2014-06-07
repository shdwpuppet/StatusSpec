/*
 *  statusspec.h
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#pragma once

#include "stdafx.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <array>

#include "Color.h"
#include "ehandle.h"
#include "icliententity.h"
#include "vgui/IVGui.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "vgui_controls/pch_vgui_controls.h"

#include "enums.h"
#include "hooks.h"
#include "ifaces.h"
#include "itemschema.h"
#include "offsets.h"
#include "paint.h"

#define PLUGIN_DESC "StatusSpec v0.10.0"

#define TEXTURE_NULL "vgui/replay/thumbnails/null"
#define TEXTURE_UBERCHARGE "vgui/replay/thumbnails/ubercharge"
#define TEXTURE_CRITBOOST "vgui/replay/thumbnails/critboost"
#define TEXTURE_MEGAHEALRED "vgui/replay/thumbnails/megaheal_red"
#define TEXTURE_MEGAHEALBLU "vgui/replay/thumbnails/megaheal_blue"
#define TEXTURE_RESISTSHIELDRED "vgui/replay/thumbnails/resist_shield"
#define TEXTURE_RESISTSHIELDBLU "vgui/replay/thumbnails/resist_shield_blue"
#define TEXTURE_BULLETRESISTRED "vgui/replay/thumbnails/defense_buff_bullet_red"
#define TEXTURE_BLASTRESISTRED "vgui/replay/thumbnails/defense_buff_explosion_red"
#define TEXTURE_FIRERESISTRED "vgui/replay/thumbnails/defense_buff_fire_red"
#define TEXTURE_BULLETRESISTBLU "vgui/replay/thumbnails/defense_buff_bullet_blue"
#define TEXTURE_BLASTRESISTBLU "vgui/replay/thumbnails/defense_buff_explosion_blue"
#define TEXTURE_FIRERESISTBLU "vgui/replay/thumbnails/defense_buff_fire_blue"
#define TEXTURE_BUFFBANNERRED "effects/soldier_buff_offense_red"
#define TEXTURE_BUFFBANNERBLU "effects/soldier_buff_offense_blue"
#define TEXTURE_BATTALIONSBACKUPRED "effects/soldier_buff_defense_red"
#define TEXTURE_BATTALIONSBACKUPBLU "effects/soldier_buff_defense_blue"
#define TEXTURE_CONCHERORRED "effects/soldier_buff_healonhit_red"
#define TEXTURE_CONCHERORBLU "effects/soldier_buff_healonhit_blue"
#define TEXTURE_JARATE "vgui/replay/thumbnails/jarated"
#define TEXTURE_MADMILK "vgui/bleed_drop"
#define TEXTURE_MARKFORDEATH "vgui/marked_for_death"
#define TEXTURE_BLEEDING "vgui/bleed_drop"
#define TEXTURE_FIRE "hud/leaderboard_class_pyro"

#define VGUI_TEXTURE_NULL "replay/thumbnails/null"
#define VGUI_TEXTURE_UBERCHARGE "replay/thumbnails/ubercharge"
#define VGUI_TEXTURE_CRITBOOST "replay/thumbnails/critboost"
#define VGUI_TEXTURE_MEGAHEALRED "replay/thumbnails/megaheal_red"
#define VGUI_TEXTURE_MEGAHEALBLU "replay/thumbnails/megaheal_blue"
#define VGUI_TEXTURE_BULLETRESISTRED "replay/thumbnails/defense_buff_bullet_red"
#define VGUI_TEXTURE_BLASTRESISTRED "replay/thumbnails/defense_buff_explosion_red"
#define VGUI_TEXTURE_FIRERESISTRED "replay/thumbnails/defense_buff_fire_red"
#define VGUI_TEXTURE_BULLETRESISTBLU "replay/thumbnails/defense_buff_bullet_blue"
#define VGUI_TEXTURE_BLASTRESISTBLU "replay/thumbnails/defense_buff_explosion_blue"
#define VGUI_TEXTURE_FIRERESISTBLU "replay/thumbnails/defense_buff_fire_blue"

extern std::map<int, std::string> itemIconTextures;

KeyValues *performlayoutCommand = new KeyValues("Command", "Command", "performlayout");
vgui::HPanel specguiPanel;

#define MAX_COSMETIC_SLOTS 3

typedef struct Player {
	TFClassType tfclass;
	TFTeam team;
	uint32_t conditions[3];
	int primary;
	int secondary;
	int melee;
	int pda;
	int pda2;
	int building;
	int cosmetic[MAX_COSMETIC_SLOTS];
	int action;
	std::string activeWeaponSlot;
} Player;

typedef struct Medigun {
	int itemDefinitionIndex;
	float chargeLevel;
	int chargeResistType;
	bool chargeRelease;
} Medigun;

std::array<std::string, 10> tfclassNames = {"", "scout", "sniper", "soldier", "demoman", "medic", "heavy", "pyro", "spy", "engineer"};
std::array<std::string, 9> itemSlots = {"primary", "secondary", "melee", "pda", "pda2", "building", "head", "misc", "action"};

std::map<CSteamID, std::string> playerAliases;
std::map<std::string, int> playerPanels;
std::map<int, Player> playerInfo;
std::map<TFTeam, Medigun> medigunInfo;

ItemSchema *itemSchema;

Color loadout_nonactive_filter(127, 127, 127, 255);
Color loadout_active_filter(255, 255, 255, 255);

std::map<std::string, vgui::Panel *> panels;

const char *Hook_IGameResources_GetPlayerName(int client);
void Hook_IPanel_PaintTraverse(vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce);
void Hook_IPanel_SendMessage(vgui::VPANEL vguiPanel, KeyValues *params, vgui::VPANEL ifromPanel);

inline int ColorRangeRestrict(int color);
inline void FindAndReplaceInString(std::string &str, const std::string &find, const std::string &replace);
inline bool IsInteger(const std::string &s);
inline void StartAnimationSequence(const char *sequenceName);

class StatusSpecPlugin: public IServerPluginCallbacks
{
public:
	StatusSpecPlugin();
	~StatusSpecPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void			Unload(void);
	virtual void			Pause(void);
	virtual void			UnPause(void);
	virtual const char		*GetPluginDescription(void);	  
	virtual void			LevelInit(char const *pMapName);
	virtual void			ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(edict_t *pEntity);
	virtual void			ClientDisconnect(edict_t *pEntity);
	virtual void			ClientPutInServer(edict_t *pEntity, char const *playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(edict_t *pEdict);
	virtual PLUGIN_RESULT	ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual PLUGIN_RESULT	ClientCommand(edict_t *pEntity, const CCommand &args);
	virtual PLUGIN_RESULT	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
	virtual void			OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);
	virtual void			OnEdictAllocated(edict_t *edict);
	virtual void			OnEdictFreed(const edict_t *edict);
};