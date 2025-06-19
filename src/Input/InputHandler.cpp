#pragma once
#include "InputHandler.h"
#include "RevE/Hooks.h"
#include "FocusManager.h"
#include "Settings.h"
#include "BPTSInputMap.h"
#include "UI/SelectionWidget.h"
#include "windows.h"

#include "lib/Util.h"

InputHandler* InputHandler::GetSingleton()
{
	static InputHandler singleton;
	return std::addressof(singleton);
}

int InputHandler::MakeScanCode(RE::InputEvent* a_event)
{
	if (!a_event)
		return -1;

	auto buttonEvent = a_event->AsButtonEvent();
	if (buttonEvent)
	{
		int idCode = buttonEvent->GetIDCode();
		int keyCode = idCode;

		switch (buttonEvent->device.get())
		{
		case RE::INPUT_DEVICE::kMouse:
			keyCode = BPTSInputMap::kMacro_MouseButtonOffset + idCode;
			break;
		case RE::INPUT_DEVICES::kGamepad:
			keyCode = BPTSInputMap::GamepadMaskToKeycode(idCode);
		}

		return keyCode;
	}

	return -1;
}

void InputHandler::AddPressedKey(int keyCode)
{
    if (std::find(PressedKeys.begin(), PressedKeys.end(), keyCode) == PressedKeys.end())
    {
        PressedKeys.push_back(keyCode);
	}
}

void InputHandler::RemovePressedKey(int keyCode)
{
    PressedKeys.erase(std::remove(PressedKeys.begin(), PressedKeys.end(), keyCode), PressedKeys.end());
}

bool InputHandler::IsKeyToggled(int keyCode)
{
    return std::find(ToggledKeys.begin(), ToggledKeys.end(), keyCode) != ToggledKeys.end();
}

void InputHandler::AddToggledKey(int keyCode)
{
    if (std::find(ToggledKeys.begin(), ToggledKeys.end(), keyCode) == ToggledKeys.end())
    {
        ToggledKeys.push_back(keyCode);
    }
}

void InputHandler::RemoveToggledKey(int keyCode)
{
    ToggledKeys.erase(std::remove(ToggledKeys.begin(), ToggledKeys.end(), keyCode), ToggledKeys.end());
}

bool InputHandler::IsKeyPressed(int keyCode)
{
    return std::find(PressedKeys.begin(), PressedKeys.end(), keyCode) != PressedKeys.end();
}

// int InputHandler::MakeIDCode(int scanCode)
// {
//     auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
//     if (!deviceManager)
//         return scanCode;
// 
//     auto mouse = deviceManager->GetMouse();
// 	if (mouse)
//         return scanCode - InputMap::kMacro_MouseButtonOffset;
// 
//     auto gamepad = deviceManager->GetGamepad();
//     if (gamepad)
//         return InputMap::GamepadKeycodeToMask(scanCode);
// 
// 	return scanCode;
// }

RE::BSEventNotifyControl InputHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
{
	auto eventPtr = *a_event;

	HandleInputRecursive(eventPtr, a_source);

	return RE::BSEventNotifyControl::kContinue;
}

