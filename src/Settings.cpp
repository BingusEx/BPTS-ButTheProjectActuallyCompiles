#include "Settings.h"
#include "FocusManager.h"
#include "API/Compatibility.h"
#include "UI/SelectionWidget.h"
#include "Filters/FilterManager.h"

#include <toml++/toml.h>
#include <iostream>

#include "lib/Util.h"

bool Settings::EnableFiltersForNativeSelection = false;

bool Settings::IsEnabledInFirstPerson = false;
bool Settings::IsEnabledInThirdPerson = true;
bool Settings::IsEnabledOnHorseBack = true;
bool Settings::IsEnabledInCombat = true;
bool Settings::IsEnabledWithWeaponsDrawn = true;

bool Settings::OnlyAllowStealingWhileSneaking = false;
bool Settings::NoFurnitureWhileSneaking = false;

bool Settings::IsWidget3DEnabledInFirstPerson = false;
bool Settings::IsWidget3DEnabledInThirdPerson = true;
bool Settings::IsWidget3DEnabledOnHorseBack = true;
bool Settings::IsWidget3DEnabledInCombat = true;
bool Settings::IsWidget3DEnabledWithWeaponsDrawn = true;

bool Settings::IsDividerEnabled = true;
bool Settings::IsActivationButtonEnabled = true;
bool Settings::IsItemInfoEnabled = true;
bool Settings::IsDismountProgressCirlcleEnabled = true;
double Settings::WidgetAlpha = 100.0;

bool Settings::EnableObjectCycle = true;
bool Settings::EnableContinuousObjectCycle = true;

int Settings::ObjectCycleModifierKey = 42; //left shift

int Settings::ObjectCycleUpKey = 264; //mousewheel up
int Settings::ObjectCycleDownKey = 265; //mousewheel down

int Settings::CycleDelayMS = 30;

double Settings::WidgetFadeInDelta = 0.05;
double Settings::WidgetFadeOutDelta = 0.05;

double Settings::WidgetZOffset = 30.0;
double Settings::WidgetZOffsetFirstPerson = 5.0;

double Settings::WidgetZOffsetAdditionalNPC = 0.0;

double Settings::WidgetSizeMin = 0.2;
double Settings::WidgetSizeMax = 1.25;

double Settings::WidgetSizeMin_HorseBack = 0.5;
double Settings::WidgetSizeMax_HorseBack = 1.75;

float Settings::DebugDrawThickness = 1.0;
float Settings::DebugDrawAlpha = 1.0;

bool Settings::Draw_RayTraces = true;
bool Settings::Draw_Bounds_Box = true;
bool Settings::Draw_Bounds_Sphere = true;
bool Settings::Draw_Bounds_MinMax = false;

bool Settings::HotReloadConfig = false;
bool Settings::ShowFurnitureMarkers = false;

bool Settings::IsHoldToDismountEnabled = true;
double Settings::HoldToDismountTime = 0.3;
double Settings::HoldToDismountMinTime = 0.2;

bool Settings::AdjustMoreHudWidgets = true;
bool Settings::AdjustQuickLootWidgets = false;

// -- -- //
bool Settings::Widget3DEnabled = true;

bool Settings::Last_ControlsEnabled = true;
bool Settings::Last_FreeCamEnabled = false;
bool Settings::Last_FirstPerson = false;

std::unordered_map<std::string, bool> Settings::ObjectNameIgnoreList;
std::unordered_map<std::string, ObjectOverride> Settings::ObjectOverrides;

// -- -- //

const std::string Settings::ConfigPath = "Data/MCM/Settings/BetterThirdPersonSelection.ini"s;
const std::string Settings::ConfigDefaultsPath = "Data/MCM/Config/BetterThirdPersonSelection/settings.ini"s;

const std::string Settings::ObjectOverridesContainingPath = "Data/MCM/Config/BetterThirdPersonSelection/Overrides/"s;
const std::string Settings::ObjectOverridesDefaultPath = "Data/MCM/Config/BetterThirdPersonSelection/Overrides/Overrides_Default.toml"s;

const std::string Settings::FilterPresetsContainingPath = "Data/MCM/Config/BetterThirdPersonSelection/Filters/"s;
const std::string Settings::FilterPresetsDefaultPath = "Data/MCM/Config/BetterThirdPersonSelection/Filters/FilterPresets_Default.toml"s;

const std::string Settings::FilterStatesPath = "Data/MCM/Settings/FilterStates.json"s;

std::mutex m_widget3D;

bool Settings::GetWidget3DEnabled()
{
	std::lock_guard lock(m_widget3D);
	return Widget3DEnabled;
}

void Settings::SetWidget3DEanbled(bool enabled)
{
	std::lock_guard lock(m_widget3D);
	Widget3DEnabled = enabled;
}

