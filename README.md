StatusSpec
==========

a Team Fortress 2 client plugin that augments game spectating

![screenshot](http://i.imgur.com/HliUKLk.png)

Changelog
---------

**WIP**
* local player
  * add command to track spectator target
  * fix warning being emitted on successful changes
* medigun info
  * change how progress bars are set (requires changes to MedigunInfo.res)
  * add ability to have custom meter settings based on medigun type

**0.15.0**
* antifreeze
  * now supports displaying a message when a freeze is occurring
* killstreaks
  * expand killstreak display to the scoreboard
  * fix killstreaks causing post-game crashes
* local player
  * new module - allows a certain player's actions to be highlighted by the killfeed
* multipanel
  * new module - allows the console and scoreboard to be opened on top of the game
* player outlines
  * make player outline calculations more efficient
  * add option to disable health-based calculations

**0.14.1**
* fix glitches with killstreaks on death
* add command to help stop flickering issues with player outlines

**0.14.0**
* add killstreaks module

**0.13.1**
* fix bleed icons not showing up

**0.13.0**
* add support for health-based team colored outlines

**0.12.1**
* fix medigun info not disappearing properly
* make unloading/pausing/unpausing cleaner and hopefully less error-prone

**0.12.0**
* add support for HUD features required for casting HUDs
* add ability to set progress direction via resource file and removed command
* add setting to allow Vaccinator to have only one charge meter
* add ability to view medigun charge advantage
* fix issues with medigun finding

**0.11.3**
* fix issues with player outlines not changing color after reloads

**0.11.2**
* fix player outline issues with players not being hooked/unhooked properly

**0.11.1**
* update offsets for TF2 update on 2014-06-11

**0.11.0**
* reorganize entire plugin into modules (changing many console variables and commands)
* add player outlines module

**0.10.0**
* fix player alias incompatibility with Spec Tools
* add utility command for setting the direction of a progress bar

**0.9.0**
* implement medigun info box

**0.8.0**
* implement player aliasing

**0.7.0**
* implement loadout icons

**0.6.0**
* implement status icons as part of spectator GUI

**0.5.0**
* added option to force refreshing of spectator GUI (partially to fully fixes post-pause issues, YMMV)

**0.4.0**
* added option (enabled by default) to have icon backgrounds adjust with icons rather than be static

**0.3.0**
* scale HUD positioning values automatically

**0.2.0**
* added Vaccinator resistances/Ubers, Soldier banners, Jarate, Mad Milk, bleeding, and on fire icons to display

**0.1.0**
* initial version
* Uber, Kritz, marked for death status icons
* many CVars for customizing appearance

Usage
-----
To install, place the `StatusSpec` folder within the `custom` folder in the `tf` directory. Because this plugin is not signed by Valve, you must launch TF2 with the `-insecure` option in order for this plugin to be active and will not be able to use this plugin on VAC-secured servers.

### AntiFreeze
*forces spectator GUI to refresh constantly, eliminating many HUD issues after game unpauses*

#### Console Variables
* `statusspec_antifreeze_display` - enables display of an info panel when a freeze is detected
* `statusspec_antifreeze_display_threshold` - the time of a freeze (in seconds) before the info panel is displayed
* `statusspec_antifreeze_enabled` - enable antifreeze (forces the spectator GUI to refresh)

#### Console Commands
* `statusspec_antifreeze_display_reload_settings` - reload settings for the freeze info panel from the resource file

#### UI Resource Files
An example file for the medigun info box is included under `Resource/UI/FreezeInfo.res`. This HUD cannot be refreshed using the normal `hud_reloadscheme` because it isn't natively implemented into TF2, and thus the command `statusspec_antifreeze_display_reload_settings` is provided as a replacement.

### Killstreaks
*enables killstreak tracking for all weapons, all players*

#### Console Variables
* `statusspec_killstreaks_enabled` - enable killstreaks display

### Loadout Icons
*displays icons to represent weapons in a player's loadout*

#### Console Variables
* `statusspec_loadouticons_enabled` - enable loadout icons
* `statusspec_loadouticons_nonloadout` - enable loadout icons for nonloadout items

#### Console Commands
* `statusspec_loadouticons_filter_active <red> <green> <blue> <alpha>` - the RGBA filter applied to the icon for an active item
* `statusspec_loadouticons_filter_nonactive <red> <green> <blue> <alpha>` - the RGBA filter applied to the icon for a nonactive item

#### UI Resource Files
To properly support loadout icons, adjust `Resource/UI/SpectatorTournament.res` so that it includes something similar to the `loadouticons` section below contained within the `playerpanels_kv` section of the `specgui` section, as demonstrated below. You will also need to adjust the rest of the file as necessary, since this will act as a normal GUI panel. The icons will be entirely contained within this element as defined in this configuration, and each icon will be a square size of `tall`. The rendering of this panel only displays icons and will not adjust anything else, no matter whether this feature is enabled or disabled - thus, you will be required to manually adjust your HUD as appropriate when enabling or disabling this feature.
```
"Resource/UI/SpectatorTournament.res"
{
	"specgui"
	{
		...
	       
		"playerpanels_kv"
		{
			...
			
			"loadouticons"
			{
				"ControlName"	"Panel"
				"fieldName"		"loadouticons"
				"xpos"			"16"
				"ypos"			"0"
				"zpos"			"2"
				"wide"			"60"
				"tall"			"15"
				"visible"		"1"
				"enabled"		"1"
			}
		}
	}
	
	...	
}
```

### Local Player
*changes the local player locally to affect certain HUD elements like the killfeed*

#### Console Variables
* `statusspec_localplayer_enabled` - enable local player override
* `statusspec_localplayer_player` - player to set as the local player

#### Console Commands
* `statusspec_localplayer_set_current_target` - set the local player to the current spectator target

### Medigun Info
*displays info about mediguns for both teams*

#### Console Variables
* `statusspec_mediguninfo_dynamic_meters` - enable charge meters to change based on medigun
* `statusspec_mediguninfo_enabled` - enable medigun info
* `statusspec_mediguninfo_individual_charge_meters` - enable individual charge meters (for Vaccinator)

#### Console Commands
* `statusspec_mediguninfo_reload_settings` - reload settings for the medigun info HUD from the resource file

#### UI Resource Files
An example file for the medigun info box is included under `Resource/UI/MedigunInfo.res`. This HUD cannot be refreshed using the normal `hud_reloadscheme` because it isn't natively implemented into TF2, and thus the command `statusspec_mediguninfo_reload_settings` is provided as a replacement.

In addition, the following HUD animations are triggered by this plugin and may be used to show events on the HUD:
* `MedigunInfoBluChargeReady` - triggered when a BLU medigun charge is ready for use
* `MedigunInfoBluChargeReleased` - triggered when a BLU medigun charge has been released/popped
* `MedigunInfoBluChargeStop` - triggered when the BLU medigun charge is no longer ready/released
* `MedigunInfoRedChargeReady` - triggered when a RED medigun charge is ready for use
* `MedigunInfoRedChargeReleased` - triggered when a RED medigun charge has been released/popped
* `MedigunInfoRedChargeStop` - triggered when the RED medigun charge is no longer ready/released (after a charge is completed/dropped)

### Multipanel
*displays panels on top of the main game*

#### Console Variables
* `statusspec_multipanel_console` - displays a console in the HUD
* `statusspec_multipanel_scoreboard` - displays the scoreboard

### Player Aliases
*substitutes in user-defined aliases for player names*

#### Console Variables
* `statusspec_playeraliases_enabled` - enable player aliases

#### Console Commands
* `statusspec_playeraliases_get <steamid>` - get an alias for a player
* `statusspec_playeraliases_remove <steamid>` - remove an alias for a player
* `statusspec_playeraliases_set <steamid> <alias>` - set an alias for a player

### Player Outlines
*displays bright outlines around players that can be seen through walls*

#### Console Variables
* `statusspec_playeroutlines_enabled` - enable player outlines
* `statusspec_playeroutlines_frequent_override_enabled` - enable more frequent player outline overrides (helps stop flickering at cost of performance)
* `statusspec_playeroutlines_health_adjusted_team_colors` - adjusts team colors depending on health of players
* `statusspec_playeroutlines_team_colors` - override default health-based outline colors with team colors

#### Console Commands
* `statusspec_playeroutlines_color_blu_buff <red> <green> <blue>` - the color used for outlines for BLU team players at max buffed health
* `statusspec_playeroutlines_color_blu_full <red> <green> <blue>` - the color used for outlines for BLU team players at full health
* `statusspec_playeroutlines_color_blu_low <red> <green> <blue>` - the color used for outlines for BLU team players at low health
* `statusspec_playeroutlines_color_blu_medium <red> <green> <blue>` - the color used for outlines for BLU team players at medium health
* `statusspec_playeroutlines_color_red_buff <red> <green> <blue>` - the color used for outlines for RED team players at max buffed health
* `statusspec_playeroutlines_color_red_full <red> <green> <blue>` - the color used for outlines for RED team players at full health
* `statusspec_playeroutlines_color_red_low <red> <green> <blue>` - the color used for outlines for RED team players at low health
* `statusspec_playeroutlines_color_red_medium <red> <green> <blue>` - the color used for outlines for RED team players at medium health
* `statusspec_playeroutlines_force_refresh` - force the player outlines to refresh

### Status Icons
*displays icons representing special statuses of a player*

#### Console Variables
* `statusspec_statusicons_enabled` - enable status icons
* `statusspec_statusicons_max_icons` - maximum number of icons to show

#### UI Resource Files
To properly support status icons, adjust `Resource/UI/SpectatorTournament.res` so that it includes something similar to the `statusicons` section below contained within the `playerpanels_kv` section of the `specgui` section, as demonstrated below. The icons will appear on the right of `xpos` with a square size of `tall`, and the player panel will also be expanded to accommodate these icons. The rendering of this panel assumes that this element is on the right edge of the player panel and will not appear if status icons are disabled.
```
"Resource/UI/SpectatorTournament.res"
{
	"specgui"
	{
		...
	       
		"playerpanels_kv"
		{
			...
			
			"statusicons"
			{
				"ControlName"	"Panel"
				"fieldName"		"statusicons"
				"xpos"			"160"
				"ypos"			"0"
				"zpos"			"8"
				"wide"			"0"
				"tall"			"15"
				"visible"		"1"
				"enabled"		"1"
			}
		}
	}
	
	...	
}
```

Thanks
------
**bluee** - for the inspiration to do this project and for a lot of code reused from his [AdvSpec project](https://github.com/MattMcNam/advspec)

**AlliedModders** - for in-depth information about the internals of the Source Engine and for [SourceHook](https://github.com/alliedmodders/metamod-source/tree/master/core/sourcehook)

**Tsuda Kageyu, RaMMicHaeL, et al.** - for [MinHook](https://github.com/RaMMicHaeL/minhook)