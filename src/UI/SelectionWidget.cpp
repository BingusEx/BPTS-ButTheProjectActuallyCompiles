#pragma once
#include "SelectionWidget.h"
#include "RevE/Offsets.h"
#include "FocusManager.h"
#include "API/Compatibility.h"
#include "Settings.h"

#include "windows.h"

#include "lib/Util.h"

std::optional<glm::dvec3> SelectionWidget::LastWidgetPos;

glm::vec2 SelectionWidget::WidgetPos;
glm::vec2 SelectionWidget::WidgetSize;

//

bool SelectionWidget::UIValuesCached = false;
bool SelectionWidget::ModulesLoadedChecked = false;
bool SelectionWidget::OrigUIValuesSet = false;

double SelectionWidget::FadeProgress = 0.0;
EUIFadeMode SelectionWidget::FadeMode = NONE;

std::string SelectionWidget::LastActivateText;
std::string SelectionWidget::LastActivateName;

bool SelectionWidget::SelectionIs2DOverride = false;
bool SelectionWidget::UpdateFocusObjDeferred = false;

std::shared_ptr<FocusObject> SelectionWidget::UIFocusRef = nullptr;

RE::UI* SelectionWidget::CachedUI;
RE::GPtr<RE::IMenu> SelectionWidget::CachedHUD;
RE::GPtr<RE::IMenu> SelectionWidget::CachedWidgetMenu;

RE::GFxValue SelectionWidget::CachedSelectionWidget;
RE::GFxValue SelectionWidget::CachedActivateText;
RE::GFxValue SelectionWidget::CachedGrayBar;
RE::GFxValue SelectionWidget::CachedRolloverText;
RE::GFxValue SelectionWidget::CachedRolloverInfoText;
RE::GFxValue SelectionWidget::CachedRolloverButton_tf;
RE::GFxValue SelectionWidget::CachedRolloverGrayBar_mc;
RE::GFxValue SelectionWidget::CachedActivateButton;
RE::GFxValue SelectionWidget::CachedInfoText;

RE::GFxValue SelectionWidget::CachedWidgetCont;
RE::GFxValue SelectionWidget::CachedAhzWidget;
RE::GFxValue SelectionWidget::CachedMoreHudContent;
RE::GFxValue SelectionWidget::CachedMoreHudIconCont;
RE::GFxValue SelectionWidget::CachedMoreHudTextField;
RE::GFxValue SelectionWidget::CachedMoreHUDLoadedIcons;

glm::vec2 SelectionWidget::OrigMCPos;
glm::vec2 SelectionWidget::OrigMCSize;

double SelectionWidget::Widget2DSize = 1.0;
glm::vec2 SelectionWidget::Widget2DPos = glm::vec2(0.5, 0.35);

float SelectionWidget::ProgressCircleValue = 0.0f;

std::vector<std::string> SelectionWidgetMenu::Hidden_Sources;
bool SelectionWidgetMenu::IsEnabled = false;
bool SelectionWidgetMenu::IsVisible = false;

std::mutex m_setEnabled;
std::recursive_mutex m_update;
std::mutex m_hideHUD;
std::mutex m_selectionChanged;

std::recursive_mutex m_visibility;

std::mutex m_load;
std::mutex m_unload;
std::mutex m_updateText;
std::recursive_mutex m_manipulateMoreHUD;
std::mutex m_setFocusRef;
std::mutex m_clearSelectionText;

std::mutex m_getWidgetPos;
std::mutex m_setWidgetPos;
std::mutex m_getWidgetSize;
std::mutex m_setWidgetSize;

void SelectionWidget::OnChangeActivateText()
{
	auto currentFocusObj = FocusManager::GetActiveFocusObject();
	if (!currentFocusObj || !currentFocusObj->ObjectRef)
	{
		OnUpdateCrosshairText2D();
	}
	else
	{
		SelectionIs2DOverride = false;
	}
}

void SelectionWidget::OnSelectionChanged()
{
	std::lock_guard lock(m_selectionChanged);

	if (Settings::GetWidget3DEnabled())
	{
		auto focusObj = FocusManager::GetActiveFocusObject();
		if (focusObj && focusObj->ObjectRef)
		{
			if (Settings::WidgetFadeInDelta > 0.0 && Settings::WidgetFadeInDelta < 1.0)
				StartFadeIn();
			else
				FadeInInstant();
		}
		else
		{
			if (Settings::WidgetFadeOutDelta > 0.0 && Settings::WidgetFadeOutDelta < 1.0)
				StartFadeOut();
			else
				FadeOutInstant();
		}

		//if (Compatibility::MoreHUD::IsInstalled)
			//SetMoreHUDWidgetVisibility(false);
	}

	//Update();
}

void SelectionWidget::TryUpdateSelectionBounds()
{
	auto newFocusObj = FocusManager::GetActiveFocusObject();
	auto oldFocusObj = GetUIFocusRef();

	// in case the focused ref is still the same, update the FocusObject 
	// (which contains the reference and object bounds, so basically: update UIFocusRef with newly calculated bounds and position)
	if (newFocusObj && oldFocusObj && newFocusObj->ObjectRef == oldFocusObj->ObjectRef)
	{
		SetUIFocusRef(newFocusObj);
	}
}

void SelectionWidget::SetUIFocusRef(const std::shared_ptr<FocusObject>& focusRef)
{
	std::lock_guard lock(m_setFocusRef);
	UIFocusRef = focusRef;
}

std::shared_ptr<FocusObject> SelectionWidget::GetUIFocusRef()
{
	std::lock_guard lock(m_setFocusRef);
	return UIFocusRef;
}

void SelectionWidgetMenu::SetEnabled(bool mode)
{
	std::lock_guard lock(m_setEnabled);

	if (IsEnabled != mode)
	{
		IsEnabled = mode;
		ToggleVisibility(true);
	}
}

glm::vec2 SelectionWidget::GetWidgetPos()
{
	std::lock_guard lock(m_getWidgetPos);
	return WidgetPos;
}

void SelectionWidget::SetWidgetPos(glm::vec2 pos)
{
	std::lock_guard lock(m_setWidgetPos);
	WidgetPos = pos;
}

glm::vec2 SelectionWidget::GetWidgetSize()
{
	std::lock_guard lock(m_getWidgetSize);
	return WidgetSize;
}

void SelectionWidget::SetWidgetSize(glm::vec2 size)
{
	std::lock_guard lock(m_setWidgetSize);
	WidgetSize = size;
}

