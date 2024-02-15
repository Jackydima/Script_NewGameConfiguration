#include "Script_NewGameConfiguration.h"
#include "Script.h"

gSScriptInit & GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}


static std::vector<ItemArguments> items = {};
static std::vector<ItemArguments> equipments = {};
static std::vector<std::pair<bCString,GEInt>> attributes = {};



void InitMap ( ) {
    functionMap["HP"] = "SetHitPoints";
    functionMap["MAXHP"] = "SetHitPointsMax";
    functionMap["MAXMP"] = "SetManaPointsMax";
    functionMap["MP"] = "SetManaPoints";
    functionMap["MAXSP"] = "SetStaminaPointsMax";
    functionMap["SP"] = "SetStaminaPoints";
    functionMap["STR"] = "SetStrength";
    functionMap["DEX"] = "SetDexterity";
    functionMap["INT"] = "SetIntelligence";
    functionMap["STH"] = "SetSmithing";
    functionMap["THF"] = "SetTheft";
    functionMap["ALC"] = "SetAlchemy";
    functionMap["PROT_BLADE"] = "SetProtectionBlades";
    functionMap["PROT_IMPACT"] = "SetProtectionImpact";
    functionMap["PROT_MISSILE"] = "SetProtectionMissile";
    functionMap["PROT_FIRE"] = "SetProtectionFire";
    functionMap["PROT_ICE"] = "SetProtectionIce";
    functionMap["PROT_LIGHTNING"] = "SetProtectionLightning";
    // LP Is different!

    booleanMap["false"] = false;
    booleanMap["true"] = true;
}

std::vector<bCString> splitTobCStrings ( const std::string str , char delim ) {
    std::vector<bCString> result;
    size_t start = 0;
    size_t end = str.find ( delim );

    while ( end != std::string::npos ) {
        bCString gES = bCString ( str.substr ( start , end - start ).c_str ());
        gES.Trim();
        result.push_back ( gES );
        
        start = end + 1;
        end = str.find ( delim , start );
    }
    bCString gES = bCString ( str.substr ( start , end - start ).c_str ( ) );
    gES.Trim ( );
    result.push_back ( gES );
    return result;
}

void LoadSettings() {
    eCConfigFile config = eCConfigFile();
    if ( config.ReadFile ( bCString ( "newGameSkills.ini" ) ) ) {
        //std::cout << sectionArray.GetCount ( ) << "\tAddress: " << &config << "Test\n";
        /*for ( GEInt i = 0; i < sectionArray.GetCount ( ); i++ ) {
            if ( sectionArray[i].GetSectionName() == "Items/Skills" ) {
                items = sectionArray[i];
            }
            else if ( sectionArray[i].GetSectionName()=="Attributes" ) {
                attributes = sectionArray[i];
            }
        }*/
        const char delimiter = ',';
        bTObjArray<eCConfigFile::eSConfigValue> itemsArray;
        config.GetSectionBlock ( "Items/Skills",  itemsArray);
        //std::cout << "Capacity: " << itemsArray.GetCount ( ) << "\n";
        for ( GEInt i = 0; i < itemsArray.GetCount ( ); i++ ) {
            bCString key = bCString ( itemsArray[i].m_pstrKey->GetText());
            bCString value = bCString ( itemsArray[i].m_pstrValue->GetText ( ) );
            //std::cout << "Key: " << key << "\n";
            //std::cout << "Value: " << value << "\n";
            Template item = Template ( key );
            if ( !item.IsValid ( ) ) continue;
            //std::cout << "Trought Template\n";
            std::string sValue = value;
            std::vector<bCString> args = splitTobCStrings ( sValue , delimiter );
            ItemArguments itemArgs;
            //std::cout << "Before Try\n";
            try {
                //std::cout << " Help ????\n";
                itemArgs = {
                    key,
                    std::stoul ( args.at ( 0 ).GetText ( ) ),
                    std::atoi ( args.at(1).GetText ( ) ),
                    ( std::atoi ( args.at ( 2 ).GetText ( ) ) ) % 10,
                    booleanMap[args.at(3).GetText ( )]
                };
                //std::cout << " Help ????\n";
            }
            catch ( std::exception e) {
                //std::cout << "Exception in Skills and items :(\t" << e.what ( ) << "\n";
                continue;
            }
            //std::cout << "\nPushing on vectore men Try: " << "\n";
            items.push_back ( itemArgs );
            //std::cout << "\nAfter Try: " << "\n";
        }
        bTObjArray<eCConfigFile::eSConfigValue> equipmentArray;
        config.GetSectionBlock ( "Equipment" , equipmentArray );
        for ( GEInt i = 0; i < equipmentArray.GetCount ( ); i++ ) {
            bCString key = bCString ( equipmentArray[i].m_pstrKey->GetText ( ) );
            bCString value = bCString ( equipmentArray[i].m_pstrValue->GetText ( ) );
            Template item = Template ( key );
            if ( !item.IsValid ( ) ) continue;
            std::string sValue = value;
            std::vector<bCString> args = splitTobCStrings ( sValue , delimiter );
            ItemArguments itemArgs;
            try {
                itemArgs = {
                    key,
                    std::stoul ( args.at ( 0 ).GetText ( ) ),
                    std::atoi ( args.at ( 1 ).GetText ( ) ),
                    (std::atoi ( args.at ( 2 ).GetText ( ) )) % 10,
                    booleanMap[args.at ( 3 ).GetText ( )]
                };
            }
            catch ( std::exception e ) {
                continue;
            }
            equipments.push_back ( itemArgs );
        }
        bTObjArray<eCConfigFile::eSConfigValue> attributesArray;
        config.GetSectionBlock ( "Attributes" , attributesArray );
        //std::cout << "Capacity: " << attributesArray.GetCount ( ) << "\n";
        for ( GEInt i = 0; i < attributesArray.GetCount ( ); i++ ) {
            bCString key = bCString ( attributesArray[i].m_pstrKey->GetText ( ) );
            bCString value = bCString ( attributesArray[i].m_pstrValue->GetText ( ) );
            //std::cout << "Key: " << key << "\n";
            //std::cout << "Value: " << value << "\n";
            GEInt iValue = 0;
            try {
                iValue = std::atoi ( value );
            }
            catch ( std::exception e ) {
                //std::cout << "Exception in Attributes :(\t" << e.what ( ) << "\n";
                continue;
            }
            attributes.push_back ( {key, iValue} );
        }
    }
}

