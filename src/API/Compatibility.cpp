#pragma once
#include "Compatibility.h"
#include "Settings.h"
#include "UI/SelectionWidget.h"

bool Compatibility::MoreHUD::IsInstalled = false;
bool Compatibility::MoreHUD::IsEnabled = false;

bool Compatibility::QuickLootRE::IsInstalled = false;

bool Compatibility::QuickLootIE::IsInstalled = false;
bool Compatibility::QuickLootIE::IsEnabled = false;
bool Compatibility::QuickLootIE::ValuesCached = false;
double Compatibility::QuickLootIE::WidgetPosOrigX = 0.0;
double Compatibility::QuickLootIE::WidgetPosOrigY = 0.0;
double Compatibility::QuickLootIE::WidgetScaleOrigX = 0.0;
double Compatibility::QuickLootIE::WidgetScaleOrigY = 0.0;
double Compatibility::QuickLootIE::WidgetWidthOrig = 0.0;
double Compatibility::QuickLootIE::WidgetHeightOrig = 0.0;

bool Compatibility::MoreHUD::CheckModuleLoaded()
{
	auto ui = RE::UI::GetSingleton();
	if (!ui)
	{
		logger::warn("failed to get UI - couldn't check if MoreHUD is loaded");
		return false;
	}

	auto hud = ui->GetMenu(RE::HUDMenu::MENU_NAME);
	if (!hud || !hud->uiMovie)
	{
		logger::warn("failed to get HUD menu - couldn't check if MoreHUD is loaded");
		return false;
	}

	RE::GFxValue moreHudContainer;
	hud->uiMovie->GetVariable(&moreHudContainer, "AHZWidgetContainer");

	IsInstalled = !moreHudContainer.IsNull() && moreHudContainer.IsObject();

	if (IsInstalled)
		logger::info("BTPS: compat - found MoreHUD");

	IsEnabled = IsInstalled && Settings::AdjustMoreHudWidgets;

	return true;
}

std::regex Compatibility::MoreHUD::LoadedIconNameExpr = std::regex("\\[(.*?)\\]", std::regex_constants::icase);

std::string Compatibility::MoreHUD::StripHTMLIconPlaceholders(std::string htmlIn)
{
    htmlIn = std::regex_replace(htmlIn, LoadedIconNameExpr, "");

	return htmlIn;
}

void Compatibility::QuickLootIE::CheckModuleLoaded(const SKSE::LoadInterface* skse)
{
	if (CheckAndLogPlugin(skse, "QuickLootIE"))
		IsInstalled = true;
}

bool Compatibility::QuickLootIE::CheckAndLogPlugin(const SKSE::LoadInterface* skse, const char* pluginName)
{
	auto pluginInfo = skse->GetPluginInfo(pluginName);
    if (pluginInfo) {
        logger::info("BTPS: compat - found {}", pluginName);
        return true;
    }
    return false;
}

void Compatibility::QuickLootRE::CheckModuleLoaded(const SKSE::LoadInterface* skse)
{
    if (CheckAndLogPlugin(skse, "QuickLootRE") ||
        CheckAndLogPlugin(skse, "QuickLootEE") ||
        CheckAndLogPlugin(skse, "QuickLootEE-NG"))
        IsInstalled = true;
}

bool Compatibility::QuickLootRE::CheckAndLogPlugin(const SKSE::LoadInterface* skse, const char* pluginName)
{
    auto pluginInfo = skse->GetPluginInfo(pluginName);
    if (pluginInfo) {
        logger::info("BTPS: compat - found {}", pluginName);
        return true;
    }
    return false;
}

[[nodiscard]] RE::GFxValue Compatibility::QuickLootRE::GetHUDObject()
{
	RE::GFxValue object;

	auto ui = RE::UI::GetSingleton();
	auto hud = ui ? ui->GetMenu<RE::HUDMenu>() : nullptr;
	auto view = hud ? hud->uiMovie : nullptr;
	if (view) {
		view->GetVariable(std::addressof(object), "_root.HUDMovieBaseInstance");
	}

	return object;
}

RE::GPtr<RE::IMenu> Compatibility::QuickLootRE::GetMenu()
{
	auto ui = RE::UI::GetSingleton();
	return ui ? ui->GetMenu(MENU_NAME) : nullptr;
}