void SelectionWidget::OnPerspectiveToggled(bool cond)
{
	LastWidgetPos.reset();
	UpdateFocusObjDeferred = false;
	
	FadeInInstant();
}

void SelectionWidget::OnNativeSelectionChanged(RE::TESObjectREFR* nativeSelection)
{
}

void SelectionWidget::OnUpdateCrosshairText()
{
	// for an explanation of this spaghetti solution of horrors, see declaration of UpdateFocusObjDeferred
	if (UpdateFocusObjDeferred)
	{
		SKSE::GetTaskInterface()->AddUITask([]()
		{
			SetUIFocusRef(FocusManager::GetActiveFocusObject());
		});
		UpdateFocusObjDeferred = false;
	}
}

void SelectionWidget::OnUpdateCrosshairText2D()
{
	// things like AutoLoadDoors: they change the crosshair text, but don't set the crosshair ref
	// make sure the widget is faded in for those. This has to be here instead of at 3D check, because
	// after fading out, 3D check fails for a little moment, but widget should be faded out
	FadeInInstant();
	SelectionIs2DOverride = true;
}

// cached Scaleform GFxValues to avoid constant string lookups
bool SelectionWidget::CacheGFxObjects()
{
	CachedUI = RE::UI::GetSingleton();
	if (!CachedUI) return false;

	CachedHUD = CachedUI->GetMenu(RE::HUDMenu::MENU_NAME);
	if (!CachedHUD || !CachedHUD->uiMovie) return false;

	CachedWidgetMenu = CachedUI->GetMenu(SelectionWidgetMenu::MENU_NAME);
	if (!CachedWidgetMenu || !CachedWidgetMenu->uiMovie) return false;

	CachedWidgetMenu->uiMovie->GetVariable(&CachedSelectionWidget, "mc_SelectionWidget");
	if (CachedSelectionWidget.IsNull() || !CachedSelectionWidget.IsObject()) return false;

    CachedHUD->uiMovie->GetVariable(&CachedRolloverText, "HUDMovieBaseInstance.RolloverText");
	if (CachedRolloverText.IsNull() || !CachedRolloverText.IsObject()) return false;

	CachedHUD->uiMovie->GetVariable(&CachedRolloverInfoText, "HUDMovieBaseInstance.RolloverInfoText");
	if (CachedRolloverInfoText.IsNull() || !CachedRolloverInfoText.IsObject()) return false;

	CachedHUD->uiMovie->GetVariable(&CachedRolloverButton_tf, "HUDMovieBaseInstance.RolloverButton_tf");
	if (CachedRolloverButton_tf.IsNull() || !CachedRolloverButton_tf.IsObject()) return false;

	CachedHUD->uiMovie->GetVariable(&CachedRolloverGrayBar_mc, "HUDMovieBaseInstance.RolloverGrayBar_mc");
	// ClearUI compatibility - probably messes with the actionscript somewhere and removes 'RolloverGrayBar_mc'
	if (CachedRolloverGrayBar_mc.IsNull() || !CachedRolloverGrayBar_mc.IsObject())
		CachedHUD->uiMovie->GetVariable(&CachedRolloverGrayBar_mc, "HUDMovieBaseInstance.RolloverInfo_mc.RolloverInfoInstance");

	if (CachedRolloverGrayBar_mc.IsNull() || !CachedRolloverGrayBar_mc.IsObject()) return false;

	CachedActivateButton = GetGFxMember(CachedSelectionWidget, "ActivateButtonInstance");
	if (CachedActivateButton.IsNull() || !CachedActivateButton.IsObject()) return false;

	CachedWidgetMenu->uiMovie->GetVariable(&CachedSelectionWidget, "mc_SelectionWidget");
	if (CachedSelectionWidget.IsNull() || !CachedSelectionWidget.IsObject()) return false;

	CachedActivateText = GetGFxMember(CachedSelectionWidget, "ActivateText");
	if (CachedActivateText.IsNull() || !CachedActivateText.IsObject()) return false;

	CachedGrayBar = GetGFxMember(CachedSelectionWidget, "GrayBar");
	if (CachedGrayBar.IsNull() || !CachedGrayBar.IsObject()) return false;

	CachedInfoText = GetGFxMember(CachedSelectionWidget, "InfoText");
	if (CachedInfoText.IsNull() || !CachedInfoText.IsObject()) return false;

	if (Compatibility::MoreHUD::IsInstalled)
	{
		CachedHUD->uiMovie->GetVariable(&CachedWidgetCont, "AHZWidgetContainer");
		if (CachedWidgetCont.IsNull() || !CachedWidgetCont.IsObject()) return false;

		CachedAhzWidget = GetGFxMember(CachedWidgetCont, "AHZWidget");
		if (CachedAhzWidget.IsNull() || !CachedAhzWidget.IsObject()) return false;

		CachedMoreHudContent = GetGFxMember(CachedAhzWidget, "content");
		if (CachedMoreHudContent.IsNull() || !CachedMoreHudContent.IsObject()) return false;

		CachedHUD->uiMovie->GetVariable(&CachedMoreHudIconCont, "ahz.scripts.widgets.AHZHudInfoWidget.IconContainer");
		if (CachedMoreHudIconCont.IsNull() || !CachedMoreHudIconCont.IsObject()) return false;

		CachedMoreHudTextField = GetGFxMember(CachedMoreHudIconCont, "_tf");
		if (CachedMoreHudIconCont.IsNull() || !CachedMoreHudIconCont.IsObject()) return false;

		CachedMoreHUDLoadedIcons = GetGFxMember(CachedMoreHudIconCont, "loadedIcons");
		if (CachedMoreHUDLoadedIcons.IsNull() || !CachedMoreHUDLoadedIcons.IsObject()) return false;
	}

	UIValuesCached = true;

	return true;
}

double SelectionWidget::GetMaxCameraDist()
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();

	if (!playerChar || !playerChar->IsOnMount())
		return MAX_CAMERA_DIST;
	return MAX_CAMERA_DIST_HORSEBACK;
}

void SelectionWidget::Update()
{
	std::lock_guard lock(m_update);

    if (!UIValuesCached)
		return;

    /* hide Skyrim's selection UI */
	SelectionWidget::HideNativeHUD();

	TryUpdateSelectionBounds();

	ProgressFade();
	UpdateWidgetText();
	UpdateTDMVisibility();
	UpdateWidgetPos();

	if (Compatibility::MoreHUD::IsEnabled)
		UpdateMoreHUDWidget();

	if (Compatibility::QuickLootIE::IsEnabled)
		UpdateQuickLootWidget();
}

