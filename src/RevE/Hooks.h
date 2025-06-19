#pragma once

namespace Hooks
{
	void Install();

	class ActivateHook
	{
	public:
		static void Actor_Dismount(RE::Actor* a_this);

		static void Hook();

	private:
		static void ProcessButton(RE::ActivateHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class UpdateCrosshairTextHook
	{
	public:
		static void Hook();

		static void OnUpdateCrosshairText(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data);
		static void OnUpdateCrosshairText_AutoLoadDoor(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data);
	private:
		static inline REL::Relocation<decltype(OnUpdateCrosshairText)> _OnUpdateCrosshairText;
		static inline REL::Relocation<decltype(OnUpdateCrosshairText_AutoLoadDoor)> _OnUpdateCrosshairText_AutoLoadDoor;
	};

	class SelectionHook
	{
	public:
		static RE::TESObjectREFR* OnSetCrosshairTargetRef(RE::ObjectRefHandle objectRefHandle);

		static void Hook();

		static bool IsValidFormType(RE::FormType t);
	};

	class MainUpdateHook
	{
	public:
		static void Hook();

	private:
		static void Update(RE::Main* a_this, float a2);
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class HorseBackHook
	{
	public:
		static bool FalseFunc() { return false; };

		static void Hook();
	};
}
