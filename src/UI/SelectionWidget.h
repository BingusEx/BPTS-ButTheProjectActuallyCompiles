#pragma once
#include "lib/FocusObject.h"
#include "lib/ObjectOverride.h"

enum EUIFadeMode
{
	NONE,
	FADE_IN,
	FADE_OUT,
	FADED
};

class SelectionWidget
{
private: 
	static std::shared_ptr<FocusObject> UIFocusRef;

	static RE::UI* CachedUI;
    static RE::GPtr<RE::IMenu> CachedHUD;
    static RE::GPtr<RE::IMenu> CachedWidgetMenu;

    static RE::GFxValue CachedSelectionWidget;
    static RE::GFxValue CachedActivateText;
    static RE::GFxValue CachedGrayBar;
	static RE::GFxValue CachedRolloverText;
    static RE::GFxValue CachedRolloverInfoText;
    static RE::GFxValue CachedRolloverButton_tf;
    static RE::GFxValue CachedRolloverGrayBar_mc;
    static RE::GFxValue CachedActivateButton;
    static RE::GFxValue CachedInfoText;

    static RE::GFxValue CachedWidgetCont;
    static RE::GFxValue CachedAhzWidget;
    static RE::GFxValue CachedMoreHudContent;
    static RE::GFxValue CachedMoreHudIconCont;
    static RE::GFxValue CachedMoreHudTextField;
    static RE::GFxValue CachedMoreHUDLoadedIcons;

public:

	static constexpr double MAX_CAMERA_DIST = 800.0f;
	static constexpr double MAX_CAMERA_DIST_HORSEBACK = 1000.0f;

	/*enum : std::size_t
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
	};*/

	// cached values to avoid querying values from flash all the time

	// saves last known object pos during fadeout, in case object doesn't exist anymore
	static std::optional<glm::dvec3> LastWidgetPos;

	// cached values to avoid querying values from flash all the time
	static glm::vec2 WidgetPos;
	static glm::vec2 WidgetSize;

	static glm::vec2 GetWidgetPos();
	static void SetWidgetPos(glm::vec2 pos);

	static glm::vec2 GetWidgetSize();
	static void SetWidgetSize(glm::vec2 size);

    static void OnPerspectiveToggled(bool cond);
	static void OnNativeSelectionChanged(RE::TESObjectREFR* nativeSelection);
	static void OnUpdateCrosshairText();
	static void OnUpdateCrosshairText2D();

	static bool CacheGFxObjects();

    //

	static double Widget2DSize;
	// position as percentage, 0.0 - 1.0 of screen size
	static glm::vec2 Widget2DPos;

	static float ProgressCircleValue;

	static double GetMaxCameraDist();

	static void SetUIFocusRef(const std::shared_ptr<FocusObject>& focusRef);
	static std::shared_ptr<FocusObject> GetUIFocusRef();

	static void Update();
	static void UpdateWidgetPos();
	static void UpdateWidgetText();
	static void HideNativeHUD();
	static void UpdateProgressCircle(const RE::GPtr<RE::IMenu>& menu);
	static void OnSelectionChanged();
	static void TryUpdateSelectionBounds();

	// hide widget if the selected actor is the same as the TDM target to avoid overlapping names
	static void UpdateTDMVisibility();

	static glm::vec2 ToLocal(glm::vec2 posIn, RE::GFxValue element, RE::GPtr<RE::IMenu> menu);
	static glm::vec2 ToGlobal(glm::vec2 posIn, RE::GFxValue element, RE::GPtr<RE::IMenu> menu);

	static void UpdateMoreHUDWidget();
	static void SetMoreHUDWidgetVisibility(bool mode);

	static void SetQuickLootWidgetVisibility(bool mode);

    static bool CacheQuickLootValues();
	static void UpdateQuickLootWidget();
	static void ResetQuickLootWidget();
	static RE::GFxValue GetQuickLootWidget();

	static void SetProgressCirlceValue(float newValue);

	static double GetWidgetTargetSize(FocusObject* focusObj);

	static glm::vec2 GetScreenLoc(const RE::GPtr<RE::IMenu>& menu, glm::vec3 worldPos);
    static glm::vec2 GetScreenLocFromPercentage(RE::GPtr<RE::IMenu> menu, glm::vec2 posIn);
	static glm::vec2 ClampWidgetToScreenRect(const RE::GPtr<RE::IMenu>& menu, glm::vec2 posIn, RE::GFxValue selectionWidget);

	static bool Get3DWidgetPos(FocusObject* focusObj, glm::dvec3& posOut);
    static bool Get3DWidgetBasePos(FocusObject* focusObj, glm::dvec3& posOut, ObjectOverride::WidgetPos pos);
    static bool Get3DWidgetBasePosAuto(FocusObject* focusObj, glm::dvec3& posOut);

	static glm::dvec2 Get3DWidgetSize(FocusObject* focusObj);
    static glm::dvec2 Get2DWidgetPos(const RE::GPtr<RE::IMenu>& menu);
	static glm::dvec2 Get2DWidgetSize();

