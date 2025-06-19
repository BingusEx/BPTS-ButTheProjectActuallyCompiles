#pragma once

#include "API/TrueDirectionalMovementAPI.h"
#include "API/TrueHUDAPI.h"

class Compatibility
{
public:
	static void CheckModulesLoaded(const SKSE::LoadInterface* skse)
	{
		QuickLootRE::CheckModuleLoaded(skse);
		QuickLootIE::CheckModuleLoaded(skse);
        TrueDirectionalMovement::CheckModuleLoaded(skse);
        IFPV::CheckModuleLoaded();
	}

	class TrueDirectionalMovement
	{
	public:
		static bool IsEnabled;
		static bool IsTrueHUDEnabled;

		static void CheckModuleLoaded(const SKSE::LoadInterface* skse);

		static bool RequestTDMInterface();
        static bool RequestTrueHUDInterface();

		static TDM_API::IVTDM2* TDMAPIInterface;
		static TRUEHUD_API::IVTrueHUD3* TrueHUDAPIInterface;
	};

	class IFPV
    {
    public:
        static bool IsEnabled;
		static RE::TESGlobal* IsFirstPersonGlobal;

		static void CheckModuleLoaded();
	};

	class SkyHUD
	{
	public:
		static bool IsEnabled;

		static bool CheckModuleLoaded();

		static glm::vec2 CalculateWidgetOffset(RE::GFxValue hudRoot, RE::GPtr<RE::IMenu> hudMenu, glm::vec2 widgetPos);

		static constexpr double ScreenRectOffsetX = 200.0;
		static constexpr double ScreenRectOffsetY = 70.0;
	};

	class MoreHUD
	{
	public:
		static bool IsInstalled;
		static bool IsEnabled;

		static bool CheckModuleLoaded();

		static std::string StripHTMLIconPlaceholders(std::string htmlIn);

		static std::regex LoadedIconNameExpr;
	};

	class QuickLootIE
	{
	public:
		static bool IsInstalled;
		static bool IsEnabled;

		static bool ValuesCached;
		static double WidgetPosOrigX;
		static double WidgetPosOrigY;
		static double WidgetScaleOrigX;
		static double WidgetScaleOrigY;
		static double WidgetWidthOrig;
		static double WidgetHeightOrig;

		static constexpr std::string_view MENU_NAME{ "LootMenu" };

		static void CheckModuleLoaded(const SKSE::LoadInterface* skse);
		static bool CheckAndLogPlugin(const SKSE::LoadInterface* skse, const char* pluginName);
	};

	// also QuickLootEE. Compatibility solution should be identical
	class QuickLootRE
	{
	public:
		static bool IsInstalled;

		static void CheckModuleLoaded(const SKSE::LoadInterface* skse);
		static bool CheckAndLogPlugin(const SKSE::LoadInterface* skse, const char* pluginName);
		
		static [[nodiscard]] RE::GFxValue GetHUDObject();
		static [[nodiscard]] RE::GPtr<RE::IMenu> GetMenu();

		static constexpr std::string_view MENU_NAME{ "LootMenu" };

		enum : std::size_t
		{
			kActivate,
			kName,
			kShowButton,
			kTextOnly,
			kFavorMode,
			kShowCrosshair,
			kWeight,
			kCost,
			kFieldValue,
			kFieldText
		};

		// Address Lib IDs:
		//
		// 24212 TESNPC::GetActivateText
		// 17301 TESObjectACTI::GetActivateText
		// 17486 TESObjectCONT::GetActivateText
		//
		// 241857 TESNPC
		// 234000 TESObjectACTI
		// 234148 TESObjectCONT

		// QuickLootRE overrides GetActivateText on containers to disable the activation widget. To restore
		// the widget, I undo the hooks here by setting the vtbl ptrs back to the original functions
		//
		// see QuickLootRE: HUDManager.h
		
		static void RevertHooks();

		static bool TESNPC_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst);
		static bool TESObjectACTI_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst);
		static bool TESObjectCONT_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst);

		[[nodiscard]] static bool CanOpen(RE::TESObjectREFR* a_ref, bool recursiveCall = false);
        [[nodiscard]] static bool IsEmpty(RE::TESObjectREFR* a_ref, bool recursive = false);
        [[nodiscard]] static bool CanDisplay(const RE::TESBoundObject& a_object);
	};
};