void SelectionWidget::UpdateWidgetPos()
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();
	if (!playerChar)
		return;

	UpdateProgressCircle(CachedWidgetMenu);

	auto focusObj = GetUIFocusRef();

	glm::dvec3 widgetPos;

	if (!Settings::GetWidget3DEnabled() ||
		!Get3DWidgetPos(focusObj.get(), widgetPos)) // Get3DWidgetPos fails on things like AutoLoadDoors for example - draw those 2D
	{
		SetWidgetPos(Get2DWidgetPos(CachedWidgetMenu));
		SetWidgetSize(Get2DWidgetSize());

		SetElementPos(GetWidgetPos(), CachedSelectionWidget);
		SetElementSize(GetWidgetSize(), CachedSelectionWidget);

		if (FadeMode != FADED)
			SetSelectionAlpha(Settings::WidgetAlpha, CachedSelectionWidget);
	}
	else
	{
		SetWidgetPos(GetScreenLoc(CachedWidgetMenu, widgetPos));
		SetWidgetPos(ClampWidgetToScreenRect(CachedWidgetMenu, GetWidgetPos(), CachedSelectionWidget));

		SetWidgetSize(Get3DWidgetSize(focusObj.get()));

 		SetElementSize(GetWidgetSize(), CachedSelectionWidget);
 		SetElementPos(GetWidgetPos(), CachedSelectionWidget);

		LastWidgetPos = widgetPos;
	}
}

constexpr float ACTIVATE_BUTTON_OFFSET_LEFT = 10.0;

void SelectionWidget::UpdateWidgetText()
{
	std::lock_guard lock(m_updateText);

	auto nativeHtmlString = GetGFxMember(CachedRolloverText, "htmlText");
	auto stringName = GetGFxMember(CachedRolloverText, "text");
    auto htmlStringInfo = GetGFxMember(CachedRolloverInfoText, "htmlText");
	auto stringInfo = GetGFxMember(CachedRolloverInfoText, "text");
	auto htmlStringButton = GetGFxMember(CachedRolloverButton_tf, "htmlText");

	std::string sanitizedHTMLString = 
		Compatibility::MoreHUD::IsEnabled ? 
		Compatibility::MoreHUD::StripHTMLIconPlaceholders(nativeHtmlString.ToString().c_str()) : 
		nativeHtmlString.ToString().c_str();

	auto activateButtonText = GetGFxMember(CachedActivateButton, "Text");
	if (!activateButtonText.IsObject())
		return;

	auto focusObj = GetUIFocusRef();

	SetSelectionAlpha(GetWidgetCurrentAlpha(), CachedSelectionWidget);
	
	if (FadeMode != FADE_OUT)
	{
		std::string currActivateText = std::string(nativeHtmlString.ToString());
		std::string currActivateName = std::string(stringName.ToString());

		if (currActivateName != LastActivateName)
		{
			CachedActivateText.SetTextHTML(sanitizedHTMLString.c_str());
			CachedInfoText.SetTextHTML(htmlStringInfo.GetString());
			activateButtonText.SetTextHTML(htmlStringButton.GetString());

			LastActivateText = currActivateText;
			LastActivateName = currActivateName;
			OnChangeActivateText();

			bool grayBarVisible = Settings::IsDividerEnabled && !Util::IsEmpty(stringInfo.GetString());

			// show if infotext isn't empty
			CachedGrayBar.SetMember("_visible", grayBarVisible);
			CachedInfoText.SetMember("_visible", Settings::IsItemInfoEnabled);
		}
	}

	// disable activate button for containers if quickloot is installed
    bool quickLootActive = Compatibility::QuickLootRE::IsInstalled && focusObj && focusObj->ObjectRef.get() && Compatibility::QuickLootRE::CanOpen(focusObj->ObjectRef.get().get());

	if (!Settings::IsActivationButtonEnabled ||
		quickLootActive ||
		Util::IsFirstLineEmpty(stringName.GetString()) || 
		SelectionIs2DOverride)
	{
		CachedActivateButton.SetMember("_visible", false);
	}
	else
	{
		RE::GFxValue lineMetricsOut;
		RE::GFxValue args[1]{ 0 };

		try { CachedActivateText.Invoke("getLineMetrics", &lineMetricsOut, args, 1); }
		catch ([[maybe_unused]] std::exception& e)
		{
			logger::info("BTPS SelectionWidget::UpdateWidgetText(): failed on activateText invoke: 'getLineMetrics'");
			return;
		}

 		auto lineMetricsX = GetGFxMember(lineMetricsOut, "width").GetNumber();
		lineMetricsX = PixelToStageCoordinates(CachedWidgetMenu, glm::vec2(lineMetricsX, 0.0)).x;

		CachedActivateButton.SetMember("_x", -lineMetricsX - ACTIVATE_BUTTON_OFFSET_LEFT);
		CachedActivateButton.SetMember("_visible", true);
	}
}

void SelectionWidget::HideNativeHUD()
{
	std::lock_guard lock(m_hideHUD);

	auto ui = RE::UI::GetSingleton();
	if (!ui)
		return;

	auto hud = ui->GetMenu(RE::HUDMenu::MENU_NAME);
	if (!hud || !hud->uiMovie)
		return;

	CachedRolloverText.SetMember("_visible", false);
	CachedRolloverInfoText.SetMember("_visible", false);
	CachedRolloverGrayBar_mc.SetMember("_visible", false);
	CachedRolloverButton_tf.SetMember("_visible", false);
}

void SelectionWidget::UpdateProgressCircle(const RE::GPtr<RE::IMenu>& menu)
{
	if (!menu || !menu->uiMovie)
		return;

	RE::GFxValue progressCirlce;
	menu->uiMovie->GetVariable(&progressCirlce, "ProgressCircleInstance");

	if (!progressCirlce.IsObject())
		return;

	progressCirlce.SetMember("_alpha", Settings::WidgetAlpha);

	if (ProgressCircleValue == 0.0f)
		progressCirlce.SetMember("_visible", false);
	else
	{
		progressCirlce.SetMember("_visible", true);

		RE::GFxValue args[1]{ ProgressCircleValue };

		try
		{
			progressCirlce.Invoke("SetProgressCircleProgress", nullptr, args, 1);
		}
		catch ([[maybe_unused]] std::exception& e)
		{
			logger::info("BTPS SelectionWidget::UpdateProgressCircle: failed on progressCirlce invoke: 'SetProgressCircleProgress'");
		}
	}
}

