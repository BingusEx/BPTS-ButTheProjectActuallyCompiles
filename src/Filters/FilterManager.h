#include <vector>
#include <string>

class Filter;

class FilterManager
{
public:
    static std::vector<std::shared_ptr<Filter>> Filters;

	static bool ShouldFilterObj(RE::TESObjectREFR* objectRef);

	static void ClearFilters();
    static void AddFilter(std::shared_ptr<Filter> filter);
	static bool RemoveFilter(std::string filterName);

	static bool SetFilterState(std::string filterName, bool mode);
	static std::shared_ptr<Filter> GetFilter(std::string filterName);
};
