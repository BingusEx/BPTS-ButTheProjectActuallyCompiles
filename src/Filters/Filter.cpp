#include "Filter.h"
#include "Input/InputHandler.h"
#include "FocusManager.h"
#include "Papyrus.h"
#include "API/Compatibility.h"

#include "lib/Util.h"

bool Filter::DoesConditionApply(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
    auto playerChar = RE::PlayerCharacter::GetSingleton();
    if (!playerChar || !objectRef)
        return false;

	bool conditionResult = false;

	switch (condition.Condition)
	{
    case ECondition::NONE:
		conditionResult = true;
        break;
	case ECondition::IsSneaking:
		conditionResult = playerChar->IsSneaking();
        break;
	case ECondition::IsInThirdPerson:
		conditionResult = !Util::IsPlayerInFirstPerson();
        break;
	case ECondition::IsInFirstPerson:
		conditionResult = Util::IsPlayerInFirstPerson();
        break;
	case ECondition::IsInCombat:
        conditionResult = playerChar->IsInCombat();
        break;
	case ECondition::IsWeaponDrawn:
        conditionResult = playerChar->AsActorState()->IsWeaponDrawn();
        break;
	case ECondition::IsOnHorseback:
        conditionResult = Util::GetPlayerMountRef();
        break;
	case ECondition::IsCrimeToActivate:
		conditionResult = objectRef->IsCrimeToActivate();
        break;
	case ECondition::IsKeyDown:
        conditionResult = DoesConditionApply_IsKeyDown(condition);
        break;
	case ECondition::IsKeyToggled:
        conditionResult = DoesConditionApply_IsKeyToggled(condition);
        break;
	case ECondition::IsDisplayName:
        conditionResult = DoesConditionApply_IsDisplayName(condition, objectRef);
        break;
	case ECondition::IsSwimming:
        conditionResult = playerChar->AsActorState()->IsSwimming();
        break;
	case ECondition::IsSprinting:
        conditionResult = playerChar->AsActorState()->IsSprinting();
        break;
	case ECondition::IsFlying:
        conditionResult = playerChar->AsActorState()->IsFlying();
        break;
	case ECondition::IsSitting:
        conditionResult = playerChar->AsActorState()->GetSitSleepState() == RE::SIT_SLEEP_STATE::kIsSitting;
        break;
	case ECondition::IsBaseID:
        conditionResult = DoesConditionApply_IsBaseID(condition, objectRef);
        break;
	case ECondition::IsValuePerWeightEqual:
		conditionResult = DoesConditionApply_IsValuePerWeightEqual(condition, objectRef);
        break;
	case ECondition::IsValuePerWeightMore:
		conditionResult = DoesConditionApply_IsValuePerWeightMore(condition, objectRef);
        break;
	case ECondition::IsValuePerWeightLess:
		conditionResult = DoesConditionApply_IsValuePerWeightLess(condition, objectRef);
        break;
	case ECondition::IsValueEqual:
		conditionResult = DoesConditionApply_IsValueEqual(condition, objectRef);
        break;
	case ECondition::IsValueMore:
		conditionResult = DoesConditionApply_IsValueMore(condition, objectRef);
        break;
	case ECondition::IsValueLess:
		conditionResult = DoesConditionApply_IsValueLess(condition, objectRef);
        break;
	case ECondition::IsWeightEqual:
		conditionResult = DoesConditionApply_IsWeightEqual(condition, objectRef);
        break;
	case ECondition::IsWeightMore:
		conditionResult = DoesConditionApply_IsWeightMore(condition, objectRef);
        break;
	case ECondition::IsWeightLess:
		conditionResult = DoesConditionApply_IsWeightLess(condition, objectRef);
        break;
	case ECondition::HasKeyword:
		conditionResult = DoesConditionApply_HasKeyword(condition, objectRef);
		break;
    case ECondition::HasRaceKeyword:
		conditionResult = DoesConditionApply_HasRaceKeyword(condition, objectRef);
		break;
	case ECondition::HasMagicEffect:
		conditionResult = DoesConditionApply_HasMagicEffect(condition, objectRef);
		break;
	case ECondition::IsScriptAttached:
		conditionResult = DoesConditionApply_IsScriptAttached(condition, objectRef);
		break;
	case ECondition::IsDetected:
		conditionResult = DoesConditionApply_IsDetected(condition);
		break;
	case ECondition::IsHorse:
		conditionResult = objectRef->IsHorse();
		break;
	case ECondition::IsCrosshairSelection:
		conditionResult = objectRef == FocusManager::FocusRef_Orig;
		break;
	case ECondition::IsActor:
		conditionResult = DoesConditionApply_IsActor(condition, objectRef);
		break;
	case ECondition::IsDead:
		conditionResult = DoesConditionApply_IsDead(condition, objectRef);
		break;
	case ECondition::IsInBleedOut:
		conditionResult = DoesConditionApply_IsInBleedOut(condition, objectRef);
		break;
	case ECondition::IsUsingBTPSSelection:
		conditionResult = FocusManager::GetIsEnabled();
		break;
	case ECondition::IsUsingNativeSelection:
		conditionResult = !FocusManager::GetIsEnabled();
		break;
	case ECondition::IsInBeastForm:
		conditionResult = DoesConditionApply_IsInBeastForm(condition);
		break;
	case ECondition::HasPlayerMagicEffect:
		conditionResult = DoesConditionApply_HasPlayerMagicEffect(condition);
		break;
    case ECondition::HasPlayerKeyword:
		conditionResult = DoesConditionApply_HasPlayerKeyword(condition);
		break;
	case ECondition::HasPlayerRaceKeyword:
		conditionResult = DoesConditionApply_HasPlayerRaceKeyword(condition);
		break;
    case ECondition::IsPlayerInFaction:
		conditionResult = DoesConditionApply_IsPlayerInFaction(condition);
		break;
	case ECondition::IsBook:
		conditionResult = DoesConditionApply_IsBook(condition, objectRef);
		break;
	case ECondition::IsBookRead:
		conditionResult = DoesConditionApply_IsBookRead(condition, objectRef);
		break;
	case ECondition::IsSkillBook:
		conditionResult = DoesConditionApply_IsSkillBook(condition, objectRef);
		break;
	case ECondition::IsSpellBook:
		conditionResult = DoesConditionApply_IsSpellBook(condition, objectRef);
		break;
	case ECondition::IsEmptyContainer:
		conditionResult = DoesConditionApply_IsEmptyContainer(condition, objectRef);
		break;
	case ECondition::LocationHasKeyword:
		conditionResult = DoesConditionApply_LocationHasKeyword(condition);
		break;
    case ECondition::IsUnconscious:
		conditionResult = DoesConditionApply_IsUnconscious(condition, objectRef);
		break;
    case ECondition::IsRestrained:
		conditionResult = DoesConditionApply_IsRestrained(condition, objectRef);
		break;
    case ECondition::IsReanimated:
		conditionResult = DoesConditionApply_IsReanimated(condition, objectRef);
		break;
    case ECondition::IsEssential:
		conditionResult = DoesConditionApply_IsEssential(condition, objectRef);
		break;
    case ECondition::IsInFaction:
		conditionResult = DoesConditionApply_IsInFaction(condition, objectRef);
		break;
    }

	if (condition.Inverse)
        conditionResult = !conditionResult;

	return conditionResult;
}

