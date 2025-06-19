#pragma once

class Filter;

namespace Papyrus
{
	void Register();

	class BTPS_MCM
	{
	public:
		static bool Register(RE::BSScript::IVirtualMachine* a_vm);

		static void ClearFilters();
		static void AddFilter(Filter* a_filter);
		static void InitFilters();
		static void SetFilterState(std::string filterName, bool mode);
		static void SerializeFilterStates();

		static bool GetQuestHandle(RE::BSScript::IVirtualMachine* a_vm, RE::VMHandle& handleOut);

		static void OnConfigClose(RE::TESQuest* quest);
		static void OnConfigOpen(RE::TESQuest* quest);
		static void OnPageReset(RE::TESQuest* quest);
	};

	// by PowerOf3
	bool IsScriptAttached(const  RE::BSScript::Internal::VirtualMachine& a_vm, const RE::VMHandle a_handle, const RE::BSFixedString& a_scriptName);
	bool IsScriptAttached(const RE::TESForm* a_form, const RE::BSFixedString& a_scriptName);
}
