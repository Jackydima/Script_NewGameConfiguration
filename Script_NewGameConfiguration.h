#pragma once

#include <iostream>
#include <map>
#include "Script.h"
#include "util/Memory.h"
#include "util/Logging.h"
#include "util/Hook.h"
#include <vector>
#include <string>

std::vector<bCString> splitTobCStrings (const std::string str, char delim);

gSScriptInit & GetScriptInit();
void GE_STDCALL StartAttributes ( );
GEI32 GE_STDCALL StartItems ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );
void LoadSettings ( );

static std::map<bCString , bCString> functionMap = {};
static std::map<bCString , bool> booleanMap = {};
void InitMap();

struct ItemArguments {
	bCString templateName;
	GEU32 quality;
	GEInt amount;
	GEInt hotkey;
	GEBool learned;
	GEInt equipSlot;
};