bool Filter::DoConditionsApply(std::vector<ConnectedCondition>& conditions, RE::TESObjectREFR* objectRef)
{
    bool conditionResult = true;
    EConditionConnector LastConnector = EConditionConnector::NONE;

	for (int i = 0; i < conditions.size(); i++)
    {
        ConnectedCondition& condition = conditions[i];

		switch (LastConnector)
		{
        case EConditionConnector::NONE:
            conditionResult = DoesConditionApply(condition, objectRef);
            break;

        case EConditionConnector::OR:
            conditionResult |= DoesConditionApply(condition, objectRef);
			break;

		case EConditionConnector::AND:
            conditionResult &= DoesConditionApply(condition, objectRef);
			break;
		}

		LastConnector = condition.Connector;
	}

    return conditionResult;
}

// taken from PowerOf3 script extender
bool Filter::DoesConditionApply_IsDetected(ConnectedCondition& condition)
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();

	if (playerChar->GetActorRuntimeData().currentProcess)
	{
		const auto processLists = RE::ProcessLists::GetSingleton();
		if (processLists)
		{
			for (auto& targetHandle : processLists->highActorHandles)
			{
                if (RE::NiPointer<RE::Actor> target = targetHandle.get())
				{
					auto crimeFaction = target->GetCrimeFaction();
					if (crimeFaction && 
						!crimeFaction->IgnoresStealing() && 
						crimeFaction->crimeData.crimevalues.stealCrimeGoldMult > 0.0f && 
						target->GetActorRuntimeData().currentProcess)
					{
						int detectionLevel = target->RequestDetectionLevel(playerChar);

						if (detectionLevel > 0)
							return true;
					}
				}
			}
		}
	}

	return false;
}

