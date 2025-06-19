#pragma once
#include <vector>

class NearbyActorScanner
{
public:
	static std::vector<RE::TESObjectREFR*> FindCloseActors();

	static bool IsValid(RE::TESObjectREFR& object);

	static bool IsValidFormType(RE::FormType t);
	static bool IsMovingProjectile(RE::TESObjectREFR& object);
	static bool HasBlockingFlagsNative(RE::TESObjectREFR& object);
	static bool HasBlockingFlags(RE::TESObjectREFR& object);
	static bool HasBlockingFlagsHorseback(RE::TESObjectREFR& object);
	static bool HasInvalidName(RE::TESObjectREFR& object);
	static bool HasFilteredName(RE::TESObjectREFR& object);

	static bool HasValidCollisionObject(RE::TESObjectREFR& object);

	//static bool CheckCollisionLayers(RE::TESObjectREFR& object);

private:
	static RE::BSContainer::ForEachResult ReferenceLoopCallback(RE::TESObjectREFR* object);

	static std::vector<RE::TESObjectREFR*> LastScanResults;
};
