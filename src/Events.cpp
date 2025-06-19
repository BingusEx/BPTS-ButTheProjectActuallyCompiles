#pragma once
#include "Events.h"
#include "UI/DebugAPI.h"
#include "UI/SelectionWidget.h"

MenuOpenCloseEventHandler* MenuOpenCloseEventHandler::GetSingleton()
{
	static MenuOpenCloseEventHandler singleton;
	return std::addressof(singleton);
}

void MenuOpenCloseEventHandler::Register()
{
	auto ui = RE::UI::GetSingleton();
	if (!ui)
	{
		logger::error("BTPS: failed to register MenuOpenCloseEventHandler");
		return;
	}

	ui->AddEventSink(GetSingleton());

	logger::info("BTPS: successfully registered MenuOpenCloseEventHandler");
}

RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
{
    RE::BSFixedString mName = a_event->menuName;

	// for some reason, after each cell change, the menu is hidden and needs to be shown again
	// using a UI message kShow
	if (mName == RE::LoadingMenu::MENU_NAME && !a_event->opening)
	{
		DebugOverlayMenu::Load();
		SelectionWidgetMenu::Load();

		SelectionWidget::InitUIValues();

		// just to make sure, because some people use coc from the main menu, which
		// would get around the kNewGame and kPreLoadGame events
		SelectionWidgetMenu::SetEnabled(true);
	}

	if (mName == RE::JournalMenu::MENU_NAME ||
		mName == RE::InventoryMenu::MENU_NAME ||
		mName == RE::MapMenu::MENU_NAME ||
		mName == RE::BookMenu::MENU_NAME ||
		mName == RE::LockpickingMenu::MENU_NAME ||
		mName == RE::MagicMenu::MENU_NAME ||
		mName == RE::RaceSexMenu::MENU_NAME ||
		mName == RE::CraftingMenu::MENU_NAME ||
		mName == RE::SleepWaitMenu::MENU_NAME ||
		mName == RE::TrainingMenu::MENU_NAME ||
		mName == RE::BarterMenu::MENU_NAME ||
		mName == RE::FavoritesMenu::MENU_NAME ||
		mName == RE::GiftMenu::MENU_NAME ||
		mName == RE::StatsMenu::MENU_NAME ||
		mName == RE::ContainerMenu::MENU_NAME ||
		mName == RE::DialogueMenu::MENU_NAME ||
		mName == RE::MessageBoxMenu::MENU_NAME ||
		mName == RE::TweenMenu::MENU_NAME || // tab menu
		mName == RE::MainMenu::MENU_NAME ||
		mName == "CustomMenu") // papyrus custom menues go here
	{
		if (a_event->opening)
		{
			DebugOverlayMenu::Hide(mName.c_str());
			SelectionWidgetMenu::Hide(mName.c_str());
		}
		else
		{
			DebugOverlayMenu::Show(mName.c_str());
			SelectionWidgetMenu::Show(mName.c_str());
		}
	}

	return RE::BSEventNotifyControl::kContinue;
}