void Compatibility::QuickLootRE::RevertHooks()
{
	logger::info("BTPS: overriding QuickLootRE/EE/NG hooks");

	// TODO These are defined and much broader in clib-ng.
	// SE: 159fcd0
	// AE: ???
	REL::Relocation<std::uintptr_t> TESNPC_vtbl{ RELOCATION_ID(241857, 195816) };
	// SE: 15576f0
	// AE: ???
	REL::Relocation<std::uintptr_t> TESObjectACTI_vtbl{ RELOCATION_ID(234000, 189485) };
	// SE: 1559930
	// AE: ???
	REL::Relocation<std::uintptr_t> TESObjectCONT_vtbl{ RELOCATION_ID(234148, 189633) };

	TESNPC_vtbl.write_vfunc(0x4C, TESNPC_Original_GetActivateText);
	TESObjectACTI_vtbl.write_vfunc(0x4C, TESObjectACTI_Original_GetActivateText);
	TESObjectCONT_vtbl.write_vfunc(0x4C, TESObjectCONT_Original_GetActivateText);
}

bool Compatibility::QuickLootRE::TESNPC_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst)
{
	if (!Settings::GetWidget3DEnabled() && CanOpen(a_activator))
		return false;

	using func_t = decltype(&RE::TESBoundObject::GetActivateText);

	REL::Relocation<func_t> func{ RELOCATION_ID(24212, 24716) };

	return func(a_this, a_activator, a_dst);
}

bool Compatibility::QuickLootRE::TESObjectACTI_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst)
{
	if (!Settings::GetWidget3DEnabled() && CanOpen(a_activator))
		return false;

	using func_t = decltype(&RE::TESBoundObject::GetActivateText);

	REL::Relocation<func_t> func{ RELOCATION_ID(17301, 17703) };

	return func(a_this, a_activator, a_dst);
}

bool Compatibility::QuickLootRE::TESObjectCONT_Original_GetActivateText(RE::TESBoundObject* a_this, RE::TESObjectREFR* a_activator, RE::BSString& a_dst)
{
	if (!Settings::GetWidget3DEnabled() && CanOpen(a_activator))
		return false;

	using func_t = decltype(&RE::TESBoundObject::GetActivateText);

	REL::Relocation<func_t> func{ RELOCATION_ID(17486, 17888) };

	return func(a_this, a_activator, a_dst);
}

// from QuickLootRE
[[nodiscard]] bool Compatibility::QuickLootRE::CanOpen(RE::TESObjectREFR* a_ref, bool recursiveCall)
{
    if (!a_ref)
        return false;

	auto obj = a_ref->GetObjectReference();
	if (!obj)
		return false;

	// recursion check to safeguard against a potential, very rare freeze. Theoretical fix
	if (obj->Is(RE::FormType::Activator) && !recursiveCall)
	{
		return CanOpen(a_ref->extraList.GetAshPileRef().get().get(), true);
	}

	if (auto actor = a_ref->As<RE::Actor>(); actor)
	{
		if (!actor->IsDead() ||
			actor->IsSummoned()) {
			return false;
		}
	}

    return a_ref->GetContainer() && !a_ref->IsLocked() && !IsEmpty(a_ref);
}

bool Compatibility::QuickLootRE::IsEmpty(RE::TESObjectREFR* a_ref, const bool recursive)
{
	auto ashPileRef = a_ref->extraList.GetAshPileRef().get().get();
	if (!recursive && ashPileRef)
	{
		return IsEmpty(ashPileRef, true);
	}

    auto inv = a_ref->GetInventory(CanDisplay);
    for (auto& [obj, data] : inv)
    {
        auto& [count, entry] = data;
        if (count > 0 && entry)
			return false;
    }

    auto dropped = a_ref->GetDroppedInventory(CanDisplay);
    for (auto& [obj, data] : dropped)
    {
        auto& [count, items] = data;
        if (count > 0 && !items.empty())
            return false;
    }

    return true;
}

[[nodiscard]] bool Compatibility::QuickLootRE::CanDisplay(const RE::TESBoundObject& a_object)
{
    switch (a_object.GetFormType())
    {
    case RE::FormType::Scroll:
    case RE::FormType::Armor:
    case RE::FormType::Book:
    case RE::FormType::Ingredient:
    case RE::FormType::Misc:
    case RE::FormType::Weapon:
    case RE::FormType::Ammo:
    case RE::FormType::KeyMaster:
    case RE::FormType::AlchemyItem:
    case RE::FormType::Note:
    case RE::FormType::SoulGem:
        break;
    case RE::FormType::Light:
        {
            auto& light = static_cast<const RE::TESObjectLIGH&>(a_object);
            if (!light.CanBeCarried())
            {
                return false;
            }
        }
        break;
    default:
        return false;
    }

    if (!a_object.GetPlayable())
    {
        return false;
    }

    auto name = a_object.GetName();
    if (!name || name[0] == '\0')
    {
        return false;
    }

    return true;
}