void GE_STDCALL StartAttributes ()
{
    Entity player = Entity::GetPlayer ( );
    gCScriptAdmin& sA = GetScriptAdmin ( );
    for each ( std::pair<bCString,GEInt> var in attributes )
    {
        if ( var.first == "LP" ) {
            player.PlayerMemory.AccessProperty<PSPlayerMemory::PropertyLPAttribs> ( ) = var.second;
            continue;
        }
        if ( var.first == "LVL" ) {
            player.NPC.AccessProperty<PSNpc::PropertyLevel> ( ) = var.second;
            continue;
        }
        if ( var.first == "EXP" ) {
            player.PlayerMemory.AccessProperty<PSPlayerMemory::PropertyXP> ( ) = var.second;
            continue;
        }
        if ( functionMap[var.first] == "" ) continue; // If nothing matches then Skip script call
        //std::cout << "ATTRIBUTE: " << var.first.GetText ( ) << "\t" << "Result: " << functionMap[var.first].GetText ( ) << "\n";
        sA.CallScriptFromScript ( functionMap[var.first].GetText() , &player , &None , var.second );
    }
}


GEI32 GE_STDCALL StartItems ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs ) {
    INIT_SCRIPT_EXT ( entity1 , entity2 );

    /*
        Config should be like:
        ItemName/PerkName/SpellName=<QualityInteger>,<AmountInteger>,<HotKeySlotInteger>,<LearnedBoolean>
        It_Perk_1H_1=0,1,1,true
    */

    //std::cout << "StartItems" << "\n";

    Entity player = Entity::GetPlayer ( );
    
    for each ( ItemArguments var in items )
    {
        //std::cout << "Template: " << var.templateName.GetText ( ) << "\n";
        Template temp = Template(var.templateName);
        if ( !temp.IsValid ( ) ) continue;
        player.Inventory.AssureItemsEx ( temp , var.quality , var.amount , var.hotkey , var.learned );
    }
    for each ( ItemArguments var in equipments )
    {
        Template temp = Template ( var.templateName );
        if ( !temp.IsValid ( ) ) continue;
        GEInt stackIndex = player.Inventory.FindStackIndex ( temp );
        if ( stackIndex == -1 ) {
            stackIndex = player.Inventory.AssureItemsEx ( temp , var.quality , var.amount , var.hotkey , var.learned );
        }
        player.Inventory.EquipStack( stackIndex );
        
    }

    return 1;
}
extern "C" __declspec( dllexport )
gSScriptInit const * GE_STDCALL ScriptInit( void )
{
    // Ensure that that Script_Game.dll is loaded.
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

    InitMap();
    LoadSettings();

    static mCCallHook Hook_StartAttributes;
    Hook_StartAttributes
        .Prepare ( RVA_ScriptGame ( 0xad405 ) , &StartAttributes , mCBaseHook::mEHookType_OnlyStack )
        .ReplaceSize ( 0xad5c4 - 0xad405 ).Hook();

    static mCFunctionHook Hook_StartItems;
    Hook_StartItems.Prepare ( RVA_ScriptGame ( 0x784a0 ) , &StartItems , mCBaseHook::mEHookType_OnlyStack ).
        Hook();

    return &GetScriptInit();
}

//
// Entry Point
//

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID )
{
    switch( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        //AllocConsole ( );
        //freopen_s ( ( FILE** )stdout , "CONOUT$" , "w" , stdout );
        ::DisableThreadLibraryCalls( hModule );
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
