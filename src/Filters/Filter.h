#pragma once

#include "FilterFormType.h"
#include <vector>
#include "magic_enum/magic_enum.hpp"
#include "FilterParam.h"

enum class ECondition
{
	NONE,
	// PLAYER
	IsSneaking,
	IsInThirdPerson,
	IsInFirstPerson,
	IsInCombat,
	IsWeaponDrawn,
	IsOnHorseback,
	IsSwimming,
    IsSprinting,
	IsFlying,
	IsSitting,
	IsDetected,
	IsInBeastForm,
	HasPlayerMagicEffect,
	HasPlayerKeyword,
	HasPlayerRaceKeyword,
	IsPlayerInFaction,

	// GENERAL
	IsKeyDown,
    IsKeyToggled,
	IsUsingBTPSSelection,
	IsUsingNativeSelection,

	// OBJECT
	IsBaseID,
	HasKeyword,
	HasMagicEffect,
	IsScriptAttached,
	IsCrimeToActivate,
	IsDisplayName,
	IsHorse,
	IsCrosshairSelection,

	IsValuePerWeightEqual,
	IsValuePerWeightMore,
	IsValuePerWeightLess,

	IsValueEqual,
	IsValueMore,
	IsValueLess,

	IsWeightEqual,
	IsWeightMore,
    IsWeightLess,

	// BOOKS
	IsBook,
	IsBookRead,
	IsSkillBook,
	IsSpellBook,

	// ACTOR
	IsActor,
	IsDead,
	IsInBleedOut,
	IsUnconscious,
	IsRestrained,
	IsReanimated,
	IsEssential,
	IsInFaction,
	HasRaceKeyword,

	// CONTAINER
	IsEmptyContainer,

	// LOCATION
	LocationHasKeyword,
};

enum class EConditionConnector
{
    OR,
	AND,
	NONE,
};

class Filter;

class ConnectedCondition
{
public:
    ConnectedCondition(std::shared_ptr<Filter> parent, ECondition condition, EConditionConnector connector, bool inverse, std::vector<std::string> parameters);

	bool GetParam(int idx, int& valueOut);
	bool GetParam(int idx, float& valueOut);
	bool GetParam(int idx, bool& valueOut);
	bool GetParam(int idx, std::string& valueOut);

	std::shared_ptr<Filter> Parent;

	std::string GetParentName();

	bool Inverse;
    ECondition Condition;
    EConditionConnector Connector; // applies to this + next condition
    std::vector<FilterParam> Parameters;
};

class Filter
{
public:
    static std::unordered_map<std::string, ECondition> ConditionMap;

    std::string FilterName;
    std::string Description;
    std::vector<ConnectedCondition> Conditions;
    std::vector<FilterFormType> FormTypes;

	bool IsEnabled = false;

	bool ShouldFilterFormType(RE::FormType formType, bool shouldFilterPrevious);

	static bool DoesConditionApply(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoConditionsApply(std::vector<ConnectedCondition>& conditions, RE::TESObjectREFR* objectRef);

    static bool DoesConditionApply_IsDetected(ConnectedCondition& condition);
    static bool DoesConditionApply_IsInBeastForm(ConnectedCondition& condition);
    static bool DoesConditionApply_HasPlayerMagicEffect(ConnectedCondition& condition);
    static bool DoesConditionApply_HasPlayerKeyword(ConnectedCondition& condition);
    static bool DoesConditionApply_HasPlayerRaceKeyword(ConnectedCondition& condition);
    static bool DoesConditionApply_IsPlayerInFaction(ConnectedCondition& condition);

	static bool DoesConditionApply_IsKeyDown(ConnectedCondition& condition);
    static bool DoesConditionApply_IsKeyToggled(ConnectedCondition& condition);
    static bool DoesConditionApply_IsDisplayName(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsBaseID(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_HasKeyword(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_HasRaceKeyword(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_HasMagicEffect(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsScriptAttached(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

    static bool DoesConditionApply_IsActor(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsDead(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsInBleedOut(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsUnconscious(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsRestrained(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsReanimated(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsEssential(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsInFaction(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

	static bool DoesConditionApply_IsValuePerWeightEqual(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsValuePerWeightMore(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsValuePerWeightLess(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

    static bool DoesConditionApply_IsValueEqual(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsValueMore(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsValueLess(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

    static bool DoesConditionApply_IsWeightEqual(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsWeightMore(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
    static bool DoesConditionApply_IsWeightLess(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

    static bool DoesConditionApply_IsEmptyContainer(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);

	static bool DoesConditionApply_IsBook(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref);
	static bool DoesConditionApply_IsSpellBook(ConnectedCondition& condition, RE::TESObjectREFR* objectRef);
	static bool DoesConditionApply_IsSkillBook(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref);
	static bool DoesConditionApply_IsBookRead(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref);

    static bool DoesConditionApply_LocationHasKeyword(ConnectedCondition& condition);

	static ECondition MakeCondition(std::string conditionName, bool& inverseOut);
	static EConditionConnector MakeConditionConnector(std::string conditionConnectorName);
};