bool Filter::DoesConditionApply_IsKeyDown(ConnectedCondition& condition)
{
    int keyCode = -1;
	if (!condition.GetParam(0, keyCode))
	{
		logger::warn("BTPS: filterPreset failed on IsKeyDown, invalid parameter at idx 0");
        return false;
	}

    if (keyCode < 0)
        return false;

    auto inputHandler = InputHandler::GetSingleton();
    if (!inputHandler)
        return false;

    return inputHandler->IsKeyPressed(keyCode);
}

bool Filter::DoesConditionApply_IsKeyToggled(ConnectedCondition& condition)
{
    int keyCode = -1;
	if (!condition.GetParam(0, keyCode))
	{
		logger::warn("BTPS: filterPreset failed on IsKeyToggled, invalid parameter at idx 0");
        return false;
	}

    if (keyCode < 0)
        return false;

    auto inputHandler = InputHandler::GetSingleton();
    if (!inputHandler)
        return false;

    return inputHandler->IsKeyToggled(keyCode);
}

bool Filter::DoesConditionApply_IsDisplayName(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
    if (condition.Parameters.empty())
        return false;

	std::string displayName = objectRef->GetDisplayFullName();
    Util::ToLowerString(displayName);

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string currDisplayName;
		if (!condition.GetParam(i, currDisplayName))
			continue;

        Util::ToLowerString(currDisplayName);

		if (currDisplayName == displayName)
            return true;
	}

    return false;
}

bool Filter::DoesConditionApply_IsBaseID(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	auto formID = boundObj->GetFormID();

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		int baseID;
		if (!condition.GetParam(i, baseID))
			continue;

		if (formID == (unsigned int)baseID)
			return true;
	}

	return false;
}

bool Filter::DoesConditionApply_IsInBeastForm(ConnectedCondition & condition)
{
	auto menuCtrls = RE::MenuControls::GetSingleton();
	return menuCtrls && menuCtrls->InBeastForm();
}

bool Filter::DoesConditionApply_HasPlayerMagicEffect(ConnectedCondition& condition)
{
	return DoesConditionApply_HasMagicEffect(condition, RE::PlayerCharacter::GetSingleton());
}

bool Filter::DoesConditionApply_HasPlayerKeyword(ConnectedCondition& condition)
{
	return DoesConditionApply_HasKeyword(condition, RE::PlayerCharacter::GetSingleton());
}

bool Filter::DoesConditionApply_HasPlayerRaceKeyword(ConnectedCondition& condition)
{
	return DoesConditionApply_HasRaceKeyword(condition, RE::PlayerCharacter::GetSingleton());
}

bool Filter::DoesConditionApply_IsPlayerInFaction(ConnectedCondition& condition)
{
	return DoesConditionApply_IsInFaction(condition, RE::PlayerCharacter::GetSingleton());
}

