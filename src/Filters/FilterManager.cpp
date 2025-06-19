#pragma once
#include "FilterManager.h"
#include "Filter.h"

#include "lib/Util.h"

std::vector<std::shared_ptr<Filter>> FilterManager::Filters;

bool FilterManager::ShouldFilterObj(RE::TESObjectREFR* objectRef)
{
    if (!objectRef) return false;

	auto boundObj = objectRef->GetObjectReference();
    if (!boundObj) return false;

    bool shouldFilter = false;

	for (auto& filter : Filters)
	{
        if (!filter || !filter->IsEnabled)
            continue;

		if (Filter::DoConditionsApply(filter->Conditions, objectRef))
		{
            shouldFilter = filter->ShouldFilterFormType(boundObj->GetFormType(), shouldFilter);
		}
	}

	return shouldFilter;
}

void FilterManager::ClearFilters()
{
    Filters.clear();
}

void FilterManager::AddFilter(std::shared_ptr<Filter> filter)
{
    if (!filter)
    {
        logger::warn("BTPS: FilterManager::AddFilter, attempted to add nullptr");
        return;
    }

    RemoveFilter(filter->FilterName);

	Filters.push_back(filter);
}

bool FilterManager::RemoveFilter(std::string filterName)
{
    Util::ToLowerString(filterName);

	for (int i = 0; i < Filters.size(); i++)
	{
        auto currFilter = Filters[i];
        std::string currFilterName = currFilter->FilterName;
        Util::ToLowerString(currFilterName);

		if (!currFilter || currFilterName == filterName)
        {
            Filters.erase(Filters.begin() + i);
            return true;
		}
	}

	return false;
}

bool FilterManager::SetFilterState(std::string filterName, bool mode)
{
	auto filter = GetFilter(filterName);
	if (!filter)
		return false;

	filter->IsEnabled = mode;
	return true;
}

std::shared_ptr<Filter> FilterManager::GetFilter(std::string filterName)
{
	for (auto filter : Filters)
	{
		if (filter && filter->FilterName == filterName)
			return filter;
	}

	return nullptr;
}