bool Settings::IsNameInIgnoreList(std::string nameIn)
{
    return ObjectNameIgnoreList.find(nameIn) != ObjectNameIgnoreList.end();
}

bool Settings::GetObjectOverride(RE::TESObjectREFR* objectRef, ObjectOverride& objOverrideOut)
{
    if (!objectRef)
        return false;

	auto boundObj = objectRef->GetObjectReference();
    if (!boundObj)
        return false;

    auto model = boundObj->As<RE::TESModel>();
    if (!model)
        return false;

	auto modelName = model->GetModel();
	auto displayName = objectRef->GetDisplayFullName();
    return GetObjectOverrideByName(modelName, displayName, objOverrideOut);
}

bool Settings::GetObjectOverrideByModelName(std::string modelName, ObjectOverride& objOverrideOut)
{
    Util::ToLowerString(modelName);

	auto objOverride = ObjectOverrides.find(modelName);
	if (objOverride == ObjectOverrides.end())
        return false;

	objOverrideOut = objOverride->second;
    return true;
}

bool Settings::GetObjectOverrideByDisplayName(std::string displayName, ObjectOverride& objOverrideOut)
{
    Util::ToLowerString(displayName);

    auto objOverride = ObjectOverrides.find(displayName);
    if (objOverride == ObjectOverrides.end())
        return false;

    objOverrideOut = objOverride->second;
    return true;
}

bool Settings::GetObjectOverrideByName(std::string modelName, std::string displayName, ObjectOverride& objOverrideOut)
{
    if (GetObjectOverrideByModelName(modelName, objOverrideOut))
        return true;
    return GetObjectOverrideByDisplayName(displayName, objOverrideOut);
}

bool Settings::HasObjectOverride(std::string modelName, std::string displayName)
{
    Util::ToLowerString(modelName);

	auto objOverrideModel = ObjectOverrides.find(modelName);
    if (objOverrideModel != ObjectOverrides.end())
        return true;

    Util::ToLowerString(displayName);

    auto objOverrideDisplay = ObjectOverrides.find(displayName);
    return objOverrideDisplay != ObjectOverrides.end();
}

double Settings::GetCurrentWidgetZOffset()
{
	if (Util::IsPlayerInFirstPerson())
		return WidgetZOffsetFirstPerson;
	return WidgetZOffset;
}

double Settings::GetWidgetSizeMin()
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();

	if (!playerChar || !playerChar->IsOnMount())
		return WidgetSizeMin;
	return WidgetSizeMin_HorseBack;
}

double Settings::GetWidgetSizeMax()
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();

	if (!playerChar || !playerChar->IsOnMount())
		return WidgetSizeMax;
	return WidgetSizeMax_HorseBack;
}

// -- -- //

std::mutex m_updateSettings;
void Settings::UpdateSettings()
{
	std::lock_guard lock(m_updateSettings);

	auto playerCam = RE::PlayerCamera::GetSingleton();
	if (!playerCam || !playerCam->currentState)
	{
		logger::warn("BTPS: Cannot update game settings: failed to find player camera state");
		return;
	}

	auto playerChar = RE::PlayerCharacter::GetSingleton();
	if (!playerChar)
		return;

	// disable selector (but not 3D widget) when player is in beast form
	// enabling custom selection in beast form would require reversing the native
	// game's werewolf crosshair pick and replicating that. And since the game only allows
	// interacting with pretty big things in beast form anyways (actors, doors, activators)
	// it just doesn't seem worth the effort
	auto menuCtrls = RE::MenuControls::GetSingleton();
	bool isInBeastForm = menuCtrls && menuCtrls->InBeastForm();

	bool isInFreeCam = IsInFreeCamera();

	bool isCommanding = playerChar->GetActorDoingPlayerCommand() != nullptr;//playerChar->HasActorDoingCommand();

	//auto controlMap = RE::ControlMap::GetSingleton();
	// some tests to disable activation while controls are disabled. Don't work right now
	bool controlsEnabled = 
		!playerChar->AsActorState()->IsBleedingOut() &&
		!playerChar->IsInKillMove() &&
		!playerChar->GetActorRuntimeData().boolFlags.all(RE::Actor::BOOL_FLAGS::kMovementBlocked);// &&
		//!playerChar->IsInRagdollState();
		//controlMap &&
		//controlMap->IsMovementControlsEnabled() &&
        //controlMap->IsConsoleControlsEnabled() &&
        //controlMap->IsMainFourControlsEnabled();*/
		//controlMap->IsActivateControlsEnabled();

	bool onMount = playerChar->IsOnMount();
	bool mountState = !(onMount && !Settings::IsEnabledOnHorseBack);
	bool mountStateWidget = !(onMount && !Settings::IsWidget3DEnabledOnHorseBack);

	bool inCombat = playerChar->IsInCombat();
	bool combatState = !(inCombat && !Settings::IsEnabledInCombat);
	bool combatStateWidget = !(inCombat && !Settings::IsWidget3DEnabledInCombat);
	
	bool weaponDrawn = playerChar->AsActorState()->IsWeaponDrawn();
	bool weaponDrawnState = !(weaponDrawn && !Settings::IsEnabledWithWeaponsDrawn);
	bool weaponDrawnStateWidget = !(weaponDrawn && !Settings::IsWidget3DEnabledWithWeaponsDrawn);

	bool selectorState = !isInBeastForm && !isInFreeCam && !isCommanding && mountState && combatState && weaponDrawnState && controlsEnabled;
	bool widgetState = !isInFreeCam && mountStateWidget && !isCommanding && combatStateWidget && weaponDrawnStateWidget && controlsEnabled;

	if (Util::IsPlayerInFirstPerson())
	{
		FocusManager::SetIsEnabled(Settings::IsEnabledInFirstPerson && selectorState);
		SetWidget3DEanbled(Settings::IsWidget3DEnabledInFirstPerson && widgetState);

		if (!Last_FirstPerson)
		{
			SelectionWidget::OnPerspectiveToggled(true);
			Last_FirstPerson = true;
		}
	}
	else
	{
		FocusManager::SetIsEnabled(Settings::IsEnabledInThirdPerson && selectorState);
		SetWidget3DEanbled(Settings::IsWidget3DEnabledInThirdPerson && widgetState);

		if (Last_FirstPerson)
		{
			SelectionWidget::OnPerspectiveToggled(false);
			Last_FirstPerson = false;
		}
	}

	// hide completely with controls disabled
	if (controlsEnabled != Last_ControlsEnabled)
	{
		SelectionWidget::OnControlsToggled(controlsEnabled);
		Last_ControlsEnabled = controlsEnabled;
	}

	// hide completely while in free cam mode
	if (isInFreeCam != Last_FreeCamEnabled)
	{
		SelectionWidget::OnFreeCamToggled(isInFreeCam);
		Last_FreeCamEnabled = isInFreeCam;
	}
}

