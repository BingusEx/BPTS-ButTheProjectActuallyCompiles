#include "NearbyActorScanner.h"
#include "FocusManager.h"
#include "Papyrus.h"
#include "Settings.h"
#include "API/Compatibility.h"
#include "RevE/Offsets.h"
#include "UI/SelectionWidget.h"

#include <glm/vec3.hpp>
//#include <boost/format.hpp>

#include "Filters/FilterManager.h"

#include "lib/Util.h"

std::vector<RE::TESObjectREFR*> NearbyActorScanner::LastScanResults;

std::vector<RE::TESObjectREFR*> NearbyActorScanner::FindCloseActors()
{
	LastScanResults.clear();

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	auto playerPos = playerCharacter->GetPosition();
	playerPos.z += (playerCharacter->GetHeight() / 2.0f);

	Util::ForEachReferenceInRange(playerPos, FocusManager::GetMaxInteractionRange(), ReferenceLoopCallback);

	return LastScanResults;
}

RE::BSContainer::ForEachResult NearbyActorScanner::ReferenceLoopCallback(RE::TESObjectREFR* object)
{
	if (!object || !IsValid(*object))
		return RE::BSContainer::ForEachResult::kContinue;

	LastScanResults.push_back(object->AsReference());
	return RE::BSContainer::ForEachResult::kContinue;
}

bool NearbyActorScanner::IsValidFormType(RE::FormType t)
{
	return t == RE::FormType::Misc ||
		t == RE::FormType::Activator ||
		t == RE::FormType::ActorCharacter ||
		t == RE::FormType::NPC ||
		t == RE::FormType::Container ||
		t == RE::FormType::Ingredient ||
		t == RE::FormType::Flora ||
		t == RE::FormType::Tree ||
		t == RE::FormType::Weapon ||
		t == RE::FormType::Door ||
		t == RE::FormType::Furniture ||
		t == RE::FormType::AlchemyItem ||
		t == RE::FormType::Book ||
		t == RE::FormType::Armor ||
		t == RE::FormType::Ammo ||
		t == RE::FormType::Note ||
		t == RE::FormType::Scroll ||
		t == RE::FormType::SoulGem ||
		t == RE::FormType::Spell ||
		t == RE::FormType::KeyMaster ||
	    t == RE::FormType::TalkingActivator ||
		t == RE::FormType::Apparatus ||
		t == RE::FormType::Projectile ||
		t == RE::FormType::Light;
}

bool NearbyActorScanner::IsMovingProjectile(RE::TESObjectREFR& object)
{
	auto projectileRef = object.As<RE::MissileProjectile>();

	return (projectileRef && projectileRef->GetMissileRuntimeData().impactResult == RE::ImpactResult::kNone);
}

bool NearbyActorScanner::IsValid(RE::TESObjectREFR& object)
{
	auto formType = object.GetObjectReference()->GetFormType();

	return !(object.IsDisabled()			||
		object.IsMarkedForDeletion()		||
		HasBlockingFlagsNative(object)		||
		HasBlockingFlags(object)			||
		!IsValidFormType(formType)			||
		HasInvalidName(object)				||
        HasFilteredName(object)				||
		object.IsPlayerRef()				||
		!HasValidCollisionObject(object)	||
		IsMovingProjectile(object)			||
		HasBlockingFlagsHorseback(object));
}

bool NearbyActorScanner::HasBlockingFlagsNative(RE::TESObjectREFR& object)
{
	auto characterObject = object.As<RE::Actor>();
	if (characterObject)
	{
		// ash piles have their own object ref, ignore associated character. Otherwise, two separate objects will show up
		if (characterObject->extraList.GetAshPileRef())
			return true;
	}

	auto playerChar = RE::PlayerCharacter::GetSingleton();
	auto boundObj = object.GetObjectReference();

	if (!playerChar || !boundObj)
		return false;

	// ignore harvested plants
    auto objHarvestable = object.GetObjectReference()->As<RE::TESFlora>();
    auto objTree = object.GetObjectReference()->As<RE::TESObjectTREE>();
    if ((objHarvestable || objTree) &&
        (object.formFlags & RE::TESObjectREFR::RecordFlags::kHarvested) != 0)
        return true;

	if (FilterManager::ShouldFilterObj(&object))
		return true;

	return false;
}