void SelectionWidget::UpdateTDMVisibility()
{
	using tdmCompat = Compatibility::TrueDirectionalMovement;
    auto focusObj = GetUIFocusRef();

	if (tdmCompat::IsEnabled && tdmCompat::IsTrueHUDEnabled && Settings::GetWidget3DEnabled() &&
		focusObj && focusObj->ObjectRef.get())
	{
        auto actorRef = focusObj->ObjectRef.get()->As<RE::Actor>();

        if (tdmCompat::TrueHUDAPIInterface)
        {
            if (actorRef)
            {
                auto actorRefHandle = actorRef->GetHandle();
				if (tdmCompat::TrueHUDAPIInterface->HasInfoBar(actorRefHandle))
				{
                    SelectionWidgetMenu::Hide("TDM");
                    return;
				}
            }
        }
	}

	SelectionWidgetMenu::Show("TDM");
}

glm::vec2 SelectionWidget::ToLocal(glm::vec2 posIn, RE::GFxValue element, RE::GPtr<RE::IMenu> menu)
{
	if (!menu || !menu->uiMovie || !element.IsObject())
		return posIn;

	RE::GFxValue objPoint;
	menu->uiMovie->CreateObject(&objPoint);
	objPoint.SetMember("x", posIn.x);
	objPoint.SetMember("y", posIn.y);

	RE::GFxValue gfxResult;
	RE::GFxValue args[1]{ objPoint };
	try
	{
		menu->uiMovie->Invoke("globalToLocal", &gfxResult, args, 1);
	}
	catch ([[maybe_unused]] std::exception& e) 
	{
		logger::info("BTPS SelectionWidget::ToLocal: failed on element invoke: 'globalToLocal'");
	}

	double newX = GetGFxMember(args[0], "x").GetNumber();
	double newY = GetGFxMember(args[0], "y").GetNumber();

	return {newX, newY};
}

glm::vec2 SelectionWidget::ToGlobal(glm::vec2 posIn, RE::GFxValue element, RE::GPtr<RE::IMenu> menu)
{
	if (!menu || !menu->uiMovie || !element.IsObject())
		return posIn;

	RE::GFxValue objPoint;
	menu->uiMovie->CreateObject(&objPoint);
	objPoint.SetMember("x", posIn.x);
	objPoint.SetMember("y", posIn.y);

	RE::GFxValue gfxResult;
	RE::GFxValue args[1]{ objPoint };
	try
	{
		menu->uiMovie->Invoke("localToGlobal", &gfxResult, args, 1);
	}
	catch ([[maybe_unused]] std::exception& e) 
	{
		logger::info("BTPS SelectionWidget::ToGlobal: failed on element invoke: 'localToGlobal'");
	}

	double newX = GetGFxMember(args[0], "x").GetNumber();
	double newY = GetGFxMember(args[0], "y").GetNumber();

	return {newX, newY};
}

static constexpr double MoreHUDIconsOffsetRight = 60.0;
static constexpr double MoreHUDIconMargin = 10.0;

static constexpr double MoreHUDEffectsSizeFact = 1.25;
static constexpr double InfoWidgetOffsetLeft = 75.0;

void SelectionWidget::UpdateMoreHUDWidget()
{
	std::lock_guard lock(m_manipulateMoreHUD);

	double sizeFactor = WidgetSize.x / 100.0;

	auto textWidth = GetGFxMember(CachedActivateText, "textWidth").GetNumber();
	textWidth = (PixelToStageCoordinates(CachedHUD, glm::vec2(textWidth, 0.0)).x * sizeFactor);

	auto textHeight = GetGFxMember(CachedActivateText, "textHeight").GetNumber();
	textHeight = PixelToStageCoordinates(CachedHUD, glm::vec2(0.0, textHeight)).y * sizeFactor;

	auto SelectionWidgetHalfHeight = GetGFxMember(CachedSelectionWidget, "_height").GetNumber() / 2.0;

	auto grayBarHalfWidth = (GetGFxMember(CachedGrayBar, "_width").GetNumber() / 2.0) * sizeFactor;

	auto gfxIconSize = GetGFxMember(CachedMoreHudIconCont, "_iconSize");
	if (!gfxIconSize.IsNumber()) return;

	auto gfxIconScale = GetGFxMember(CachedMoreHudIconCont, "_iconScale");
	if (!gfxIconScale.IsNumber()) return;

	//
	// position 'content', which is the MoreHUD widget that displays
	// magic effects on alchemical substances and food for example
	//

	double effectsSize = WidgetSize.x * MoreHUDEffectsSizeFact;
	CachedMoreHudContent.SetMember("_xscale", effectsSize);
	CachedMoreHudContent.SetMember("_yscale", effectsSize);

	// position to the right of the selection text, but don't intersect gray separation bar
	auto maxHalfWidth = glm::max(textWidth, grayBarHalfWidth);

	auto widgetPos = GetWidgetPos();

	glm::vec2 containerPos(widgetPos.x + maxHalfWidth + (InfoWidgetOffsetLeft * sizeFactor), widgetPos.y - SelectionWidgetHalfHeight);
	auto scaledContainerPos = ToScaledAspectRatioCoordinates(CachedHUD, containerPos);

    CachedMoreHudContent.SetMember("_x", scaledContainerPos.x);
    CachedMoreHudContent.SetMember("_y", scaledContainerPos.y);

	//
	// position icons
	//

	int arraySize = CachedMoreHUDLoadedIcons.GetArraySize();
	double iconSize = gfxIconSize.GetNumber() * gfxIconScale.GetNumber();
	double iconRealSize = iconSize * sizeFactor;

	auto scaledWidgetPos = ToScaledAspectRatioCoordinates(CachedHUD, glm::vec2(widgetPos.x - textWidth - MoreHUDIconsOffsetRight * sizeFactor, widgetPos.y));

	for (int i = 0; i < arraySize; i++)
	{
		RE::GFxValue currIcon;
		CachedMoreHUDLoadedIcons.GetElement(i, &currIcon);

		if (!currIcon.IsObject())
			continue;

		// size of the icon container. Could probably get dynamically, but I don't think it matters
        glm::vec2 iconPos(640, 480);

		iconPos = ToScaledAspectRatioCoordinates(CachedHUD, iconPos);

		currIcon.SetMember("_x", -iconPos.x + scaledWidgetPos.x + iconSize - (i * MoreHUDIconMargin * sizeFactor) - (i * iconRealSize));
 		currIcon.SetMember("_y", -iconPos.y + scaledWidgetPos.y - iconRealSize / 2.0);

 		currIcon.SetMember("_xscale", iconRealSize);
		currIcon.SetMember("_yscale", iconRealSize);
	}
}