bool Settings::IsInFreeCamera()
{
	auto playerCam = RE::PlayerCamera::GetSingleton();
	if (!playerCam || !playerCam->currentState)
		return false;

	return playerCam->currentState->id == RE::CameraState::kFree;
}

void Settings::InitObjectOverrides()
{
	ObjectNameIgnoreList.clear();
    ObjectOverrides.clear();

	ReadObjectOverrideFile(ObjectOverridesDefaultPath);

	auto overridePaths = FindObjectOverrideFiles();
	for (auto path : overridePaths)
	{
		ReadObjectOverrideFile(path);
	}
}

void Settings::ReadObjectOverrideFile(std::string path)
{
	try
	{
		std::ifstream file(path);
		if (file)
		{
			std::stringstream ss;
			ss << file.rdbuf();
			file.close();

			toml::table tbl = toml::parse(ss);
 			ReadObjectOverrideTable(tbl);

			logger::info("BTPS: finished reading ObjectOverrides '{}'", path);
		}
		else
			logger::error("BTPS: failed to load ObjectOverrides - can't open file '{}'", path);
	}
	catch (const toml::parse_error& e)
	{
        std::ostringstream ss;
		ss
			<< '\t' << e.description() << '\n'
			<< "\t\t(" << e.source().begin << ')';

		std::string errorString = ss.str();
        logger::error("BTPS: error parsing Override file:\n{} - {}", path, errorString);
	}
}

void Settings::ReadObjectOverrideTable(const toml::table& tbl)
{
    ReadIgnoreList(tbl);
	ReadObjectOverrides(tbl);
}

void Settings::ReadSettings(const std::string path)
{
	try
	{
		std::ifstream file(path);
		if (file)
		{
			std::stringstream ss;
			ss << file.rdbuf();
			file.close();

			toml::table tbl = toml::parse(ss);

			ReadTable(tbl);
		}
		else
			logger::warn("BTPS: failed to load settings - can't open file '{}'", path);
	}
	catch (const toml::parse_error& e)
	{
		std::ostringstream ss;
		ss
			<< "BTPS: error parsing config file \'" << *e.source().path << "\':\n"
			<< '\t' << e.description() << '\n'
			<< "\t\t(" << e.source().begin << ')';
		logger::error(fmt::runtime(ss.str()));
	}
}

void Settings::ReadSettings()
{
	// read the default settings, then apply the changed settings on top
	ReadSettings(ConfigDefaultsPath);
	ReadSettings(ConfigPath);

	logger::info("BTPS: finished reading settings");
}

