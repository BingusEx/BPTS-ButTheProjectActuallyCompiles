#pragma once
#include "Papyrus.h"
#include "Settings.h"
#include "Filters/FilterManager.h"
#include "Filters/FilterParam.h"

void Papyrus::Register()
{
	auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(BTPS_MCM::Register);

	logger::info("BTPS: successfully registered Papyrus data");
}

// lifted from PowerOf3's script extender
bool Papyrus::IsScriptAttached(const RE::BSScript::Internal::VirtualMachine& a_vm, const RE::VMHandle a_handle, const RE::BSFixedString& a_scriptName)
{
	return std::ranges::any_of(a_vm.attachedScripts, [&](const auto& attachedScript)
	{
		const auto& [handle, scripts] = attachedScript;
		if (handle == a_handle)
		{
			RE::BSScript::ObjectTypeInfo* typeInfo = nullptr;

			return std::ranges::any_of(scripts, [&](const auto& script)
			{
				typeInfo = script ? script->GetTypeInfo() : nullptr;

				return (typeInfo && typeInfo->name == a_scriptName);
			});
		}

		return false;
	});
}

// lifted from PowerOf3's script extender
bool Papyrus::IsScriptAttached(const RE::TESForm* a_form, const RE::BSFixedString& a_scriptName)
{
	const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	if (!vm)
		return false;

	const auto policy = vm->GetObjectHandlePolicy();
	if (!policy)
		return false;

	const auto handle = policy->GetHandleForObject(static_cast<RE::VMTypeID>(a_form->GetFormType()), a_form);
	if (handle == policy->EmptyHandle())
		return false;

	return IsScriptAttached(*vm, handle, a_scriptName);
}

void Papyrus::BTPS_MCM::OnConfigClose(RE::TESQuest* quest)
{
	SerializeFilterStates();

	Settings::ReadSettings();
	Settings::UpdateSettings();

	if (Settings::HotReloadConfig)
    {
        Settings::InitObjectOverrides();
        Settings::InitFilterPresets();
    }

	Settings::InitFilterPresetStates();
}

void Papyrus::BTPS_MCM::OnConfigOpen(RE::TESQuest * quest)
{
	InitFilters();
}

void Papyrus::BTPS_MCM::OnPageReset(RE::TESQuest * quest)
{
	InitFilters();
}

bool Papyrus::BTPS_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
{
	a_vm->RegisterFunction("OnConfigClose_CLib", "BetterThirdPersonSelection_MCM", OnConfigClose);
	a_vm->RegisterFunction("OnConfigOpen", "BetterThirdPersonSelection_MCM", OnConfigOpen);
	a_vm->RegisterFunction("OnPageReset_CLib", "BetterThirdPersonSelection_MCM", OnPageReset);

	logger::info("BTPS: Registered BetterThirdPersonSelection_MCM class");
	return true;
}

void Papyrus::BTPS_MCM::ClearFilters()
{
	auto papyrusVM = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	RE::VMHandle questHandle;
	if (!GetQuestHandle(papyrusVM, questHandle))
	{
		logger::warn("BTPS: Papyrus ClearFilters failed - failed to get quest VM handle");
		return;
	}

	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> funcReturn;

	auto args = RE::MakeFunctionArguments();
	papyrusVM->DispatchMethodCall2(questHandle, "BetterThirdPersonSelection_MCM", "ClearFilters", args, funcReturn);
}

void Papyrus::BTPS_MCM::AddFilter(Filter* a_filter)
{
	if (!a_filter)
		return;

	auto papyrusVM = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	RE::VMHandle questHandle;
	if (!GetQuestHandle(papyrusVM, questHandle))
	{
		logger::warn("BTPS: Papyrus AddFilter failed - failed to get quest VM handle");
		return;
	}

	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> funcReturn;

	auto args = RE::MakeFunctionArguments(a_filter->FilterName.c_str(), a_filter->Description.c_str(), (bool)a_filter->IsEnabled);
	papyrusVM->DispatchMethodCall2(questHandle, "BetterThirdPersonSelection_MCM", "AddFilter", args, funcReturn);
}

void Papyrus::BTPS_MCM::InitFilters()
{
	ClearFilters();

	for (auto filter : FilterManager::Filters)
	{
		if (!filter)
			continue;

		AddFilter(filter.get());
	}
}

void Papyrus::BTPS_MCM::SetFilterState(std::string filterName, bool mode)
{
	/*auto papyrusVM = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	RE::VMHandle questHandle;
	if (!GetQuestHandle(papyrusVM, questHandle))
	{
		logger::warn("BTPS: Papyrus SetFilterState failed - failed to get quest VM handle");
		return;
	}

	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> funcReturn;

	auto args = RE::MakeFunctionArguments(filterName.c_str(), (bool)mode);
	papyrusVM->DispatchMethodCall2(questHandle, "BetterThirdPersonSelection_MCM", "SetFilterState", args, funcReturn);*/
}

void Papyrus::BTPS_MCM::SerializeFilterStates()
{
	auto papyrusVM = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	RE::VMHandle questHandle;
	if (!GetQuestHandle(papyrusVM, questHandle))
	{
		logger::warn("BTPS: Papyrus SerializeFilterStates failed - failed to get quest VM handle");
		return;
	}

	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> funcReturn;

	auto args = RE::MakeFunctionArguments();
	papyrusVM->DispatchMethodCall2(questHandle, "BetterThirdPersonSelection_MCM", "WriteFilterStatesToFile", args, funcReturn);
}

bool Papyrus::BTPS_MCM::GetQuestHandle(RE::BSScript::IVirtualMachine* a_vm, RE::VMHandle& handleOut)
{
	auto handlePolicy = a_vm->GetObjectHandlePolicy();
	if (!handlePolicy)
	{
		logger::error("BTPS: failed to call MCM Papyrus function - no VM handle policy");
		return false;
	}

	auto mcmQuest = RE::TESForm::LookupByEditorID("BetterThirdPersonSelection_MCM");
	if (!mcmQuest)
	{
		logger::error("BTPS: failed to call MCM Papyrus function - couldn't find quest form 'BetterThirdPersonSelection_MCM'");
		return false;
	}

	handleOut = handlePolicy->GetHandleForObject(static_cast<RE::VMTypeID>(mcmQuest->GetFormType()), mcmQuest);
	if (!handleOut)
	{
		logger::error("BTPS: failed to call MCM Papyrus function - couldn't get MCM quest form handle");
		return false;
	}

	return true;
}