void SelectionWidget::SetMoreHUDWidgetVisibility(bool mode)
{
	std::lock_guard lock(m_manipulateMoreHUD);

	auto ui = RE::UI::GetSingleton();
	if (!ui) return;

	auto hud = ui->GetMenu(RE::HUDMenu::MENU_NAME);
	if (!hud || !hud->uiMovie) return;

	RE::GFxValue iconCont;
	hud->uiMovie->GetVariable(&iconCont, "ahz.scripts.widgets.AHZHudInfoWidget.IconContainer");
	if (!iconCont.IsObject()) return;

	if (mode)
		iconCont.Invoke("Show");
	else
		iconCont.Invoke("Hide");

	RE::GFxValue widgetCont;
	hud->uiMovie->GetVariable(&widgetCont, "AHZWidgetContainer.AHZWidget.content");
	if (!widgetCont.IsObject()) return;

	widgetCont.SetMember("_visible", mode);
}

void SelectionWidget::SetQuickLootWidgetVisibility(bool mode)
{
	RE::GFxValue qlWidget = GetQuickLootWidget();
	if (!qlWidget.IsObject())
		return;

	qlWidget.SetMember("_visible", mode);
}

bool SelectionWidget::CacheQuickLootValues()
{
	auto qlWidget = GetQuickLootWidget();
	if (qlWidget.IsNull() || !qlWidget.IsObject())
		return false;

	auto widgetPosX = GetGFxMember(qlWidget, "_x");
	auto widgetPosY = GetGFxMember(qlWidget, "_y");
	auto widgetScaleX = GetGFxMember(qlWidget, "_xscale");
	auto widgetScaleY = GetGFxMember(qlWidget, "_yscale");
	auto widgetWidth = GetGFxMember(qlWidget, "_width");
	auto widgetHeight = GetGFxMember(qlWidget, "_height");

	Compatibility::QuickLootIE::WidgetPosOrigX = widgetPosX.GetNumber();
	Compatibility::QuickLootIE::WidgetPosOrigY = widgetPosY.GetNumber();

	Compatibility::QuickLootIE::WidgetScaleOrigX = widgetScaleX.GetNumber();
	Compatibility::QuickLootIE::WidgetScaleOrigY = widgetScaleY.GetNumber();

	Compatibility::QuickLootIE::WidgetHeightOrig = widgetWidth.GetNumber();
	Compatibility::QuickLootIE::WidgetWidthOrig = widgetHeight.GetNumber();

	Compatibility::QuickLootIE::ValuesCached = true;
	return true;
}

// this is pure guesswork. There is a difference in scale between my menu and QuickLootIE's menu, but I can't
// figure out where to get the correct factor between them at runtime. Found this value through experimentation
// and seems to work
constexpr double QUICKLOOT_SCALE_FACTOR = 1.5;

void SelectionWidget::UpdateQuickLootWidget()
{
	if (!UIValuesCached)
		return;

	 if (!Compatibility::QuickLootIE::ValuesCached && !CacheQuickLootValues())
		return;

	// set to 2D location?
	if (!Settings::GetWidget3DEnabled())
	{
		ResetQuickLootWidget();
		return;
	}

	//
	// 3D location: just move relative to 3D widget and adjust scale
	//

	auto qlMenu = CachedUI->GetMenu(Compatibility::QuickLootIE::MENU_NAME);

	auto qlWidget = GetQuickLootWidget();
	if (qlWidget.IsNull() || !qlWidget.IsObject() || !qlMenu ||!qlMenu->uiMovie)
		return;

	auto widgetPos = GetWidgetPos();
	double sizeFactor = WidgetSize.x / 100.0;

	qlWidget.SetMember("_x", (widgetPos.x - Compatibility::QuickLootIE::WidgetScaleOrigX * sizeFactor * 2.0f) * QUICKLOOT_SCALE_FACTOR);
 	qlWidget.SetMember("_y", widgetPos.y * QUICKLOOT_SCALE_FACTOR);

 	qlWidget.SetMember("_xscale", Compatibility::QuickLootIE::WidgetScaleOrigX * sizeFactor);
	qlWidget.SetMember("_yscale", Compatibility::QuickLootIE::WidgetScaleOrigY * sizeFactor);
}

void SelectionWidget::ResetQuickLootWidget()
{
	if (!Compatibility::QuickLootIE::ValuesCached)
		return;

	auto qlWidget = GetQuickLootWidget();
	if (qlWidget.IsNull() || !qlWidget.IsObject())
		return;

	qlWidget.SetMember("_x", Compatibility::QuickLootIE::WidgetPosOrigX);
	qlWidget.SetMember("_y", Compatibility::QuickLootIE::WidgetPosOrigY);
}

RE::GFxValue SelectionWidget::GetQuickLootWidget()
{
	auto qlMenu = CachedUI->GetMenu(Compatibility::QuickLootIE::MENU_NAME);
	if (!qlMenu || !qlMenu->uiMovie) return RE::GFxValue();

	RE::GFxValue qlWidget;
	qlMenu->uiMovie->GetVariable(&qlWidget, "lootMenu");
	
	return qlWidget;
}

void SelectionWidget::SetProgressCirlceValue(float newValue)
{
	if (Settings::IsDismountProgressCirlcleEnabled)
		ProgressCircleValue = newValue;
	else
		ProgressCircleValue = 0.0;
}

double SelectionWidget::GetWidgetTargetSize(FocusObject* focusObj)
{
    if (!focusObj)
		return Settings::GetWidgetSizeMin();

	glm::dvec3 widgetPos;
	if (!Get3DWidgetPos(focusObj, widgetPos))
		return Widget2DSize;

	glm::vec3 glmWidgetPos(widgetPos.x, widgetPos.y, widgetPos.z);

	glm::vec3 cameraPos = Util::GetCameraPos();
	double distToObject = glm::length(cameraPos - glmWidgetPos);

	double distFactor = std::min(distToObject / GetMaxCameraDist(), 1.0);
	double distFactorInv = 1 - distFactor;

	double widgetSizeMin = Settings::GetWidgetSizeMin();
	double widgetSizeMax = Settings::GetWidgetSizeMax();

	return distFactorInv * (widgetSizeMax - widgetSizeMin) + widgetSizeMin;
}

