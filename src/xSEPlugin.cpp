
#define USE_CONSOLE

namespace {

	void InitializeLogging() {

		std::shared_ptr <spdlog::logger> log;

		#ifdef USE_CONSOLE

			Util::Win32::AllocateConsole();

			log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

			log->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

			spdlog::set_default_logger(std::move(log));
			spdlog::set_level(spdlog::level::level_enum::trace);
			spdlog::flush_on(spdlog::level::level_enum::trace);

		#else

			auto path = log::log_directory();

			if (!path) {
				SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory.");
			}

			*path /= PluginDeclaration::GetSingleton()->GetName();
			*path += L".log";


			log = std::make_shared <spdlog::logger>("Global", std::make_shared <spdlog::sinks::basic_file_sink_mt>(path->string(), true));
			log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

			#ifdef DEBUG
				spdlog::set_default_logger(std::move(log));
				spdlog::set_level(spdlog::level::level_enum::debug);
				spdlog::flush_on(spdlog::level::level_enum::debug);
			#else
				spdlog::set_default_logger(std::move(log));
				spdlog::set_level(spdlog::level::level_enum::warn);
				spdlog::flush_on(spdlog::level::level_enum::warn);
			#endif

		#endif
	}

	void InitializeMessaging() {

		if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message* a_Message) {

			switch (a_Message->type) {

				// Called after all plugins have finished running SKSEPluginLoad.
				case MessagingInterface::kPostLoad:{
					logger::debug("kPostLoad");
					break;
				}

				// Called after all kPostLoad message handlers have run.
				case MessagingInterface::kPostPostLoad:{
					logger::debug("kPostPostLoad");
					break;
				}

				// Called when all game data has been found.
				case MessagingInterface::kInputLoaded:{
					logger::debug("kInputLoaded");
					break;
				}

				// All ESM/ESL/ESP plugins have loaded, main menu is now active.
				case MessagingInterface::kDataLoaded:{
					logger::debug("kDataLoaded");
					break;
				}

				// Player's selected save game has finished loading.
				case MessagingInterface::kPostLoadGame:{
					logger::debug("kPostLoadGame");
					break;
				}

				// Player starts a new game from main menu.
				case MessagingInterface::kNewGame:{
					logger::debug("kNewGame");
					break;
				}

				// Player selected a game to load, but it hasn't loaded yet, data will be the name of the loaded save.
				case MessagingInterface::kPreLoadGame:{
					logger::debug("kPreLoadGame");
					break;
				}

				// The player has saved a game.
				case MessagingInterface::kSaveGame:{
					logger::debug("kSaveGame");
					break;
				}

				// The player deleted a saved game from within the load menu, data will be the save name.
				case MessagingInterface::kDeleteGame:{
					logger::debug("kDeleteGame");
					break;
				}

				default:{}
			}
			})) {
			Util::Win32::ReportAndExit("Unable to register message listener.");
		}
	}

	void WaitForDebugger() {
		#ifdef DEBUG
			Util::Win32::ReportInfo("Attach Debugger And Press OK.");
		#endif
	}
}

SKSEPluginLoad(const LoadInterface * a_SKSE) {
	WaitForDebugger();

	Init(a_SKSE);

	InitializeLogging();
	InitializeMessaging();



	logger::info("SKSEPluginLoad OK");

	return true;
}

SKSEPluginInfo(
	.Version = REL::Version { 1, 0, 0, 0 },
	.Name = "TemplatePlugin",
	.Author = "BingusEx",
	.StructCompatibility = SKSE::StructCompatibility::Independent,
	.RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
);