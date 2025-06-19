#pragma once

class InputHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:

	static InputHandler* GetSingleton();
	static int MakeScanCode(RE::InputEvent* a_event);
/*    static int MakeIDCode(int scanCode);*/

	std::vector<int> PressedKeys;

	bool IsKeyPressed(int keyCode);
    void AddPressedKey(int keyCode);
    void RemovePressedKey(int keyCode);
	
	std::vector<int> ToggledKeys;

	bool IsKeyToggled(int keyCode);
    void AddToggledKey(int keyCode);
    void RemoveToggledKey(int keyCode);

	bool IsModifierKeyDown = false;
	unsigned __int64 NextIncDecTickCount = 0;

	static bool IsEnabled;

	void Enable();
	void Disable();

	bool IsMouseWheelEnabled = true;
	bool ShouldToggleMouseWheel();
	void EnableMouseWheel();
	void DisableMouseWheel();

	void SelectIncreasePriority();
	void SelectDecreasePriority();

	RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_source) override;
	void HandleInputRecursive(RE::InputEvent* a_event, RE::BSTEventSource<RE::InputEvent*>* a_source);
};