glm::vec2 SelectionWidget::GetScreenLoc(const RE::GPtr<RE::IMenu>& menu, glm::vec3 worldPos)
{
	glm::vec3 screenLoc;
	RE::NiCamera::WorldPtToScreenPt3((float(*)[4])Offsets::WorldToCamMatrix.address(), *(RE::NiRect<float>*)Offsets::ViewPort.address(),
		RE::NiPoint3((float)worldPos.x, (float)worldPos.y, (float)worldPos.z),
		screenLoc.x, screenLoc.y, screenLoc.z, 1e-5f);

	return GetScreenLocFromPercentage(menu, screenLoc);
}

glm::vec2 SelectionWidget::GetScreenLocFromPercentage(RE::GPtr<RE::IMenu> menu, glm::vec2 posIn)
{
    if (!menu)
        return posIn;

	glm::vec2 posOut = posIn;
    RE::GRectF rect = SelectionWidgetMenu::GetMenuRect(menu.get());

	posOut.x = rect.left + (rect.right - rect.left) * posOut.x;
	posOut.y = 1.0f - posOut.y;  // flip y for Flash coordinate system
	posOut.y = rect.top + (rect.bottom - rect.top) * posOut.y;

	return posOut;
}

glm::vec2 SelectionWidget::ClampWidgetToScreenRect(const RE::GPtr<RE::IMenu>& menu, glm::vec2 posIn, RE::GFxValue selectionWidget)
{
	if (!selectionWidget.IsObject() || !menu)
		return posIn;

	RE::GRectF rect = SelectionWidgetMenu::GetMenuRect(menu.get());
	double gfxTextWidth = GetGFxMember(CachedActivateText, "textWidth").GetNumber();
	double gfxWidgetHeight = GetGFxMember(selectionWidget, "_height").GetNumber();

	glm::vec2 halfTextBounds = PixelToStageCoordinates(menu, glm::vec2(gfxTextWidth, gfxWidgetHeight)) * 0.5f;

	if (posIn.x < halfTextBounds.x || posIn.x > rect.right - halfTextBounds.x ||
		posIn.y < halfTextBounds.y || posIn.y > rect.bottom - halfTextBounds.y)
	{
		glm::vec2 allowedMin(rect.left + halfTextBounds.x, rect.top + halfTextBounds.y);
		glm::vec2 allowedMax(rect.right - halfTextBounds.x, rect.bottom - halfTextBounds.y);
		// center location within allowed rect (accounting for text bounds)
		glm::vec2 allowedCenter = (allowedMin + allowedMax) * 0.5f;

		// direction from center to posIn
		glm::vec2 centerToPos = posIn - allowedCenter;
		glm::vec2 dirToPos = glm::normalize(centerToPos);

		float tX = (dirToPos.x > 0) 
            ? (allowedMax.x - allowedCenter.x) / dirToPos.x 
            : (allowedMin.x - allowedCenter.x) / dirToPos.x;
		float tY = (dirToPos.y > 0) 
			? (allowedMax.y - allowedCenter.y) / dirToPos.y 
			: (allowedMin.y - allowedCenter.y) / dirToPos.y;

		float moveMagnitude = std::min(tX, tY);

		// starting at allowedCenter, move on dirToPos for moveMagnitude screen units. This gives
		// a more accurate position (to the original 3D location) than a simple clamp
		posIn = allowedCenter + dirToPos * moveMagnitude;
	}

	return posIn;
}

bool SelectionWidget::Get3DWidgetPos(FocusObject* focusObj, glm::dvec3& posOut)
{
	// is ref still valid?
	if (!focusObj || !focusObj->ObjectRef.get() || focusObj->ObjectRef.get()->IsDisabled() || focusObj->ObjectRef.get()->IsMarkedForDeletion())
	{
		if (LastWidgetPos.has_value())
		{
			posOut = LastWidgetPos.value();
			return true;
		}

		return false;
	}

	auto focusRef = focusObj->ObjectRef.get();

	// place widget over characters' head, but in center for other objects, or for dead characters
	auto characterObject = focusRef->As<RE::Actor>();

	ObjectOverride objOverride;
	bool hasOverride = Settings::GetObjectOverride(focusRef.get(), objOverride);

	if (characterObject && (
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kAlive ||
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kBleedout ||
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kDying ||
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kEssentialDown ||
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kRestrained ||
		characterObject->AsActorState()->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kReanimate
		))
	{
		const char* raceName = characterObject->GetRace()->GetName();
		// draw name above bb for horses, instead of at bb center or head bone location
		if (strcmp(raceName, "Horse") == 0)
		{
			posOut = Util::GetBoundingBoxTop(focusObj);
		}
		else
		{
            if (auto characterHead = Util::GetCharacterHead(focusRef.get()))
			{
				auto worldTranslate = characterHead->world.translate;
				posOut = glm::dvec3(worldTranslate.x, worldTranslate.y, worldTranslate.z);
			}
			else
				posOut = Util::GetBoundingBoxCenter(focusObj);

			posOut.z += Settings::GetCurrentWidgetZOffset() + Settings::WidgetZOffsetAdditionalNPC;
		}
	}
	else
	{
		if (hasOverride)
        {
            Get3DWidgetBasePos(focusObj, posOut, objOverride.WidgetBasePos);
			
			if (objOverride.HasWidgetPosOffset)
            {
                auto rotation = focusObj->CollisionObject.BoundingBox.rotation;
                posOut += objOverride.WidgetPosOffset;
            }
			else if (objOverride.HasWidgetPosOffsetRel)
			{
                auto rotation = focusObj->CollisionObject.BoundingBox.rotation;
                posOut += Util::RotateVector(rotation, objOverride.WidgetPosOffset);
			}
        }
		else
		{
            Get3DWidgetBasePos(focusObj, posOut, ObjectOverride::WidgetPos::Auto);
		}
	}
	
	return true;
}

bool SelectionWidget::Get3DWidgetBasePos(FocusObject* focusObj, glm::dvec3& posOut, ObjectOverride::WidgetPos pos)
{
	switch (pos)
	{
    case ObjectOverride::WidgetPos::Auto:

		return Get3DWidgetBasePosAuto(focusObj, posOut);

    case ObjectOverride::WidgetPos::Center:

		posOut = Util::GetBoundingBoxCenter(focusObj);
        return true;

    case ObjectOverride::WidgetPos::Top:

		posOut = Util::GetBoundingBoxTop(focusObj);
        return true;

    case ObjectOverride::WidgetPos::Bottom:

		posOut = Util::GetBoundingBoxBottom(focusObj);
        return true;

    case ObjectOverride::WidgetPos::Root:

		posOut = Util::GetObjectAccuratePosition(focusObj->ObjectRef.get().get());
        return true;
	}

	return false;
}