void Settings::ReadTable(const toml::table& tbl)
{
	auto tabGeneral = tbl.get_as<toml::table>("General");
	auto tabWidgets = tbl.get_as<toml::table>("Widgets");
	auto tabDebug = tbl.get_as<toml::table>("Debug");
	auto tabPriority = tbl.get_as<toml::table>("Priority");
	auto tabPerformance = tbl.get_as<toml::table>("Performance");
	auto tabCompat = tbl.get_as<toml::table>("Compatibility");

	if (tabGeneral)
	{
		auto subTbl = tabGeneral->ref<toml::table>();

		ReadBool(subTbl, "bEnableFiltersForNativeSelection", EnableFiltersForNativeSelection);

		ReadBool(subTbl, "bIsEnabledInFirstPerson", IsEnabledInFirstPerson);
		ReadBool(subTbl, "bIsEnabledInThirdPerson", IsEnabledInThirdPerson);
		ReadBool(subTbl, "bIsEnabledOnHorseBack", IsEnabledOnHorseBack);
		ReadBool(subTbl, "bIsEnabledInCombat", IsEnabledInCombat);
		ReadBool(subTbl, "bIsEnabledWithWeaponsDrawn", IsEnabledWithWeaponsDrawn);

		ReadBool(subTbl, "bOnlyAllowStealingWhileSneaking", OnlyAllowStealingWhileSneaking);
		ReadBool(subTbl, "bNoFurnitureWhileSneaking", NoFurnitureWhileSneaking);

		ReadBool(subTbl, "bEnableObjectCycle", EnableObjectCycle);
		ReadBool(subTbl, "bEnableContinuousObjectCycle", EnableContinuousObjectCycle);

		ReadInt(subTbl, "iObjectCycleModifierKey", ObjectCycleModifierKey);
		ReadInt(subTbl, "iCycleUpKey", ObjectCycleUpKey);
		ReadInt(subTbl, "iCycleDownKey", ObjectCycleDownKey);
		ReadInt(subTbl, "iCycleDelayMS", CycleDelayMS);

		ReadFloat(subTbl, "fMaxAngleDif", FocusManager::MAX_ANGLE_DIFFERENCE);
		ReadFloat(subTbl, "fMaxAngleDif_HorseBack", FocusManager::MAX_ANGLE_DIFFERENCE_MOUNTED);

		ReadFloat(subTbl, "fMaxInteractionRange", FocusManager::MAX_INTERACTION_RANGE);
		ReadFloat(subTbl, "fMaxInteractionRange_HorseBack", FocusManager::MAX_INTERACTION_RANGE_MOUNTED);

		ReadBool(subTbl, "bIsHoldToDismountEnabled", IsHoldToDismountEnabled);
		ReadDouble(subTbl, "fHoldToDismountTime", HoldToDismountTime);
        ReadDouble(subTbl, "fHoldToDismountMinTime", HoldToDismountMinTime);
	}

	if (tabWidgets)
	{
		auto subTbl = tabWidgets->ref<toml::table>();

		ReadBool(subTbl, "bIsWidget3DEnabledInFirstPerson", IsWidget3DEnabledInFirstPerson);
		ReadBool(subTbl, "bIsWidget3DEnabledInThirdPerson", IsWidget3DEnabledInThirdPerson);
		ReadBool(subTbl, "bIsWidget3DEnabledOnHorseBack", IsWidget3DEnabledOnHorseBack);
		ReadBool(subTbl, "bIsWidget3DEnabledInCombat", IsWidget3DEnabledInCombat);
		ReadBool(subTbl, "bIsWidget3DEnabledWithWeaponsDrawn", IsWidget3DEnabledWithWeaponsDrawn);

		ReadBool(subTbl, "bIsDividerEnabled", IsDividerEnabled);
		ReadBool(subTbl, "bIsActivationButtonEnabled", IsActivationButtonEnabled);
		ReadBool(subTbl, "bIsItemInfoEnabled", IsItemInfoEnabled);
		ReadBool(subTbl, "bIsDismountProgressCirlcleEnabled", IsDismountProgressCirlcleEnabled);
		ReadDouble(subTbl, "fWidgetAlpha", WidgetAlpha);

		ReadDouble(subTbl, "fWidgetSizeMin", WidgetSizeMin);
		ReadDouble(subTbl, "fWidgetSizeMax", WidgetSizeMax);
		ReadDouble(subTbl, "fWidgetSizeMin_HorseBack", WidgetSizeMin_HorseBack);
		ReadDouble(subTbl, "fWidgetSizeMax_HorseBack", WidgetSizeMax_HorseBack);

		ReadDouble(subTbl, "fWidget2DSize", SelectionWidget::Widget2DSize);
		ReadFloat(subTbl, "fWidget2DXPos", SelectionWidget::Widget2DPos.x);
		ReadFloat(subTbl, "fWidget2DYPos", SelectionWidget::Widget2DPos.y);

		ReadDouble(subTbl, "fWidgetFadeInDelta", Settings::WidgetFadeInDelta);
		ReadDouble(subTbl, "fWidgetFadeOutDelta", Settings::WidgetFadeOutDelta);

		ReadDouble(subTbl, "fWidgetZOffset", WidgetZOffset);
		ReadDouble(subTbl, "fWidgetZOffsetFirstPerson", WidgetZOffsetFirstPerson);

		ReadDouble(subTbl, "fWidgetZOffsetAdditionalNPC", WidgetZOffsetAdditionalNPC);
	}

	if (tabDebug)
	{
		auto subTbl = tabDebug->ref<toml::table>();

		ReadBool(subTbl, "bDraw_RayTraces", Draw_RayTraces);
		ReadBool(subTbl, "bDraw_Bounds_Box", Draw_Bounds_Box);
		ReadBool(subTbl, "bDraw_Bounds_Sphere", Draw_Bounds_Sphere);
		ReadBool(subTbl, "bDraw_Bounds_MinMax", Draw_Bounds_MinMax);
		ReadFloat(subTbl, "fDebugDrawThickness", DebugDrawThickness);
		ReadFloat(subTbl, "fDebugDrawAlpha", DebugDrawAlpha);

		ReadBool(subTbl, "bHotReloadConfig", HotReloadConfig);
		ReadBool(subTbl, "bShowFurnitureMarkers", ShowFurnitureMarkers);
	}

	if (tabPriority)
	{
		auto subTbl = tabPriority->ref<toml::table>();
		ReadFloat(subTbl, "fAngleMult",		FocusManager::PRIORITY_ANGLE_MULT);
		ReadFloat(subTbl, "fDistMult",		FocusManager::PRIORITY_DIST_MULT);
		ReadFloat(subTbl, "fTypemult",		FocusManager::PRIORITY_TYPE_MULT);
		ReadFloat(subTbl, "fSpecificMult",	FocusManager::PRIORITY_SPECIFIC_MULT);
		ReadFloat(subTbl, "fValueMult",		FocusManager::PRIORITY_VALUE_MULT);

		ReadInt(subTbl, "iMaxValue",		FocusManager::MAX_VALUE);

		ReadFloat(subTbl, "fNativeSelectionBonus",		FocusManager::NATIVE_SELECTION_PRIORITY_BONUS);
		ReadFloat(subTbl, "fPreviousSelectionBonus",	FocusManager::PREVIOUS_SELECTION_PRIORITY_BONUS);
		ReadFloat(subTbl, "fNPCSelectionBonus",			FocusManager::NPC_SELECTION_PRIORITY_BONUS);
	}

	if (tabPerformance)
	{
		auto subTbl = tabPerformance->ref<toml::table>();

		ReadInt(subTbl, "iNumTracesHorizontal", FocusManager::NUM_TRACES_TO_OBJECT_HORIZONTAL);
		ReadInt(subTbl, "iNumTracesVertical", FocusManager::NUM_TRACES_TO_OBJECT_VERTICAL);
	}

	if (tabCompat)
	{
		auto subTbl = tabCompat->ref<toml::table>();

		ReadBool(subTbl, "bAdjustMoreHudWidgets", AdjustMoreHudWidgets);
		ReadBool(subTbl, "bAdjustQuickLootWidgets", AdjustQuickLootWidgets);

		Compatibility::MoreHUD::IsEnabled = Compatibility::MoreHUD::IsInstalled && AdjustMoreHudWidgets;
		Compatibility::QuickLootIE::IsEnabled = Compatibility::QuickLootIE::IsInstalled && AdjustQuickLootWidgets;
	}
}