bool Filter::DoesConditionApply_HasKeyword(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	auto keywordForm = skyrim_cast<RE::BGSKeywordForm*>(boundObj);
	if (!keywordForm)
		return false;

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string keyword;
		if (!condition.GetParam(i, keyword))
			continue;

		if (keywordForm->HasKeywordString(keyword))
			return true;
	}

	return false;
}

bool Filter::DoesConditionApply_HasRaceKeyword(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	auto race = actor->GetRace();
	if (!race)
		return false;

	auto keywords = race->GetKeywords();

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string keywordParam;
		if (!condition.GetParam(i, keywordParam))
			continue;

		for(int j = 0; j < keywords.size(); j++)
		{
			RE::BGSKeyword* keyword = keywords[j];

			if (keyword && keyword->formEditorID.c_str() == keywordParam)
			{
				return true;
			}
		}
	}

	return false;
}

// note: requires po3 tweaks to be installed, otherwise active effects and their editor IDs aren't cached, and this will likely always fail
bool Filter::DoesConditionApply_HasMagicEffect(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	if (!objectRef)
		return false;

	auto magicTarget = objectRef->GetMagicTarget();
	if (!magicTarget)
		return false;

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string keyword;
		if (!condition.GetParam(i, keyword))
			continue;

		auto bgsKeyword = RE::TESForm::LookupByEditorID<RE::EffectSetting>(keyword);
		if (!bgsKeyword)
			continue;

		if (magicTarget->HasMagicEffect(bgsKeyword))
			return true;
	}

	return false;
}

bool Filter::DoesConditionApply_IsScriptAttached(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	auto objForm = boundObj->As<RE::TESForm>();
	if (!objForm)
		return false;

	std::string scriptName;
	if (!condition.GetParam(0, scriptName))
	{
		logger::warn("BTPS: filter preset failed on function HasScript - missing parameter 0 (string)");
		return false;
	}

	return Papyrus::IsScriptAttached(objForm, scriptName);
}

bool Filter::DoesConditionApply_IsActor(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	return objectRef && objectRef->As<RE::Actor>();
}

bool Filter::DoesConditionApply_IsDead(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	auto lifeState = actor->AsActorState()->GetLifeState();
	return lifeState == RE::ACTOR_LIFE_STATE::kDead || lifeState == RE::ACTOR_LIFE_STATE::kDying;
}

bool Filter::DoesConditionApply_IsInBleedOut(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	auto lifeState = actor->AsActorState()->GetLifeState();
	return lifeState == RE::ACTOR_LIFE_STATE::kBleedout || lifeState == RE::ACTOR_LIFE_STATE::kEssentialDown;
}

bool Filter::DoesConditionApply_IsUnconscious(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	return actor->AsActorState()->GetLifeState() == RE::ACTOR_LIFE_STATE::kUnconcious;
}

bool Filter::DoesConditionApply_IsRestrained(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	return actor->AsActorState()->GetLifeState() == RE::ACTOR_LIFE_STATE::kRestrained;
}

bool Filter::DoesConditionApply_IsReanimated(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	return actor->AsActorState()->GetLifeState() == RE::ACTOR_LIFE_STATE::kReanimate;
}

bool Filter::DoesConditionApply_IsEssential(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	return actor->IsEssential();
}

bool Filter::DoesConditionApply_IsInFaction(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	if (!objectRef)
		return false;

	auto actor = objectRef->As<RE::Actor>();
	if (!actor)
		return false;

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string keyword;
		if (!condition.GetParam(i, keyword))
			continue;

		auto tesFaction = RE::TESForm::LookupByEditorID<RE::TESFaction>(keyword);

		if (actor->IsInFaction(tesFaction))
			return true;
	}

	return false;
}

bool Filter::DoesConditionApply_IsValuePerWeightEqual(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float param = 0;
	if (!condition.GetParam(0, param))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueEqual', invalid param at idx 0 ", condition.GetParentName());
		return false;
	}

	float ValuePerWeight = ((float)objectRef->GetGoldValue()) / boundObj->GetWeight();
	return Util::IsRoughlyEqual(ValuePerWeight, param, 0.01f);
}

