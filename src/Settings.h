#pragma once

#include "Filters/Filter.h"

#include "json/json.h"

#include "lib/ObjectOverride.h"

class Settings
{
private:
	static bool Widget3DEnabled;

public:

	static bool GetWidget3DEnabled();
	static void SetWidget3DEanbled(bool enabled);

public:

	// -- DEFAULTS -- //

	static bool EnableFiltersForNativeSelection;

	static bool IsEnabledInFirstPerson;
	static bool IsEnabledInThirdPerson;
	static bool IsEnabledOnHorseBack;
	static bool IsEnabledInCombat;
	static bool IsEnabledWithWeaponsDrawn;

	static bool OnlyAllowStealingWhileSneaking;
	static bool NoFurnitureWhileSneaking;

	static bool IsWidget3DEnabledInFirstPerson;
	static bool IsWidget3DEnabledInThirdPerson;
	static bool IsWidget3DEnabledOnHorseBack;
	static bool IsWidget3DEnabledInCombat;
	static bool IsWidget3DEnabledWithWeaponsDrawn;

	static bool IsDividerEnabled;
	static bool IsActivationButtonEnabled;
	static bool IsItemInfoEnabled;
	static bool IsDismountProgressCirlcleEnabled;
	static double WidgetAlpha;

	static bool EnableObjectCycle;
	static bool EnableContinuousObjectCycle;

	static int ObjectCycleModifierKey;
	static int ObjectCycleUpKey;
	static int ObjectCycleDownKey;

	static int CycleDelayMS;

	static double WidgetFadeInDelta;
	static double WidgetFadeOutDelta;

	static double WidgetZOffset;
	static double WidgetZOffsetFirstPerson;
	static double WidgetZOffsetAdditionalNPC;

	static double WidgetSizeMin;
	static double WidgetSizeMax;

	static double WidgetSizeMin_HorseBack;
	static double WidgetSizeMax_HorseBack;

	static float DebugDrawThickness;
	static float DebugDrawAlpha;

	static bool Draw_RayTraces;
	static bool Draw_Bounds_Box;
	static bool Draw_Bounds_Sphere;
	static bool Draw_Bounds_MinMax;

	static bool HotReloadConfig;
	static bool ShowFurnitureMarkers;

	static bool IsHoldToDismountEnabled;
	static double HoldToDismountTime;
    static double HoldToDismountMinTime;

	static bool AdjustMoreHudWidgets;
	static bool AdjustQuickLootWidgets;

	// -- CURRENT -- //

 	static bool Last_ControlsEnabled;
 	static bool Last_FreeCamEnabled;
 	static bool Last_FirstPerson;

	static std::unordered_map<std::string, bool> ObjectNameIgnoreList;
    static std::unordered_map<std::string, ObjectOverride> ObjectOverrides;

	// -- -- //

	static const std::string ConfigPath;
	static const std::string ConfigDefaultsPath;

	static const std::string ObjectOverridesContainingPath;
	static const std::string ObjectOverridesDefaultPath;

	static const std::string FilterPresetsContainingPath;
    static const std::string FilterPresetsDefaultPath;

    static const std::string FilterStatesPath;

	static bool IsNameInIgnoreList(std::string nameIn);
    static bool GetObjectOverride(RE::TESObjectREFR* objectRef, ObjectOverride& objOverrideOut);
    static bool GetObjectOverrideByName(std::string modelName, std::string displayName, ObjectOverride& objOverrideOut);
    static bool GetObjectOverrideByModelName(std::string modelName, ObjectOverride& objOverrideOut);
    static bool GetObjectOverrideByDisplayName(std::string displayName, ObjectOverride& objOverrideOut);
    static bool HasObjectOverride(std::string modelName, std::string displayName);

	static double GetCurrentWidgetZOffset();

	static double GetWidgetSizeMin();
	static double GetWidgetSizeMax();

	static void UpdateSettings();
	static bool IsInFreeCamera();

	static void InitObjectOverrides();
	static void ReadObjectOverrideFile(std::string path);
	static void ReadObjectOverrideTable(const toml::table& tbl);

	static void ReadSettings();
	static void ReadSettings(const std::string path);
	static void ReadTable(const toml::table& tbl);

	static bool ReadBool(const toml::table& tbl, std::string key, bool& valueOut);
	static bool ReadInt(const toml::table& tbl, std::string key, int& valueOut);
    static bool ReadString(const toml::table& tbl, std::string key, std::string& valueOut, bool toLower = false);
	static bool ReadDouble(const toml::table& tbl, std::string key, double& valueOut);
	static bool ReadFloat(const toml::table& tbl, std::string key, float& valueOut);
    static bool ReadArray(const toml::table& tbl, std::string key, std::vector<std::string>& valueOut, bool toLower = false);
	static bool ReadVector3(const toml::table& tbl, std::string key, glm::vec3& valueOut);

	static bool ReadIgnoreList(const toml::table& tbl);

	static bool ReadObjectOverrides(const toml::table& tbl);
	static bool ReadClObjects(const toml::array* tomlArray, std::vector<ClObjectOverride>& overrides);
	static bool ReadCustomClObjects(const toml::array* tomlArray, std::vector<CollisionFocusObject>& bounds);
    static bool ReadWidgetBasePos(const toml::table& tbl, ObjectOverride::WidgetPos& widgetPos);

	static void InitFilterPresets();
	static void InitFilterPresetStates();
    static void ReadFilterStatesFile(std::string path);
    static void ReadFilterStates(Json::Value jsonRoot);

	static std::vector<std::string> FindFilterPresetFiles();
	static std::vector<std::string> FindObjectOverrideFiles();

    static void ReadFilterPresetFile(std::string path);
    static void ReadFilterPresetsTable(const toml::table& tbl);
    static void ReadFilterPreset(const toml::table& tbl);
    static std::vector<ConnectedCondition> ReadConditions(const toml::array& conditions, std::shared_ptr<Filter> filter);
    static std::vector<FilterFormType> ReadFormTypes(const toml::array& formTypes, std::shared_ptr<Filter> filter);
};