bool Settings::ReadBool(const toml::table& tbl, std::string key, bool& valueOut)
{
	auto value = tbl.get_as<int64_t>(key);
	if (value)
	{
		valueOut = (bool)value->get();
		return true;
	}

	return false;
}

bool Settings::ReadInt(const toml::table& tbl, std::string key, int& valueOut)
{
	auto value = tbl.get_as<int64_t>(key);
	if (value)
	{
		valueOut = (int)value->get();
		return true;
	}

	return false;
}

bool Settings::ReadString(const toml::table& tbl, std::string key, std::string& valueOut, bool toLower)
{
    auto value = tbl.get_as<std::string>(key);
    if (value)
    {
        valueOut = value->get();
        if (toLower)
            Util::ToLowerString(valueOut);
		
        return true;
    }

    return false;
}

bool Settings::ReadDouble(const toml::table& tbl, std::string key, double& valueOut)
{
	auto value = tbl.get_as<double>(key);
	if (value)
	{
		valueOut = value->get();
		return true;
	}

	return false;
}

bool Settings::ReadFloat(const toml::table& tbl, std::string key, float& valueOut)
{
	auto value = tbl.get_as<double>(key);
	if (value)
	{
		valueOut = (float)value->get();
		return true;
	}

	return false;
}

bool Settings::ReadArray(const toml::table& tbl, std::string key, std::vector<std::string>& valueOut, bool toLower)
{
	auto tblArray = tbl.get_as<toml::array>(key);

	if (!tblArray)
		return false;

	for (auto& currNode : *tblArray)
	{
        auto nodeString = currNode.as_string();
        if (!nodeString)
            continue;

        auto currString = nodeString->get();

        if (toLower)
            Util::ToLowerString(currString);

		valueOut.push_back(currString);
	}

	return true;
}

