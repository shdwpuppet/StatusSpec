/*
 *  entities.h
 *  StatusSpec project
 *  
 *  Copyright (c) 2014 thesupremecommander
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#pragma once

#include "stdafx.h"

#include <cstdarg>

#include "cdll_client_int.h"
#include "client_class.h"
#include "ehandle.h"
#include "icliententity.h"
#include "shareddefs.h"

#include "ifaces.h"

#define MAX_WEAPONS 48

#define MAKE_PTR(cast, ptr, addValue) (cast)((unsigned long) (ptr) + (unsigned long) (addValue))
#define ENTITY_INDEX_FROM_ENTITY_OFFSET(entity, offset) reinterpret_cast<CHandle<C_BaseEntity>*>(MAKE_PTR(char*, entity, offset))->GetEntryIndex()

class Entities {
public:
	static int pCTFPlayer__m_iClass;
	static int pCTFPlayer__m_iTeamNum;
	static int pCTFPlayer__m_nPlayerCond;
	static int pCTFPlayer___condition_bits;
	static int pCTFPlayer__m_nPlayerCondEx;
	static int pCTFPlayer__m_nPlayerCondEx2;
	static int pCTFPlayer__m_hActiveWeapon;
	static int pCTFPlayer__m_hMyWeapons[MAX_WEAPONS];
	static int pCEconEntity__m_hOwnerEntity;
	static int pCEconEntity__m_iItemDefinitionIndex;
	static int pCWeaponMedigun__m_bChargeRelease;
	static int pCWeaponMedigun__m_nChargeResistType;
	static int pCWeaponMedigun__m_flChargeLevel;
	static int pCTFPlayerResource__m_iHealth[MAX_PLAYERS + 1];
	static int pCTFPlayerResource__m_iMaxHealth[MAX_PLAYERS + 1];
	static int pCTFPlayerResource__m_iMaxBuffedHealth[MAX_PLAYERS + 1];
	static int pCTFPlayerResource__m_iKillstreak[MAX_PLAYERS + 1];
	static int pCWeaponMedigun__m_bHealing;
	static int pCWeaponMedigun__m_hHealingTarget;
	static int pCTFPlayer__m_iKillStreak;
	static int pCTFGrenadePipebombProjectile__m_iType;

	static bool PrepareOffsets();
	static bool GetClassPropOffset(const char *className, int &offset, int depth, ...);
	static bool GetSubProp(RecvTable *table, const char *propName, RecvProp *&prop, int &offset);
	static bool CheckClassBaseclass(ClientClass *clientClass, const char *baseclassDataTableName);

private:
	static bool CrawlForPropOffset(RecvTable *sTable, const char *propName, int &offset);
	static bool CrawlForArrayEnt(RecvTable *sTable, const char *propName, int element, int &offset);
};