	static glm::vec2 PixelToStageCoordinates(const RE::GPtr<RE::IMenu>& menu, glm::vec2 vecIn);
	static glm::vec2 StageToPixelCoordinates(RE::GPtr<RE::IMenu> menu, glm::vec2 vecIn);

	static glm::vec2 ToScaledAspectRatioCoordinates(RE::GPtr<RE::IMenu> menu, glm::vec2 vecIn);

	static void SetElementPos(glm::vec2 screenPos, RE::GFxValue& element);
	static void SetElementSize(glm::vec2 size, RE::GFxValue& element);

	static bool InitUIValues();

	static void OnControlsToggled(bool mode);
	static void OnFreeCamToggled(bool mode);

    static bool UIValuesCached;
    static bool ModulesLoadedChecked;
    static bool OrigUIValuesSet;

    static std::string LastActivateText;
    static std::string LastActivateName;

	static double FadeProgress;
	static EUIFadeMode FadeMode;

	static void ProgressFade();
	static void FadeInInstant();
	static void FadeOutInstant();
	static void StartFadeIn();
	static void StartFadeOut();

	static void EndFadeIn();
	static void EndFadeOut();

	static void ClearSelectionText();

	// Whatever is the current selection needs to be displayed 2D, such as an AutoLoadDoor
	static bool SelectionIs2DOverride;

	// This is here to defeat a race condition I've been dealing with for ages, causing slight flickering in the activation text,
	// especially with other mods updating the activation text involved, such as MoreHUD.
	// 
	// The widget's position updated before the text changed, due to the FocusObject changing and UI code updating position,
	// but text changes not applying immediately.
	// 
	// Using this, I only update the reference the UI uses for the position
	// after the text has changed. Terrible workaround, but I don't have a better solution.
	//
	// 1. FocusObject updates in FocusManager - not forwarded to the UI: UpdateFocusObjDeferred is enabled
	// 2. selection text change is requested from main thread, but Scaleform doesn't immediately update
	// 3. one UI frame (?) later, the text actually updates (I schedule a UITask to account for this in SelectionWidget::OnUpdateCrosshairText)
	// 4. now UIFocusObject is updated with FocusObject of FocusManager, UpdateFocusObjDeferred disabled. No more flickering :)
	static bool UpdateFocusObjDeferred;

	static void OnChangeActivateText();

	static glm::vec2 OrigMCPos;
	static glm::vec2 OrigMCSize;

	static double GetWidgetCurrentAlpha();
	static void SetSelectionAlpha(double alpha, RE::GFxValue& selectionWidget);

	[[nodiscard]] static RE::GFxValue GetGFxMember(RE::GFxValue& object, const char* memberName)
	{
		RE::GFxValue valueOut;
		object.GetMember(memberName, &valueOut);

		return valueOut;
	}

	[[nodiscard]] static RE::GFxValue GetHUDRoot(RE::GPtr<RE::IMenu> hud)
	{
		RE::GFxValue rootElement;
		hud->uiMovie->GetVariable(&rootElement, "HUDMovieBaseInstance");

		return rootElement;
	}

private:
	class Logger : public RE::GFxLog
	{
	public:
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
		{
			std::string fmt(a_fmt ? a_fmt : "");
			while (!fmt.empty() && fmt.back() == '\n') {
				fmt.pop_back();
			}

			std::va_list args;
			va_copy(args, a_argList);
			std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
			std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
			va_end(args);

			logger::info("{}"sv, buf.data());
		}
	};
};

class SelectionWidgetMenu : RE::IMenu
{
public:
	static constexpr const char* MENU_PATH = "BetterThirdPersonSelection/BTPS_menu";
	static constexpr const char* MENU_NAME = "BTPS Menu";

	SelectionWidgetMenu();

	static void InitProgressCirclePosition(const SelectionWidgetMenu* menu);

	static void Register();

	static std::vector<std::string> Hidden_Sources;

    static RE::GRectF GetMenuRect(const RE::IMenu* menu);

	// somehow, when the main menu first opens, it sends an opening event, and immediately a closing event - before
	// any actual user input. So to avoid the widget showing up in the main menu, I disable it entirely until
	// the first time a game is started
	static bool IsEnabled;
    static bool IsVisible;

	static void SetEnabled(bool mode);
	static void Load();
	static void Unload();

	// while the above disables the entire menu, this only hides the topmost flash elements -
	// in some cases this is desirable because menus can interrupt other UI events when showing, such as
	// showing another element when the dialog menu closes can cause follower commands to be interrupted 
	static void Show(const std::string& source);
	static void Hide(const std::string& source);
	static void ToggleVisibility(bool mode);

	static RE::stl::owner<RE::IMenu*> Creator() { return new SelectionWidgetMenu(); }

	void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;

private:
	class Logger : public RE::GFxLog
	{
	public:
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
		{
			std::string fmt(a_fmt ? a_fmt : "");
			while (!fmt.empty() && fmt.back() == '\n') {
				fmt.pop_back();
			}

			std::va_list args;
			va_copy(args, a_argList);
			std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
			std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
			va_end(args);

			logger::info("{}"sv, buf.data());
		}
	};
};