bool Settings::ReadVector3(const toml::table& tbl, std::string key, glm::vec3& valueOut)
{
    auto vectorTbl = tbl.get_as<toml::table>(key);
    if (!vectorTbl)
        return false;
    
	ReadFloat(*vectorTbl, "x", valueOut.x);
	ReadFloat(*vectorTbl, "y", valueOut.y);
	ReadFloat(*vectorTbl, "z", valueOut.z);

	return true;
}

bool Settings::ReadIgnoreList(const toml::table& tbl)
{
    auto tblArray = tbl.get_as<toml::array>("ignore");

    if (!tblArray)
        return false;

    for (auto& currNode : *tblArray)
    {
        auto nodeString = currNode.as_string();
        if (!nodeString)
            continue;

        auto currString = nodeString->get();
        Util::ToLowerString(currString);

        ObjectNameIgnoreList.insert({ currString, true });
    }

    return true;
}

bool Settings::ReadObjectOverrides(const toml::table& tbl)
{
    auto tblArray = tbl.get_as<toml::array>("objectOverrides");
	if (!tblArray)
		return false;

	for (auto& currObj : *tblArray)
    {
        ObjectOverride currOverride;

        auto currTbl = currObj.as_table();
        if (!currTbl)
			continue;

		if (ReadString(*currTbl, "modelName", currOverride.ModelName, true))
            currOverride.HasModelName = true;

		if (ReadString(*currTbl, "displayName", currOverride.DisplayName, true))
            currOverride.HasDisplayName = true;

		Util::ToLowerString(currOverride.ModelName);

		auto cObjects = currTbl->get_as<toml::array>("clObjects");
		if (cObjects)
		{
            ReadClObjects(cObjects, currOverride.ClObjects);
		}

		// customClObjects are a way to customize collision boxes through config files. An example are
		// cupboards (clutter\\common\\cupboard01.nif): by default, they are very hard to select due to
		// the collision box spanning the entire mesh, not just the part the player wants to interact with
		auto customCObjects = currTbl->get_as<toml::array>("customClObjects");
		if (customCObjects)
		{
            ReadCustomClObjects(customCObjects, currOverride.CustomClObjects);
		}

		ReadWidgetBasePos(*currTbl, currOverride.WidgetBasePos);

        if (ReadVector3(*currTbl, "widgetPosOffset", currOverride.WidgetPosOffset))
            currOverride.HasWidgetPosOffset = true;

        if (ReadVector3(*currTbl, "widgetPosOffsetRel", currOverride.WidgetPosOffsetRel))
            currOverride.HasWidgetPosOffsetRel = true;

		std::string priorityModifierName;
		if (ReadString(*currTbl, "priorityModifier", priorityModifierName, true))
		{
			currOverride.PriorityModifier = ObjectOverride::MakePriorityModifier(priorityModifierName);
            
			if (currOverride.PriorityModifier != ObjectOverride::EPriorityModifier::None &&
				ReadDouble(*currTbl, "priorityValue", currOverride.PriorityValue))
				currOverride.HasPriorityModifier = true;
		}

		if (currOverride.HasModelName)
            ObjectOverrides.insert({ currOverride.ModelName, currOverride });

		if (currOverride.HasDisplayName)
            ObjectOverrides.insert({ currOverride.DisplayName, currOverride });
    }

    return true;
}

bool Settings::ReadClObjects(const toml::array* tomlArray, std::vector<ClObjectOverride>& overrides)
{
    if (!tomlArray)
        return false;

    for (auto& currClObject : *tomlArray)
    {
        auto currObjectTbl = currClObject.as_table();
        if (!currObjectTbl)
            continue;

		ClObjectOverride newClOverride;

		ReadArray(*currObjectTbl, "path", newClOverride.Path, true);

		overrides.push_back(newClOverride);
	}

	return true;
}

bool Settings::ReadCustomClObjects(const toml::array* tomlArray, std::vector<CollisionFocusObject>& bounds)
{
    if (!tomlArray)
        return false;

    for (auto& currClObject : *tomlArray)
    {
        auto currObjectTbl = currClObject.as_table();
        if (!currObjectTbl)
            continue;

		ObjectBound currentBound;

		ReadVector3(*currObjectTbl, "boundMin", currentBound.boundMin);
		ReadVector3(*currObjectTbl, "boundMax", currentBound.boundMax);
		ReadVector3(*currObjectTbl, "rotation", currentBound.rotation);

		CollisionFocusObject newClObject;
        newClObject.BoundingBox = currentBound;

		bounds.push_back(newClObject);
	}

	return true;
}