bool NearbyActorScanner::HasBlockingFlags(RE::TESObjectREFR& object)
{
	bool isShownFurnitureMarker = false;
	auto objFurniture = object.GetObjectReference()->As<RE::TESFurniture>();
	if (objFurniture)
	{
		if (objFurniture->IsMarker())
		{
			if (Settings::ShowFurnitureMarkers)
				isShownFurnitureMarker = true;
			else
				return true;
		}
	}

	if (!object.GetPlayable())
		return true;

	// disables ore mines and some other objects. Filters some valid
	// things too though, such as the door leading to the companions'
	// initiation cave - I must be missing something
	if (object.IsActivationBlocked())
        return true;

	RE::TESObjectLIGH* light = skyrim_cast<RE::TESObjectLIGH*>(object.GetObjectReference());
	if (light && !light->CanBeCarried())
		return true;

	auto boundObject = object.GetObjectReference();
	if (!boundObject)
		return true;

	//if (RE::PlayerCharacter::GetSingleton()->HasPerkEntries(RE::BGSPerkEntry::EntryPoint::kFilterActivation))
		//return true;

	auto mesh = object.GetCurrent3D();
	if (mesh)
	{
		const auto bsxFlags = mesh->GetExtraData<RE::BSXFlags>("BSX");
		auto model = boundObject->As<RE::TESModel>();

		// hopefully this is the last puzzle piece in order to hide triggers and markers that should be invisible
		// has the editor marker flag set or no loaded model
		if (bsxFlags && (bsxFlags->value & (int)RE::BSXFlags::Flag::kEditorMarker) == 1 ||
			(model && model->GetModel()[0] == '\0'))
			return true;
	}
	// ignore objects without associated mesh data - probably some invisible marker
	// unless it's a furniture marker and showing markers is enabled
	else if (!isShownFurnitureMarker)
		return true;

	return false;
}

bool NearbyActorScanner::HasBlockingFlagsHorseback(RE::TESObjectREFR& object)
{
    auto playerMount = Util::GetPlayerMountRef();
    if (!playerMount)
        return false;
	
	// with Settings::IsHoldToDismountEnabled disabled, the mount should not be filtered out, otherwise
	// the player can't dismount anymore
	if (Settings::IsHoldToDismountEnabled && &object == playerMount)
        return true;

	auto objectRef = object.GetObjectReference();

	if (!objectRef)
		return true;

	if (objectRef->Is(RE::TESFurniture::FORMTYPE))
		return true;

	// ignore all activatables that have a script called 'MineOreScript' attached - ignore all mineable objects while on horseback
	//if (objectRef->Is(RE::TESObjectACTI::FORMTYPE) && Papyrus::IsScriptAttached(object.As<RE::TESForm>(), "MineOreScript"))
		//return true;

	return false;
}

// filter out objects without name
bool NearbyActorScanner::HasInvalidName(RE::TESObjectREFR& object)
{
	std::string displayName = object.GetDisplayFullName();

	if (Util::IsEmpty(displayName.c_str()))
		return true;

	auto boundObj = object.GetObjectReference();
	if (!boundObj)
		return true;

	RE::BSString activateString;
	boundObj->GetActivateText(&object, activateString);

	return Util::IsEmpty(activateString.c_str());
}

bool NearbyActorScanner::HasFilteredName(RE::TESObjectREFR& object)
{
    std::string displayName = object.GetDisplayFullName();

    Util::ToLowerString(displayName);
    return Settings::IsNameInIgnoreList(displayName);
}

bool NearbyActorScanner::HasValidCollisionObject(RE::TESObjectREFR& object)
{
	// collision object check fails on actors sometimes, and isn't necessary on them anyways
	if (object.As<RE::Actor>())
		return true;

	auto mesh = object.GetCurrent3D();
	if (!mesh)
		return true;

	auto boundObject = object.GetObjectReference();
    if (!boundObject)
        return true;

	auto model = boundObject->As<RE::TESModel>();
    if (!model)
        return true;

	auto modelName = model->GetModel();
    auto displayName = object.GetDisplayFullName();

	if (Settings::HasObjectOverride(modelName, displayName))
        return true;

	return Util::HasCollisionObjectRecursive(mesh->AsNode());
}

/*bool NearbyActorScanner::CheckCollisionLayers(RE::TESObjectREFR& object)
{
	auto mesh = object.GetCurrent3D();
	if (!mesh)
		return false;

	auto clObjects = Util::FindAllCollisionObjectsRecursive(mesh->AsNode());

	// trap linkers etc. find no collision layer at all
	for (auto& clObject : clObjects)
	{
		auto clLayer = Util::GetCollisionLayer(clObject);
		logger::info("clLayer: {}", clLayer);
	}

	// no collision object = invalid layer
	return clObjects.empty();
}*/
