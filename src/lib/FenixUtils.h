#pragma once
#include <vector>

#include "RE/N/NiMath.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FocusObject.h"

namespace Util
{
	RE::bhkPickData CrosshairRaycast(RE::Actor* caster, RE::NiPoint3 ray_start, RE::NiPoint3 ray_end);

	RE::NiPoint3 GetRayCastHitPos(RE::bhkPickData pick_data, RE::NiPoint3 ray_start, RE::NiPoint3 ray_end);

	RE::NiAVObject* GetRayCastHitRef(RE::bhkPickData pick_data);
};