bool Settings::ReadWidgetBasePos(const toml::table& tbl, ObjectOverride::WidgetPos& widgetPos)
{
    std::string basePosStr;
    if (!ReadString(tbl, "widgetBasePos", basePosStr))
		return false;

    Util::ToLowerString(basePosStr);

	if (basePosStr == "auto")
        widgetPos = ObjectOverride::WidgetPos::Auto;
    else if (basePosStr == "center")
        widgetPos = ObjectOverride::WidgetPos::Center;
    else if (basePosStr == "top")
        widgetPos = ObjectOverride::WidgetPos::Top;
    else if (basePosStr == "bottom")
        widgetPos = ObjectOverride::WidgetPos::Bottom;
    else if (basePosStr == "root")
        widgetPos = ObjectOverride::WidgetPos::Root;

	return true;
}

void Settings::InitFilterPresets()
{
    FilterManager::ClearFilters();

    ReadFilterPresetFile(FilterPresetsDefaultPath);

	auto filterPresetPaths = FindFilterPresetFiles();
	for (auto path : filterPresetPaths)
	{
		ReadFilterPresetFile(path);
	}
}

void Settings::InitFilterPresetStates()
{
	ReadFilterStatesFile(FilterStatesPath);
}

void Settings::ReadFilterStatesFile(std::string path)
{
	try
	{
		std::ifstream file(path);
		if (file)
		{
			std::stringstream ss;
			ss << file.rdbuf();
			file.close();

			auto jsonReader = Json::Reader();
			auto jsonRoot = Json::Value();

			if (!jsonReader.parse(ss, jsonRoot, false))
			{
				logger::warn("BTPS: failed to read ReadFilterStatesFile (this is normal until the MCM was opened and closed once)");
				return;
			}

			ReadFilterStates(jsonRoot);

			logger::info("BTPS: finished reading FilterStates.json");
		}
		else
			logger::warn("BTPS: failed to load FilterStates.json - can't open file '{}'", path);
	}
	catch (const Json::Exception& e)
	{
        std::ostringstream ss;
		ss << '\t' << e.what();

		std::string errorString = ss.str();
        logger::error("BTPS: error parsing FilterStates.json:\n{}", errorString);
	}
}

void Settings::ReadFilterStates(Json::Value jsonRoot)
{
	auto memberNames = jsonRoot.getMemberNames();

	for (auto memberName : memberNames)
	{
			auto defaultValue = Json::Value();

		auto currObj = jsonRoot.get(memberName, defaultValue);
		if (!currObj.isObject())
			continue;

		auto jIsEnabled = currObj.get("IsEnabled", defaultValue);
		if (!jIsEnabled.isInt())
			continue;

		bool isEnabled = (bool)jIsEnabled.asInt();

		FilterManager::SetFilterState(memberName, isEnabled);
	}
}

std::vector<std::string> Settings::FindFilterPresetFiles()
{
	std::vector<std::string> listOut;

	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(FilterPresetsContainingPath))
		{
			if (entry.is_directory() || entry.path() == FilterPresetsDefaultPath)
				continue;

			const std::filesystem::path& filePath(entry);

			if (filePath.extension() != ".toml")
				continue;

			listOut.push_back(entry.path().string());
		}
	}
	catch (std::filesystem::filesystem_error& e)
	{
		logger::error("BTPS: FindFilterPresetFiles: {} - {} - {} | {}", e.code().message(), e.what(), e.path1().string(), e.path2().string());
	}

	return listOut;
}

std::vector<std::string> Settings::FindObjectOverrideFiles()
{
	std::vector<std::string> listOut;

	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(ObjectOverridesContainingPath))
		{
			if (entry.is_directory() || entry.path() == ObjectOverridesDefaultPath)
				continue;

			const std::filesystem::path& filePath(entry);

			if (filePath.extension() != ".toml")
				continue;

			listOut.push_back(entry.path().string());
		}
	}
	catch (std::filesystem::filesystem_error& e)
	{
		logger::error("BTPS: FindObjectOverrideFiles: {} - {} - {} | {}", e.code().message(), e.what(), e.path1().string(), e.path2().string());
	}

	return listOut;
}

void Settings::ReadFilterPresetFile(std::string path)
{
    try
    {
        std::ifstream file(path);
        if (file)
        {
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();

            toml::table tbl = toml::parse(ss);
            ReadFilterPresetsTable(tbl);

            logger::info("BTPS: finished reading filter preset file '{}'", path);
        }
        else
            logger::error("BTPS: failed to load filter preset - can't open file '{}'", path);
    }
    catch (const toml::parse_error& e)
    {
        std::ostringstream ss;
        ss
            << '\t' << e.description() << '\n'
            << "\t\t(" << e.source().begin << ')';

        std::string errorString = ss.str();
        logger::error("BTPS: error parsing filter preset file:\n{} - {}", path, errorString);
    }
}