void InputHandler::HandleInputRecursive(RE::InputEvent* a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
{
	if (!a_event)
		return;

	auto buttonEvent = a_event->AsButtonEvent();

	if (buttonEvent && buttonEvent->HasIDCode())
	{
		int keyIdScanCode = MakeScanCode(a_event);

		if (buttonEvent->IsDown() || buttonEvent->IsPressed())
		{
            AddPressedKey(keyIdScanCode);

			auto ui = RE::UI::GetSingleton();

			if (ui && !ui->GameIsPaused() && !buttonEvent->IsHeld() && 
				SelectionWidgetMenu::IsVisible && FocusManager::GetIsEnabled())
            {
                if (IsKeyToggled(keyIdScanCode))
                    RemoveToggledKey(keyIdScanCode);
                else
                    AddToggledKey(keyIdScanCode);
            }

		}
		else
		{
            RemovePressedKey(keyIdScanCode);
		}

		if (!Settings::EnableObjectCycle)
        {
            if (!IsMouseWheelEnabled)
                EnableMouseWheel();
            return;
        }

		if (keyIdScanCode == Settings::ObjectCycleModifierKey)
		{
            if (buttonEvent->IsDown() || buttonEvent->IsPressed() || buttonEvent->IsHeld())
			{
                DisableMouseWheel();
                IsModifierKeyDown = true;
			}
			else
			{
                EnableMouseWheel();
                IsModifierKeyDown = false;
			}
		}
		// mousewheel down
		if ( (IsModifierKeyDown || Settings::ObjectCycleModifierKey < 0) && // -1 = unbound
			keyIdScanCode == Settings::ObjectCycleUpKey && buttonEvent->value > 0.0)
		{
			SelectIncreasePriority();
		}
		// mousewheel up
        else if ((IsModifierKeyDown || Settings::ObjectCycleModifierKey < 0) &&
			keyIdScanCode == Settings::ObjectCycleDownKey && buttonEvent->value > 0.0)
		{
			SelectDecreasePriority();
		}
	}

	auto nextEvent = a_event->next;
	if (nextEvent && nextEvent->HasIDCode())
		HandleInputRecursive(nextEvent, a_source);
}

bool InputHandler::IsEnabled = false;

void InputHandler::Enable()
{
	auto input = RE::BSInputDeviceManager::GetSingleton();
	if (input)
	{
		input->AddEventSink(this);
        IsEnabled = true;

		logger::info("BTPS: successfully enabled InputHandler");
	}
	else
		logger::error("BTPS: failed to enable InputHandler");
}

void InputHandler::Disable()
{
	auto input = RE::BSInputDeviceManager::GetSingleton();
	if (input)
	{
		input->RemoveEventSink(this);
        IsEnabled = false;

		logger::info("BTPS: successfully disabled InputHandler");
	}
	else
		logger::error("BTPS: failed to disable InputHandler");
}

bool InputHandler::ShouldToggleMouseWheel()
{
	// check against mouse wheel codes
	return Settings::ObjectCycleUpKey == 264 || Settings::ObjectCycleUpKey == 265 ||
		Settings::ObjectCycleDownKey == 264 || Settings::ObjectCycleDownKey == 265;
}

void InputHandler::EnableMouseWheel()
{
	auto controlMap = RE::ControlMap::GetSingleton();

	// if disabled, enable anyways, even if toggle is disabled
	if ( (ShouldToggleMouseWheel() || !IsMouseWheelEnabled) && controlMap)
	{
		Util::EnableControlFlag(RE::ControlMap::UEFlag::kWheelZoom);

		// CRASHES - wait for CLibNG to update RE and replace Util::EnableControlFlag call
		//controlMap->ToggleControls(RE::ControlMap::UEFlag::kWheelZoom, true);

		IsMouseWheelEnabled = true;
	}
}

void InputHandler::DisableMouseWheel()
{
	auto controlMap = RE::ControlMap::GetSingleton();

	if (ShouldToggleMouseWheel() && IsMouseWheelEnabled && controlMap)
	{
		Util::DisableControlFlag(RE::ControlMap::UEFlag::kWheelZoom);
		
		// DOES NOTHING - wait for CLibNG to update RE and replace Util::DisableControlFlag call
		//controlMap->ToggleControls(RE::ControlMap::UEFlag::kWheelZoom, false);

		IsMouseWheelEnabled = false;
	}
}

void InputHandler::SelectIncreasePriority()
{
	auto tickCount = GetTickCount64();

	if (tickCount >= NextIncDecTickCount)
	{
		FocusManager::SelectIncreasePriority();
		NextIncDecTickCount = tickCount + Settings::CycleDelayMS;
	}
}

void InputHandler::SelectDecreasePriority()
{
	auto tickCount = GetTickCount64();

	if (tickCount >= NextIncDecTickCount)
	{
		FocusManager::SelectDecreasePriority();
		NextIncDecTickCount = tickCount + Settings::CycleDelayMS;
	}
}
