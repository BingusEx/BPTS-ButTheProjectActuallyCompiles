#pragma once

#include "Hooks.h"
#include "NearbyActorScanner.h"
#include "FocusManager.h"
#include "Settings.h"
#include "Offsets.h"
#include "Filters/FilterManager.h"

#include <UI/SelectionWidget.h>

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Install()
	{
		logger::info("BTPS applying hooks...");

		ActivateHook::Hook();
		SelectionHook::Hook();
		//MainUpdateHook::Hook();
		HorseBackHook::Hook();
		UpdateCrosshairTextHook::Hook();

		logger::info("BTPS finished applying hooks...");
	}

	void MainUpdateHook::Hook()
	{
		/*logger::info("BTPS applying MainUpdateHook");

		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ RELOCATION_ID(35551, 36544), REL::VariantOffset(0x11F, 0x160, 0x160) };  // main loop
		_Update = trampoline.write_call<5>(hook.address(), Update);*/
	}

	void MainUpdateHook::Update(RE::Main* a_this, float a2)
	{
		/*_Update(a_this, a2);

		Settings::UpdateSettings();*/
	}

	RE::TESObjectREFR* SelectionHook::OnSetCrosshairTargetRef(RE::ObjectRefHandle objectRefHandle)
	{
		Settings::UpdateSettings();

		auto crosshairPick = RE::CrosshairPickData::GetSingleton();
		if (!crosshairPick)
			return nullptr;

		RE::TESObjectREFR* objectRef = nullptr;
		if (objectRefHandle && objectRefHandle.get())
			objectRef = objectRefHandle.get().get();

		if (FocusManager::GetIsEnabled())
		{
			auto focusObj = FocusManager::GetCustomFocusObject(objectRef);
			FocusManager::SetFocusObject(focusObj);

			if (focusObj.get())
                return focusObj->ObjectRef.get().get();
			return nullptr;
		}
		else
		{
			if (Settings::EnableFiltersForNativeSelection && FilterManager::ShouldFilterObj(objectRef))
				objectRef = nullptr;

			// filter out native object selection for some objects on horseback - eg. furniture
			// because activating furniture from horseback breaks things
			if (objectRef && !NearbyActorScanner::HasBlockingFlagsHorseback(*objectRef) )
			{
				FocusManager::SetFocusObject(objectRef);
				SelectionWidget::Update();
				return objectRef;
			}
			else
			{
				FocusManager::SetFocusObject(nullptr);
				SelectionWidget::Update();
				return nullptr;
			}
		}
	}

	void SelectionHook::Hook()
	{
		logger::info("BTPS applying SelectionHook");

		struct Patch : Xbyak::CodeGenerator
		{
			Patch(REL::Relocation<std::uintptr_t> jmpBackAddress)
			{
				// move objectRef into register
				//mov(ptr[r15 + 4], eax);

				//BingusEX;
				//rcx is 64 bit 32 -> 64 bit mov throws ERR_BAD_SIZE_OF_REGISTER
				//mov(rcx, eax);
				mov(ecx, eax);

				// call OnSetCrosshairTargetRef
				mov(rax, reinterpret_cast<size_t>(&OnSetCrosshairTargetRef));
				call(rax);

				// move return value into rbx
				mov(rbx, rax);

				// mov from original asm that is overridden as well
				mov(rcx, rbx);

				// jump back
				mov(r10, jmpBackAddress.address());
				jmp(r10);
			}
		};

		// SE: 3AA4B0 - CrossHairPickData::Pick
		// SE: 0xCF0  - mov in the function where the new focus is applied
		// AE: 3C0FA0 - CrossHairPickData::Pick
		// AE: 0xDA8  - mov in the function where the new focus is applied
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(25591, 26127), REL::VariantOffset( 0xCF0, 0xE38, 0xE38) };
		REL::Relocation<std::uintptr_t> jmpBackAddress{ RELOCATION_ID(25591, 26127), REL::VariantOffset(0xCF7, 0xE3F, 0xE3F) }; // this is right after the changed assembly
		Patch code(jmpBackAddress);
		code.ready();

		auto& trampoline = SKSE::GetTrampoline();

		trampoline.write_branch<6>(target.address(),trampoline.allocate(code));
	}

	// form types that can be activated using native Skyrim object picker
	bool SelectionHook::IsValidFormType(RE::FormType t)
	{
		return
			t == RE::FormType::Activator ||
			t == RE::FormType::ActorCharacter ||
			t == RE::FormType::NPC ||
			t == RE::FormType::Door ||
			t == RE::FormType::Furniture ||
			t == RE::FormType::TalkingActivator ||
			t == RE::FormType::Apparatus;
	}

	void ActivateHook::Actor_Dismount(RE::Actor* a_this)
	{
		// if hold to dismount is disabled, allow native dismount behaviour
		if (!Settings::IsHoldToDismountEnabled)
		{
			auto dismount = REL::Relocation<Offsets::DismountActor_func> { Offsets::DismountActor };
			dismount(a_this);
		}
	}

	void ActivateHook::Hook()
	{
		logger::info("BTPS applying ActivateHook");

		auto& trampoline = SKSE::GetTrampoline();

 		REL::Relocation<std::uintptr_t> ActivateHandlerVtbl{ RE::VTABLE_ActivateHandler[0] };
 		_ProcessButton = ActivateHandlerVtbl.write_vfunc(0x4, ProcessButton);

		// SE: 601a00 / 36840 - function where the dismount for activation key happens
		// SE: 0xE2 - offset to the call to Actor::Dismount
		// AE: ????
		REL::Relocation<std::uintptr_t> dismountHookTarget{ RELOCATION_ID(36840, 37864), 0xE2 };

		trampoline.write_call<5>(dismountHookTarget.address(), Actor_Dismount);
	}

	void ActivateHook::ProcessButton(RE::ActivateHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto playerChar = RE::PlayerCharacter::GetSingleton();

		// if player is on mount, override activation mechanics
        if (Settings::IsHoldToDismountEnabled && playerChar && playerChar->IsOnMount() && 
			a_event->heldDownSecs >= Settings::HoldToDismountMinTime)
 		{
			auto menu = RE::UI::GetSingleton()->GetMenu(SelectionWidgetMenu::MENU_NAME);

			if (a_event->IsHeld())
			{
                if (a_event->heldDownSecs - Settings::HoldToDismountMinTime >= Settings::HoldToDismountTime)
				{
					a_this->heldButtonActionSuccess = true;

					auto dismount = REL::Relocation<Offsets::DismountActor_func> { Offsets::DismountActor };
					dismount(playerChar);

					SelectionWidget::SetProgressCirlceValue(0.0);
				}
				else
				{
                    auto value = (a_event->heldDownSecs - Settings::HoldToDismountMinTime) / Settings::HoldToDismountTime;
					SelectionWidget::SetProgressCirlceValue((float)value);
				}
			}
			else if(a_event->IsUp())
			{
				SelectionWidget::SetProgressCirlceValue(0.0);
			}
		}
		else
			SelectionWidget::SetProgressCirlceValue(0.0);

		_ProcessButton(a_this, a_event, a_data);
	}

	void UpdateCrosshairTextHook::Hook()
	{
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(39535, 40621), REL::VariantOffset(0x289, 0x280, 0x280) };
		_OnUpdateCrosshairText = trampoline.write_call<5>(target.address(), OnUpdateCrosshairText);

		// pretty sure this doesn't work. Seems to trigger only on non-AutoLoadDoor text changes, same as OnUpdateCrosshairText
		//REL::Relocation<std::uintptr_t> target_autoLoadDoor{ RELOCATION_ID(50727, 51622), REL::VariantOffset(0xD7, 0x22B, 0x22B) };
		//_OnUpdateCrosshairText_AutoLoadDoor = trampoline.write_call<5>(target.address(), OnUpdateCrosshairText_AutoLoadDoor);
	}

	void UpdateCrosshairTextHook::OnUpdateCrosshairText(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
	{
		_OnUpdateCrosshairText(a_this, a_menuName, a_type, a_data);

		SelectionWidget::OnUpdateCrosshairText();
		//SelectionWidgetMenu::Show("SetNewFocusObject");
	}

	void UpdateCrosshairTextHook::OnUpdateCrosshairText_AutoLoadDoor(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
	{
		_OnUpdateCrosshairText_AutoLoadDoor(a_this, a_menuName, a_type, a_data);

		/*SelectionWidget::OnUpdateCrosshairText();
		SelectionWidgetMenu::Show("SetNewFocusObject");*/
	}

	void HorseBackHook::Hook()
	{
		logger::info("BTPS applying HorseBackHook");

		struct Patch_IsOnMount : Xbyak::CodeGenerator
		{
			Patch_IsOnMount(REL::Relocation<std::uintptr_t> jmpBackAddress)
			{
				// jump back
				mov(r10, jmpBackAddress.address());
				jmp(r10);
			}
		};

		auto& trampoline = SKSE::GetTrampoline();

		// SE: 0x6b0570		- PlayerCharacter::sub_1406B0570
		// SE: 0xAA			- call    Actor__IsOnMount_14022F320
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(39535, 40621), 0xAA };
		REL::Relocation<std::uintptr_t> jmpBackAddress{ RELOCATION_ID(39535, 40621), 0xB7 }; // this is right after the changed assembly

		REL::Relocation<std::uintptr_t> HUD_Fix{ RELOCATION_ID(50718, 51612), REL::VariantOffset(0x1209, 0x12BC, 0x12BC) }; // 87d580

		Patch_IsOnMount code(jmpBackAddress);
		code.ready();

		trampoline.write_branch<6>(target.address(),
			trampoline.allocate(code)
			);

		// override a call to IsOnMount with a function that always returns false
		//trampoline.write_call<5>(HUD_Fix.address(), FalseFunc);
	}
}