void Settings::ReadFilterPresetsTable(const toml::table& tbl)
{
    auto tblArray = tbl.get_as<toml::array>("filterPreset");
    if (!tblArray)
        return;

    for (auto& currObj : *tblArray)
    {
        auto currTbl = currObj.as_table();
        if (!currTbl)
            continue;

        ReadFilterPreset(*currTbl);
    }
}

void Settings::ReadFilterPreset(const toml::table& tbl)
{
    std::string presetName;
    if (!ReadString(tbl, "name", presetName))
    {
        logger::warn("BTPS: filterPreset has no name and will be ignored");
        return;
    }

    std::string presetDescription;
    ReadString(tbl, "description", presetDescription);

    auto arrayConditions = tbl.get_as<toml::array>("conditions");
	if (!arrayConditions)
	{
        logger::warn("BTPS: filterPreset '{}' has no conditions and will be ignored", presetName);
        return;
	}

    auto arrayFormTypes = tbl.get_as<toml::array>("formTypes");

	std::shared_ptr<Filter> newFilter = std::make_shared<Filter>();
    newFilter->FilterName = presetName;
    newFilter->Description = presetDescription;

	newFilter->Conditions = ReadConditions(*arrayConditions, newFilter);

	if (arrayFormTypes)
        newFilter->FormTypes = ReadFormTypes(*arrayFormTypes, newFilter);

	FilterManager::AddFilter(newFilter);
}

std::vector<ConnectedCondition> Settings::ReadConditions(const toml::array& conditions, std::shared_ptr<Filter> filter)
{
    std::vector<ConnectedCondition> connectedConditions;

	bool lastConditionInverse = false;
    ECondition lastCondition = ECondition::NONE;
    std::string lastFuncName;
	std::vector<std::string> lastParams;

	for (int i = 0; i < conditions.size(); i++)
	{
        auto currObj = conditions.get(i);
        if (!currObj || !currObj->is_table())
            continue;

        auto currTbl = currObj->as_table();

		// condition
		if (i % 2 == 0)
		{
            std::string funcName;
            if (!ReadString(*currTbl, "function", funcName))
            {
                logger::warn("BTPS: failed to parse filterPreset '{}', invalid condition obj; missing 'function' property", filter->FilterName);
                continue;
            }

            lastCondition = Filter::MakeCondition(funcName, lastConditionInverse);
            lastFuncName = funcName;
			lastParams.clear();
			ReadArray(*currTbl, "parameters", lastParams);

			if (i == conditions.size() - 1)
			{
                ConnectedCondition newCondition(filter, lastCondition, EConditionConnector::NONE, lastConditionInverse, lastParams);
                connectedConditions.push_back(newCondition);
			}
		}
		// logic connector
		else
		{
            std::string connectorName;
            if (!ReadString(*currTbl, "connector", connectorName))
            {
                logger::warn("BTPS: failed to parse filterPreset '{}', invalid condition obj; missing 'connector' property", filter->FilterName);
                continue;
            }
            Util::ToLowerString(connectorName);

            auto currConnector = Filter::MakeConditionConnector(connectorName);

			if (currConnector == EConditionConnector::NONE)
            {
                logger::warn("BTPS: failed to parse filter preset '{}', invalid connector '{}'", filter->FilterName, connectorName);
                return connectedConditions;
            }

			if (lastCondition == ECondition::NONE)
            {
                logger::warn("BTPS: failed to parse filter preset '{}', invalid condition '{}'", filter->FilterName, lastFuncName);
				return connectedConditions;
            }

			if (i == conditions.size() - 1)
                currConnector = EConditionConnector::NONE;

            ConnectedCondition newCondition(filter, lastCondition, currConnector, lastConditionInverse, lastParams);
            connectedConditions.push_back(newCondition);
		}
	}

	return connectedConditions;
}

std::vector<FilterFormType> Settings::ReadFormTypes(const toml::array& formTypes, std::shared_ptr<Filter> filter)
{
    std::vector<FilterFormType> formTypesOuts;

    for (auto& currObj : formTypes)
    {
        auto currTbl = currObj.as_table();
        if (!currTbl)
            continue;

        std::string typeName;
        if (!ReadString(*currTbl, "typeName", typeName))
        {
            logger::warn("BTPS: filterPreset failed to parse formType, missing 'typeName'");
            continue;
        }
        Util::ToLowerString(typeName);

		bool removeFormType = true;
        ReadBool(*currTbl, "remove", removeFormType);

		auto newFormType = FilterFormType(typeName, removeFormType);
		if (newFormType.FormType == RE::FormType::None)
		{
            logger::warn("BTPS: filterPreset failed to parse formType, invalid typeName '{}'", typeName);
            continue;
		}

		formTypesOuts.push_back(newFormType);
    }

    return formTypesOuts;
}
