#pragma once
#include "Util.h"
#include "Settings.h"
#include "UI/DebugAPI.h"
#include "bhkRigidBodyT.h"
#include "RE/RTTI.h"
#include "RevE/Offsets.h"
#include "API/Compatibility.h"
#include "FenixUtils.h"

// thanks to https://github.com/fenix31415/UselessFenixUtils/blob/master/src/Utils.cpp
RE::bhkPickData Util::CrosshairRaycast(RE::Actor* caster, RE::NiPoint3 ray_start, RE::NiPoint3 ray_end)
{
	auto havokWorldScale = RE::bhkWorld::GetWorldScale();
	RE::bhkPickData pick_data;

	pick_data.rayInput.from = ray_start * havokWorldScale;
	pick_data.rayInput.to = ray_end * havokWorldScale;

	uint32_t collisionFilterInfo = 0;
	caster->GetCollisionFilterInfo(collisionFilterInfo);
	pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionFilterInfo >> 16) << 16) |
				                    static_cast<uint32_t>(RE::COL_LAYER::kItemPicker);

	caster->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
	return pick_data;
}

// thanks to https://github.com/fenix31415/UselessFenixUtils/blob/master/src/Utils.cpp
RE::NiPoint3 Util::GetRayCastHitPos(RE::bhkPickData pick_data, RE::NiPoint3 ray_start, RE::NiPoint3 ray_end)
{
	RE::NiPoint3 hitpos;
	if (pick_data.rayOutput.HasHit())
	{
		hitpos = ray_start + (ray_end - ray_start) * pick_data.rayOutput.hitFraction;
	} else
	{
		hitpos = ray_end;
	}
	return hitpos;
}

RE::NiAVObject* Util::GetRayCastHitRef(RE::bhkPickData pick_data)
{
	if (pick_data.rayOutput.HasHit())
	{
		return RE::TESHavokUtilities::FindCollidableObject(*pick_data.rayOutput.rootCollidable);

		//auto collidableToNiObj = REL::Relocation<Offsets::HKPCollidableToNiAVObject_func> { Offsets::HKPCollidableToNiAVObject };
		//auto outRef = collidableToNiObj(pick_data.rayOutput.rootCollidable);

		//return outRef;
	}
	return nullptr;
}
