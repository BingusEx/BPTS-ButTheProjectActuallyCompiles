#include "RayCast.h"
#include "RevE/Offsets.h"
#include "bhkLinearCastCollector.h"

#include "lib/Util.h"

/* From SmoothCam */
Raycast::RayResult Raycast::CastRay(glm::vec4 start, glm::vec4 end, float traceHullSize)
noexcept
{
	RayResult res;

	const auto ply = RE::PlayerCharacter::GetSingleton();
	const auto cam = RE::PlayerCamera::GetSingleton();
	if (!ply->parentCell || !cam->unk120)
		return res;

	auto physicsWorld = ply->parentCell->GetbhkWorld();
	if (physicsWorld)
	{
		typedef bool(__fastcall* RayCastFunType)(
			decltype(RE::PlayerCamera::unk120) physics, RE::bhkWorld* world, glm::vec4& rayStart,
			glm::vec4& rayEnd, uint32_t* rayResultInfo, RE::NiAVObject** hitActor, float traceHullSize
			);

		static auto cameraCaster = REL::Relocation<RayCastFunType>(Offsets::CameraCaster);

		res.hit = cameraCaster(
			cam->unk120, physicsWorld,
			start, end, static_cast<uint32_t*>(res.data), &res.hitObject,
			traceHullSize
		);
	}

	if (res.hit)
	{
		res.hitPos = end;
		res.rayLength = glm::length(static_cast<glm::vec3>(res.hitPos) - static_cast<glm::vec3>(start));
	}

	return res;
}

SkyrimSE::bhkLinearCastCollector* getCastCollector() noexcept
{
	static SkyrimSE::bhkLinearCastCollector collector = SkyrimSE::bhkLinearCastCollector();
	return &collector;
}

Raycast::RayResult Raycast::hkpCastRay(const glm::vec4& start, const glm::vec4& end) noexcept
{
	constexpr auto hkpScale = 0.0142875f;
	const auto dif = end - start;

	SkyrimSE::bhkRayCastInfo info;
	info.start = start * hkpScale;
	info.end = dif * hkpScale;
	info.collector = getCastCollector();
	info.collector->reset();

	const auto ply = RE::PlayerCharacter::GetSingleton();
	if (!ply->parentCell)
		return {};

	// ignore player
	if (ply->loadedData && ply->loadedData->data3D)
		info.collector->addFilter(ply->loadedData->data3D.get());

	// ignore ridden mount
    if (auto playerMount = Util::GetPlayerMountRef())
	{
		info.collector->addFilter(playerMount->GetCurrent3D());
	}

    if (auto physicsWorld = ply->parentCell->GetbhkWorld())
	{
		typedef void (__thiscall RE::bhkWorld::*CastRay)(SkyrimSE::hkpRayCastInfo*) const;
		(physicsWorld->*reinterpret_cast<CastRay>(&RE::bhkWorld::PickObject))(&info);
	}

	SkyrimSE::bhkRayHitResult best = {};
	best.hitFraction = 1.0f;
	glm::vec4 bestPos = {};

	for (auto& hit : info.collector->results)
	{
		const auto pos = (dif * hit.hitFraction) + start;
		if (best.hit == nullptr)
		{
			best = hit;
			bestPos = pos;
			continue;
		}

		if (hit.hitFraction < best.hitFraction)
		{
			best = hit;
			bestPos = pos;
		}
	}

	RayResult result;
	result.hitPos = bestPos;
	result.rayLength = glm::length(bestPos - start);

	if (!best.hit)
		return result;

	typedef RE::NiAVObject* (*_GetUserData)(SkyrimSE::bhkShapeList*);
	auto getAVObject = REL::Relocation<_GetUserData>(Offsets::GetNiAVObject);
	auto av = getAVObject(best.hit);

	result.hit = av != nullptr;
	result.hitObject = av;
	result.hitObject = av;

	return result;
}