bool SelectionWidget::Get3DWidgetBasePosAuto(FocusObject* focusObj, glm::dvec3& posOut)
{
    auto focusRef = focusObj->ObjectRef.get().get();

    auto mesh = focusRef->GetCurrent3D();
    if (!mesh)
        return false;

    switch (auto collisionSize = FocusObject::GetCollisionSize(focusObj->CollisionObject.BoundingBox.boundMax.z))
    {
    case Tiny:
        posOut = Util::GetBoundingBoxTop(focusObj);

		posOut.z += Settings::GetCurrentWidgetZOffset();
        break;
    case Small:
        posOut = Util::GetBoundingBoxTop(focusObj);

		posOut.z += Settings::GetCurrentWidgetZOffset();
        break;
    default:
        posOut = Util::GetBoundingBoxCenter(focusObj);
        break;
    }

	return true;
}

glm::dvec2 SelectionWidget::Get3DWidgetSize(FocusObject* focusObj)
{
	double scaleFactor = GetWidgetTargetSize(focusObj);
	return glm::vec2(glm::vec2(OrigMCSize.x * scaleFactor, OrigMCSize.y * scaleFactor));
}

glm::dvec2 SelectionWidget::Get2DWidgetPos(const RE::GPtr<RE::IMenu>& menu)
{
	return GetScreenLocFromPercentage(menu, Widget2DPos);
}

glm::dvec2 SelectionWidget::Get2DWidgetSize()
{
	return glm::vec2(OrigMCSize.x * Widget2DSize, OrigMCSize.y * Widget2DSize);
}

glm::vec2 SelectionWidget::PixelToStageCoordinates(const RE::GPtr<RE::IMenu>& menu, glm::vec2 vecIn)
{
    RECT windowRect;
    GetClientRect(GetForegroundWindow(), &windowRect);

	RE::GRectF stageRect = menu->uiMovie->GetVisibleFrameRect();

	vecIn.x *= (stageRect.right / windowRect.right);
    vecIn.y *= (stageRect.bottom / windowRect.bottom);

	return vecIn;
}

glm::vec2 SelectionWidget::StageToPixelCoordinates(RE::GPtr<RE::IMenu> menu, glm::vec2 vecIn)
{
	if (!menu || !menu->uiMovie)
		return vecIn;

	RECT windowRect;
    GetClientRect(GetForegroundWindow(), &windowRect);

	RE::GRectF stageRect = menu->uiMovie->GetVisibleFrameRect();

	vecIn.x *= windowRect.right / stageRect.right;
    vecIn.y *= windowRect.bottom / stageRect.bottom;

	return vecIn;
}

glm::vec2 SelectionWidget::ToScaledAspectRatioCoordinates(RE::GPtr<RE::IMenu> menu, glm::vec2 vecIn)
{
	if (!menu || !menu->uiMovie)
		return vecIn;

    RECT windowRect;
    GetClientRect(GetForegroundWindow(), &windowRect);

	RE::GRectF menuStageRect = menu->uiMovie->GetSafeRect();

	float targetHeight = (float)windowRect.right * 0.5625f; // 16/9 = 0.5625f, Skyrim's Flash menus use 16/9 as standard
	float actualHeight = (float)windowRect.bottom;
	float heightPercentage = targetHeight / actualHeight;

	return glm::vec2( vecIn.x + ( (menuStageRect.right / 2.0) * (heightPercentage - 1.0) ),
		vecIn.y);
}

void SelectionWidget::SetElementPos(glm::vec2 screenPos, RE::GFxValue& element)
{
	element.SetMember("_x", screenPos.x);
	element.SetMember("_y", screenPos.y);
}

void SelectionWidget::SetElementSize(glm::vec2 size, RE::GFxValue& element)
{
 	element.SetMember("_xscale", size.x);
 	element.SetMember("_yscale", size.y);
}

bool SelectionWidget::InitUIValues()
{
	if (!ModulesLoadedChecked)
	{
		if (Compatibility::MoreHUD::CheckModuleLoaded() && Compatibility::SkyHUD::CheckModuleLoaded())
			ModulesLoadedChecked = true;
		else
			return false;
	}

	if (!CacheGFxObjects())
	{
		logger::info("BTPS: failed to cache GFxObjects. This will likely cause problems with UI manipulation.");
		return false;
	}

	if (!OrigUIValuesSet)
	{
		OrigMCPos = glm::vec2(
			GetGFxMember(CachedSelectionWidget, "_x").GetNumber(),
			GetGFxMember(CachedSelectionWidget, "_x").GetNumber());

		OrigMCSize = glm::vec2(
			GetGFxMember(CachedSelectionWidget, "_xscale").GetNumber(),
			GetGFxMember(CachedSelectionWidget, "_yscale").GetNumber());

		OrigUIValuesSet = true;
	}

	logger::info("BTPS: successfully initialized SelectionWidget UI values");

	return true;
}

void SelectionWidget::OnControlsToggled(bool mode)
{
	if (mode)
		SelectionWidgetMenu::Show("ControlsToggled");
	else
		SelectionWidgetMenu::Hide("ControlsToggled");
}

void SelectionWidget::OnFreeCamToggled(bool mode)
{
	if (!mode)
		SelectionWidgetMenu::Show("FreeCamToggled");
	else
		SelectionWidgetMenu::Hide("FreeCamToggled");
}

void SelectionWidget::FadeInInstant()
{
	SetUIFocusRef(FocusManager::GetActiveFocusObject());
	FadeMode = EUIFadeMode::NONE;
	FadeProgress = 0.0;
}

void SelectionWidget::FadeOutInstant()
{
	EndFadeOut();
}

void SelectionWidget::StartFadeIn()
{
	// don't update focus object now, but schedule for update as soon as activation text changes
	UpdateFocusObjDeferred = true;

	// invert previous fade progress
	if (FadeProgress > 0.01)
		FadeProgress = 1.0 - FadeProgress;
	else
		FadeProgress = 0.0;

	FadeMode = EUIFadeMode::FADE_IN;
}