bool Filter::DoesConditionApply_IsValuePerWeightMore(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
		auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float param = 0;
	if (!condition.GetParam(0, param))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueEqual', invalid param at idx 0 ", condition.GetParentName());
		return false;
	}

	float ValuePerWeight = ((float)objectRef->GetGoldValue()) / boundObj->GetWeight();
	return ValuePerWeight > param;
}

bool Filter::DoesConditionApply_IsValuePerWeightLess(ConnectedCondition& condition, RE::TESObjectREFR* objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float param = 0;
	if (!condition.GetParam(0, param))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueEqual', invalid param at idx 0 ", condition.GetParentName());
		return false;
	}

	float ValuePerWeight = ((float)objectRef->GetGoldValue()) / boundObj->GetWeight();
	return ValuePerWeight < param;
}

bool Filter::DoesConditionApply_IsValueEqual(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	int valueParam = 0;
	if (!condition.GetParam(0, valueParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueMore', invalid param at idx 0 ", condition.GetParentName());
		return false;
	}

	return boundObj->GetGoldValue() == valueParam;
}

bool Filter::DoesConditionApply_IsValueMore(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	int valueParam = 0;
	if (!condition.GetParam(0, valueParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueMore', invalid param at idx 0 ", condition.GetParentName());
		return false;
	}

	return boundObj->GetGoldValue() > valueParam;
}

bool Filter::DoesConditionApply_IsValueLess(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	int valueParam = 0;
	if (!condition.GetParam(0, valueParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsValueLess', invalid param at idx 0", condition.GetParentName());
		return false;
	}

	return boundObj->GetGoldValue() < valueParam;
}

bool Filter::DoesConditionApply_IsWeightEqual(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float weightParam = 0;
	if (!condition.GetParam(0, weightParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsWeightEqual', invalid param at idx 0", condition.GetParentName());
		return false;
	}

	return Util::IsRoughlyEqual(boundObj->GetWeight(), weightParam, 0.001f);
}

bool Filter::DoesConditionApply_IsWeightMore(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float weightParam = 0;
	if (!condition.GetParam(0, weightParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsWeightMore', invalid param at idx 0", condition.GetParentName());
		return false;
	}

	return boundObj->GetWeight() > weightParam;
}

bool Filter::DoesConditionApply_IsWeightLess(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;

	float weightParam = 0;
	if (!condition.GetParam(0, weightParam))
	{
		logger::warn("BTPS: FilterPreset '{}' failed on 'IsWeightLess', invalid param at idx 0", condition.GetParentName());
		return false;
	}

	return boundObj->GetWeight() < weightParam;
}

bool Filter::DoesConditionApply_IsEmptyContainer(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	// using the method from QuickLoot compatibility out of laziness
	return Compatibility::QuickLootRE::IsEmpty(objectRef);
}

bool Filter::DoesConditionApply_IsSpellBook(ConnectedCondition & condition, RE::TESObjectREFR * objectRef)
{
	auto boundObj = objectRef->GetObjectReference();
	if (!boundObj)
		return false;
	else
	{
		auto book = boundObj->As<RE::TESObjectBOOK>();
		return (book && book->TeachesSpell());
	}
}

bool Filter::DoesConditionApply_LocationHasKeyword(ConnectedCondition & condition)
{
	auto playerChar = RE::PlayerCharacter::GetSingleton();
	if (!playerChar)
	{
		logger::warn("BTPS: filterPreset failed on LocationHasKeyword, failed to retrieve player ref");
		return false;
	}

	auto location = playerChar->GetCurrentLocation();
	if (!location)
	{
		//logger::warn("BTPS: filterPreset failed on LocationHasKeyword, failed to retrieve location ref");
		return false;
	}

	for (int i = 0; i < condition.Parameters.size(); i++)
	{
		std::string keyword;
		if (!condition.GetParam(i, keyword))
			continue;

		if (location->HasKeywordString(keyword))
			return true;
	}

	return false;
}

bool Filter::ShouldFilterFormType(RE::FormType formType, bool shouldFilterPrevious)
{
	bool shouldFilter = shouldFilterPrevious;

    for (int i = 0; i < FormTypes.size(); i++)
    {
        auto currFormType = FormTypes[i];

		if (currFormType.FormType == formType || 
			currFormType.FormType == RE::FormType::Max)
		{
			if (shouldFilter)
			{
				if (!currFormType.Remove)
					shouldFilter = true;
			}
			else
				shouldFilter = currFormType.Remove;
		}
    }

	return shouldFilter;
}

bool Filter::DoesConditionApply_IsBook(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref)
{
	auto boundObj = object_ref->GetObjectReference();
	if (!boundObj)
		return false;
	else
	{
		auto book = boundObj->As<RE::TESObjectBOOK>();
		return book != nullptr;
	}
}

bool Filter::DoesConditionApply_IsBookRead(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref)
{
	auto boundObj = object_ref->GetObjectReference();
	if (!boundObj)
		return false;
	else
	{
		auto book = boundObj->As<RE::TESObjectBOOK>();
		return (book && book->IsRead());
	}
}

bool Filter::DoesConditionApply_IsSkillBook(const ConnectedCondition& condition, RE::TESObjectREFR* object_ref)
{
	auto boundObj = object_ref->GetObjectReference();
	if (!boundObj)
		return false;
	else
	{
		auto book = boundObj->As<RE::TESObjectBOOK>();
		return (book && book->TeachesSkill());
	}
}

ECondition Filter::MakeCondition(std::string conditionName, bool &inverseOut)
{
    Util::StripWhiteSpaces(conditionName);
    
	if (!conditionName.empty())
    {
        if (conditionName[0] == '!')
        {
            inverseOut = true;
            conditionName.erase(0, 1);
        }
        else
            inverseOut = false;

		auto condition = magic_enum::enum_cast<ECondition>(conditionName);
		if (!condition.has_value())
		{
			logger::warn("BTPS: invalid function name '{}'", conditionName);
			return ECondition::NONE;
		}

		return condition.value();
    }

	logger::warn("BTPS: empty function name", conditionName);
	return ECondition::NONE;
}

EConditionConnector Filter::MakeConditionConnector(std::string conditionConnectorName)
{
    if (conditionConnectorName == "and") return EConditionConnector::AND;
    if (conditionConnectorName == "or") return EConditionConnector::OR;

	return EConditionConnector::NONE;
}

ConnectedCondition::ConnectedCondition(std::shared_ptr<Filter> parent,ECondition condition, EConditionConnector connector, bool inverse, std::vector<std::string> parameters)
{
	Parent = parent;
    Condition = condition;
    Connector = connector;
    Inverse = inverse;

	for (std::string& currString : parameters)
	{
		FilterParam newParam(currString);
		Parameters.push_back(newParam);
	}
}

bool ConnectedCondition::GetParam(int idx, int& valueOut)
{
	if (idx >= Parameters.size())
		return false;

	auto param = Parameters[idx];
	if (!param.IntValue)
		return false;

	valueOut = param.IntValue.value();
	return true;
}

bool ConnectedCondition::GetParam(int idx, float& valueOut)
{
	if (idx >= Parameters.size())
		return false;

	auto param = Parameters[idx];
	if (!param.FloatValue)
		return false;

	valueOut = param.FloatValue.value();
	return true;
}

bool ConnectedCondition::GetParam(int idx, bool& valueOut)
{
	if (idx >= Parameters.size())
		return false;

	auto param = Parameters[idx];
	if (!param.BoolValue)
		return false;

	valueOut = param.BoolValue.value();
	return true;
}

bool ConnectedCondition::GetParam(int idx, std::string & valueOut)
{
	if (idx >= Parameters.size())
		return false;

	auto param = Parameters[idx];
	if (!param.StringValue)
		return false;

	valueOut = param.StringValue.value();
	return true;
}

std::string ConnectedCondition::GetParentName()
{
	if (!Parent)
		return "<NONE>";

	return Parent->FilterName;
}
