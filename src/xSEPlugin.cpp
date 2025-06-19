#include "UI/DebugAPI.h"

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS
#include "SKSE/API.h"
#include "RevE/Hooks.h"
#include "UI/SelectionWidget.h"
#include "Events.h"
#include "lib/Util.h"
#include "API/Compatibility.h"
#include "API/BTPS_API.h"
#include "API/BTPS_API_decl.h"
#include "Settings.h"
#include "Papyrus.h"
#include "Input/InputHandler.h"



const SKSE::MessagingInterface* g_messaging = nullptr;
const SKSE::LoadInterface* g_LoadInterface = nullptr;

void InitializeLog()
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format("{}.log"sv, Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:

		Hooks::Install();

		SelectionWidgetMenu::Register();
		DebugOverlayMenu::Register();
		   
		MenuOpenCloseEventHandler::Register();

		Compatibility::CheckModulesLoaded(g_LoadInterface);

		if (Compatibility::QuickLootRE::IsInstalled)
			Compatibility::QuickLootRE::RevertHooks();

		Papyrus::Register();

		Settings::ReadSettings();
		Settings::InitObjectOverrides();
		Settings::InitFilterPresets();
		Settings::InitFilterPresetStates();

		InputHandler::GetSingleton()->Enable();

		break;

	case SKSE::MessagingInterface::kNewGame:
		SelectionWidgetMenu::SetEnabled(true);

		DebugOverlayMenu::Load();
		SelectionWidgetMenu::Load();

		break;

	case SKSE::MessagingInterface::kPreLoadGame:
		SelectionWidgetMenu::SetEnabled(true);

		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		DebugOverlayMenu::Load();
		SelectionWidgetMenu::Load();

		break;

	//case SKSE::MessagingInterface::kPostLoad:
		//break;

	//case SKSE::MessagingInterface::kPostPostLoad:
		//break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "BetterThirdPersonSelection";
	a_info->version = REL::Version{ 0,8,7,0 }.pack();

	return true;
}

SKSEPluginLoad(SKSE::LoadInterface* a_skse)
{
	g_LoadInterface = a_skse;

// #ifndef NDEBUG
//	while (!IsDebuggerPresent()) {};
// #endif

	InitializeLog();
	//logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	if (!g_messaging)
	{
		logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}

	auto papyrus = reinterpret_cast<SKSE::PapyrusInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kPapyrus));
	if (!papyrus)
	{
		logger::critical("Failed to load scripting interface! This error is fatal, plugin will not load.");
		return false;
	}

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 7);

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
	v.PluginVersion(REL::Version{ 0,8,7,0 });
	v.PluginName("BetterThirdPersonSelection");
	v.UsesAddressLibrary();
	v.UsesNoStructs();

    return v;
}();

extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI(const BTPS_API_decl::Version a_interfaceVersion)
{
    auto api = BTPS_API::GetSingleton();

    logger::info("BTPS: API is being requested, version {}", magic_enum::enum_integer<BTPS_API_decl::Version>(a_interfaceVersion));

    switch (a_interfaceVersion)
    {
    case BTPS_API_decl::Version::V0:
        logger::info("BTPS: API request successful");
        return static_cast<void*>(api);
    }

    logger::info("BTPS: API request called with invalid version");
    return nullptr;
}