void SelectionWidget::StartFadeOut()
{
	// invert previous fade progress
	if (FadeProgress > 0.01)
		FadeProgress = 1.0 - FadeProgress;
	else
		FadeProgress = 0.0;

	FadeMode = EUIFadeMode::FADE_OUT;
}

void SelectionWidget::EndFadeIn()
{
	FadeMode = NONE;
}

void SelectionWidget::EndFadeOut()
{
	SetUIFocusRef(FocusManager::GetActiveFocusObject());
	FadeMode = FADED;

	LastWidgetPos.reset();

	ClearSelectionText();
}

void SelectionWidget::ClearSelectionText()
{
}

void SelectionWidget::ProgressFade()
{
	switch (FadeMode)
	{
		case FADE_IN:
			FadeProgress += Settings::WidgetFadeInDelta; break;
		case FADE_OUT:
			FadeProgress += Settings::WidgetFadeOutDelta; break;
		case FADED:
		case NONE:
			break;
	}

	if (FadeProgress > 1.0)
	{
		FadeProgress = 1.0;

		switch (FadeMode)
		{
			case FADE_IN:
				EndFadeIn(); break;
			case FADE_OUT:
				EndFadeOut(); break;
			case FADED:
			case NONE:
				break;
		}
	}
}

SelectionWidgetMenu::SelectionWidgetMenu()
{
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	if (!scaleformManager)
	{
		logger::error("BTPS: failed to initialize SelectionWidgetMenu");
		return;
	}

	depthPriority = 0;

	menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
	menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
	menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);

	inputContext = Context::kNone;

	if (uiMovie)
	{
		uiMovie->SetMouseCursorCount(0); // disable input
		uiMovie->SetVisible(false);
	}

	scaleformManager->LoadMovieEx(this, MENU_PATH, [this](RE::GFxMovieDef* a_def) -> void
	{
		a_def->SetState(RE::GFxState::StateType::kLog,
			RE::make_gptr<Logger>().get());

		logger::info("BTPS: SelectionWidgetMenu loaded flash");
	});

	InitProgressCirclePosition(this);
}

void SelectionWidgetMenu::InitProgressCirclePosition(const SelectionWidgetMenu* menu)
{
	if (!menu || !menu->uiMovie)
		return;

	RE::GFxValue progressCirlce;
	menu->uiMovie->GetVariable(&progressCirlce, "ProgressCircleInstance");

	if (!progressCirlce.IsObject())
		return;

	auto menuRect = GetMenuRect(menu);

	progressCirlce.SetMember("_x", menuRect.right / 2.0f);
    progressCirlce.SetMember("_y", menuRect.bottom / 2.0f);
}

void SelectionWidgetMenu::Register()
{
	auto ui = RE::UI::GetSingleton();
	if (ui)
	{
		ui->Register(MENU_NAME, Creator);

		logger::info("BTPS: successfully registered SelectionWidgetMenu");
	}
	else
		logger::error("BTPS: failed to register SelectionWidgetMenu");
}

RE::GRectF SelectionWidgetMenu::GetMenuRect(const RE::IMenu* menu)
{
    if (!menu || !menu->uiMovie)
        return {};

    return menu->uiMovie->GetVisibleFrameRect();
}

void SelectionWidgetMenu::Load()
{
	std::lock_guard lock(m_load);

    if (auto msgQ = RE::UIMessageQueue::GetSingleton())
		msgQ->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
	else
		logger::warn("BTPS: failed to load SelectionWidgetMenu");
}

void SelectionWidgetMenu::Unload()
{
	std::lock_guard lock(m_unload);

    if (auto msgQ = RE::UIMessageQueue::GetSingleton())
		msgQ->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
	else
		logger::warn("BTPS: failed to unload SelectionWidgetMenu");
}

double SelectionWidget::GetWidgetCurrentAlpha()
{
	double alphaBase = Settings::WidgetAlpha;

	switch (SelectionWidget::FadeMode)
	{
		case FADE_IN:
			return alphaBase * SelectionWidget::FadeProgress;
		case FADE_OUT:
			return alphaBase * (1.0 - SelectionWidget::FadeProgress);
		case FADED:
			return 0.0;
		case NONE:
			return alphaBase;
	}
	return alphaBase;
}

void SelectionWidget::SetSelectionAlpha(double alpha, RE::GFxValue& selectionWidget)
{
	if (!SelectionWidget::UIValuesCached)
		return;

	CachedActivateText.SetMember("_alpha", alpha);
	CachedInfoText.SetMember("_alpha", alpha);
	CachedGrayBar.SetMember("_alpha", alpha);
	CachedActivateButton.SetMember("_alpha", alpha);
}

void SelectionWidgetMenu::Show(const std::string& source)
{
	std::lock_guard lock(m_visibility);

	if (!SelectionWidget::UIValuesCached)
		return;

	if (Hidden_Sources.empty())
		return;

	if (!source.empty())
	{
		auto sourceIdx = std::find(Hidden_Sources.begin(), Hidden_Sources.end(), source);
		if (sourceIdx != Hidden_Sources.end())
			Hidden_Sources.erase(sourceIdx);
	}

	if (IsEnabled && Hidden_Sources.empty())
	{
		ToggleVisibility(true);
		//SelectionWidget::Update();
	}
}

void SelectionWidgetMenu::Hide(const std::string& source)
{
	std::lock_guard lock(m_visibility);

	if (!SelectionWidget::UIValuesCached)
		return;

	auto sourceIdx = std::find(Hidden_Sources.begin(), Hidden_Sources.end(), source);
	if (sourceIdx == Hidden_Sources.end())
	{
		Hidden_Sources.push_back(source);

		ToggleVisibility(false);
		//SelectionWidget::Update();
	}
}

void SelectionWidgetMenu::ToggleVisibility(const bool mode)
{
	std::lock_guard lock(m_visibility);

	if (!SelectionWidget::UIValuesCached)
		return;

	auto ui = RE::UI::GetSingleton();
	if (!ui)
		return;

	auto widgetMenu = ui->GetMenu(SelectionWidgetMenu::MENU_NAME);
	if (!widgetMenu || !widgetMenu->uiMovie)
		return;

	widgetMenu->uiMovie->SetVisible(mode);
	SelectionWidget::HideNativeHUD();

	IsVisible = mode;
}

void SelectionWidgetMenu::AdvanceMovie(const float a_interval, const std::uint32_t a_currentTime)
{
	RE::IMenu::AdvanceMovie(a_interval, a_currentTime);

	SelectionWidget::Update();
}