bool Compatibility::SkyHUD::IsEnabled = false;

bool Compatibility::SkyHUD::CheckModuleLoaded()
{
	auto ui = RE::UI::GetSingleton();
	if (!ui)
	{
		//logger::warn("failed to get UI - couldn't check if SkyHUD is loaded");
		return false;
	}

	auto hud = ui->GetMenu(RE::HUDMenu::MENU_NAME);
	if (!hud || !hud->uiMovie)
	{
		//logger::warn("failed to get HUD menu - couldn't check if SkyHUD is loaded");
		return false;
	}

	RE::GFxValue rootElement;
	hud->uiMovie->GetVariable(&rootElement, "HUDMovieBaseInstance");
	if (!rootElement.IsObject())
		return false;

	auto SkyHUDWrapper = SelectionWidget::GetGFxMember(rootElement, "RolloverName_mc");
	IsEnabled = !SkyHUDWrapper.IsNull() && SkyHUDWrapper.IsObject();

	return true;
}

glm::vec2 Compatibility::SkyHUD::CalculateWidgetOffset(RE::GFxValue hudRoot, RE::GPtr<RE::IMenu> hudMenu, glm::vec2 widgetPos)
{
	if (!hudRoot.IsObject() || !hudMenu || !hudMenu->uiMovie)
        return widgetPos;

	return SelectionWidget::ToLocal(glm::vec2(widgetPos.x, widgetPos.y), hudRoot, hudMenu);
}

bool Compatibility::TrueDirectionalMovement::IsEnabled;
bool Compatibility::TrueDirectionalMovement::IsTrueHUDEnabled;

TDM_API::IVTDM2* Compatibility::TrueDirectionalMovement::TDMAPIInterface;
TRUEHUD_API::IVTrueHUD3* Compatibility::TrueDirectionalMovement::TrueHUDAPIInterface;

void Compatibility::TrueDirectionalMovement::CheckModuleLoaded(const SKSE::LoadInterface* skse)
{
	auto tdmPlugin = skse->GetPluginInfo("TrueDirectionalMovement");
    if (tdmPlugin)
    {
        IsEnabled = true;
        logger::info("BTPS: compat - found TrueDirectionalMovement");

		//RequestTDMInterface();
    }

	auto trueHUDPlugin = skse->GetPluginInfo("TrueHUD");
    if (trueHUDPlugin)
    {
        IsTrueHUDEnabled = true;
        logger::info("BTPS: compat - found TrueHUD");

		RequestTrueHUDInterface();
    }
}

bool Compatibility::TrueDirectionalMovement::RequestTDMInterface()
{
    if (!IsEnabled)
        return false;

    TDMAPIInterface = reinterpret_cast<TDM_API::IVTDM2*>(TDM_API::RequestPluginAPI(TDM_API::InterfaceVersion::V2));
	if (TDMAPIInterface)
	{
        logger::info("BTPS: TDM interface request successful - V2");
		return false;
	}

	logger::info("BTPS: TDM interface request failed - V3");
    return false;
}

bool Compatibility::TrueDirectionalMovement::RequestTrueHUDInterface()
{
    if (!IsTrueHUDEnabled)
        return false;

    TrueHUDAPIInterface = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
    if (TrueHUDAPIInterface)
	{
        logger::info("BTPS: TrueHUD interface request successful - V3");
        return true;
	}
    
	logger::info("BTPS: TrueHUD interface request failed - V3");
    return false;
}

bool Compatibility::IFPV::IsEnabled = false;
RE::TESGlobal* Compatibility::IFPV::IsFirstPersonGlobal;

void Compatibility::IFPV::CheckModuleLoaded()
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (!dataHandler)
    {
        logger::info("BTPS: compat - failed to check for ImmersiveFirstPersonView: no dataHandler");
        return;
    }

	// from TDM
    IsFirstPersonGlobal = dataHandler->LookupForm<RE::TESGlobal>(0x801, "IFPVDetector.esl");
    if (!IsFirstPersonGlobal)
        return;

    IsEnabled = true;
    logger::info("BTPS: compat - found ImmersiveFirstPersonView");
}
