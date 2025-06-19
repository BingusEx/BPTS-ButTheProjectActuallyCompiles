#pragma once
#include "FocusManager.h"

#include "NearbyActorScanner.h"
#include "UI/SelectionWidget.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <bitset>

#include "UI/DebugAPI.h"
#include "Settings.h"
#include "lib/FenixUtils.h"
#include "lib/Util.h"

bool FocusManager::IsEnabled = false;

float FocusManager::PRIORITY_ANGLE_MULT		= 5.0f;
float FocusManager::PRIORITY_DIST_MULT		= 1.0f;
float FocusManager::PRIORITY_TYPE_MULT		= 0.1f;
float FocusManager::PRIORITY_SPECIFIC_MULT	= 0.2f;
float FocusManager::PRIORITY_VALUE_MULT		= 0.5f;

float FocusManager::MAX_INTERACTION_RANGE = 200.0f;
float FocusManager::MAX_ANGLE_DIFFERENCE = 0.5f;

float FocusManager::MAX_INTERACTION_RANGE_MOUNTED = 350.0f;
float FocusManager::MAX_ANGLE_DIFFERENCE_MOUNTED = 0.3f;

int32_t FocusManager::MAX_VALUE = 200;

int FocusManager::NUM_TRACES_TO_OBJECT_HORIZONTAL = 3;
int FocusManager::NUM_TRACES_TO_OBJECT_VERTICAL = 3;
float FocusManager::TRACES_TOTAL_WIDTH = 75.0f;

float FocusManager::CHARACTER_HEIGHT_BONUS = 25.0f;

float FocusManager::NATIVE_SELECTION_PRIORITY_BONUS = 0.1f;
float FocusManager::PREVIOUS_SELECTION_PRIORITY_BONUS = 0.01f;


float FocusManager::NPC_SELECTION_PRIORITY_BONUS = 0.0f;

std::vector<std::shared_ptr<FocusObject>> FocusManager::SortedFocusObjects;

float FocusManager::GetMaxInteractionRange()
{
	auto player = RE::PlayerCharacter::GetSingleton();

	if (player && player->IsOnMount())
		return MAX_INTERACTION_RANGE_MOUNTED;
	return MAX_INTERACTION_RANGE;
}

float FocusManager::GetMaxAngleDif()
{
	auto player = RE::PlayerCharacter::GetSingleton();

	if (player && player->IsOnMount())
		return MAX_ANGLE_DIFFERENCE_MOUNTED;
	return MAX_ANGLE_DIFFERENCE;
}

std::shared_ptr<FocusObject> FocusManager::ActiveFocusObject = nullptr;
std::shared_ptr<FocusObject> FocusManager::LastIdealFocusObject = nullptr;
std::shared_ptr<FocusObject> FocusManager::SelectedFocusObject = nullptr;



int FocusManager::GetFocusObjectIdx(std::shared_ptr<FocusObject> focusObj)
{
	if (!focusObj || !focusObj->ObjectRef.get())
		return -1;

	for (int i = 0; i < SortedFocusObjects.size(); i++)
	{
		auto currFocusObj = SortedFocusObjects[i];
        if (currFocusObj && currFocusObj->ObjectRef.get() && currFocusObj->ObjectRef.get()->formID == focusObj->ObjectRef.get()->formID)
			return i;
	}

	return -1;
}

int FocusManager::GetSelectedPriorityIdx()
{
	if (!SelectedFocusObject)
		return 0;

	int idx = GetFocusObjectIdx(SelectedFocusObject);
	if (idx < 0)
	{
		SelectedFocusObject = nullptr;
		idx = 0;
	}

	return idx;
}

void FocusManager::SelectIncreasePriority()
{
	if (SortedFocusObjects.size() <= 1)
	{
		SelectedFocusObject = nullptr;
		return;
	}

	int currIdx = 0;

	if (SelectedFocusObject && SelectedFocusObject->ObjectRef.get())
		currIdx = std::max(0, GetFocusObjectIdx(SelectedFocusObject));

	if (Settings::EnableContinuousObjectCycle)
	{
		currIdx++;
		if (currIdx >= SortedFocusObjects.size())
			currIdx = 0;
	}
	else
		currIdx = std::min((int)SortedFocusObjects.size() - 1, currIdx + 1);

	// if the user has cycled back to the starting item, handle like no cycling has been performed before (selection is unlocked)
	// doesn't apply when continuous cycling is enabled
	if (currIdx == 0 && !Settings::EnableContinuousObjectCycle)
		SelectedFocusObject = nullptr;
	else
		SelectedFocusObject = SortedFocusObjects[currIdx];
}

void FocusManager::SelectDecreasePriority()
{
	if (SortedFocusObjects.empty())
	{
		SelectedFocusObject = nullptr;
		return;
	}

	int currIdx = 0;
	if (SelectedFocusObject && SelectedFocusObject->ObjectRef)
		currIdx = std::max(0, GetFocusObjectIdx(SelectedFocusObject));

	if (Settings::EnableContinuousObjectCycle)
	{
		currIdx--;
		if (currIdx < 0)
			currIdx = (int)SortedFocusObjects.size() - 1;
	}
	else
		currIdx = std::max(0, currIdx - 1);

	// if the user has cycled back to the starting item, handle like no cycling has been performed before (selection is unlocked)
	// doesn't apply when continuous cycling is enabled
	if (currIdx == 0 && !Settings::EnableContinuousObjectCycle)
		SelectedFocusObject = nullptr;
	else
		SelectedFocusObject = SortedFocusObjects[currIdx];
}

std::mutex m_enabled;

bool FocusManager::GetIsEnabled()
{
	std::lock_guard lock(m_enabled);
	return IsEnabled;
}

void FocusManager::SetIsEnabled(bool enabled)
{
	std::lock_guard lock(m_enabled);
	IsEnabled = enabled;
}

RE::TESObjectREFR* FocusManager::FocusRef_Orig = nullptr;
std::mutex m_getFocusObj;

std::shared_ptr<FocusObject> FocusManager::GetActiveFocusObject()
{
	std::lock_guard lock(m_getFocusObj);
	return ActiveFocusObject;
}

std::shared_ptr<FocusObject> FocusManager::GetCustomFocusObject(RE::TESObjectREFR* nativeSelection)
{
	if (nativeSelection != FocusRef_Orig)
	{
		OnNativeSelectionChanged(nativeSelection);
		FocusRef_Orig = nativeSelection;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter || !playerCharacter->GetParentCell())
		return nullptr;

	auto actors = NearbyActorScanner::FindCloseActors();

	// if not already in vector, add native selection
	if (nativeSelection &&
		!NearbyActorScanner::HasBlockingFlagsNative(*nativeSelection) &&
        !NearbyActorScanner::HasFilteredName(*nativeSelection) &&
        !NearbyActorScanner::HasBlockingFlagsHorseback(*nativeSelection) &&
		std::find(actors.begin(), actors.end(), nativeSelection) >= actors.end())
	{
		actors.push_back(nativeSelection);
	}

	MakeSortedObjects(actors);
	CheckSortedObjectsOcclusion();

	std::shared_ptr<FocusObject> focusObj = GetSelectedFocusObject();

	// draw debug sphere
	if (Settings::Draw_Bounds_Sphere && focusObj && focusObj->ObjectRef.get())
	{
		auto objectRef = focusObj->ObjectRef;

		auto mesh = objectRef.get()->GetCurrent3D();
		if (mesh)
		{
			auto niWorldBound = mesh->worldBound.center;
			DebugAPI::DrawSphere(glm::vec3(niWorldBound.x, niWorldBound.y, niWorldBound.z), mesh->worldBound.radius, 10, glm::vec4(0.0f, 0.0f, 1.0f, 0.5f * Settings::DebugDrawAlpha), Settings::DebugDrawThickness);
		}
	}

	return focusObj;
}

void FocusManager::OnNativeSelectionChanged(RE::TESObjectREFR* nativeSelection)
{
	SelectionWidget::OnNativeSelectionChanged(nativeSelection);
}

bool FocusManager::IsPartOfObject(RE::NiAVObject* avObject, RE::TESObjectREFR* object)
{
	if (!object || !avObject)
		return false;

	if (avObject->userData == object)
		return true;

	return FindParentObjectREFR(avObject, object) || FindChildObjectREFR(avObject, object);
}

constexpr int FIND_PARENT_MAX_RECURSION = 2;
bool FocusManager::FindParentObjectREFR(RE::NiAVObject* avObject, RE::TESObjectREFR* object, int recursionDepth)
{
	if (!avObject)
		return false;

	if (avObject->userData && avObject->userData == object)
		return true;

	if (avObject->parent && recursionDepth <= FIND_PARENT_MAX_RECURSION)
		return FindParentObjectREFR(avObject->parent, object, recursionDepth + 1);

	return false;
}

constexpr int FIND_CHILD_MAX_RECURSION = 2;
bool FocusManager::FindChildObjectREFR(RE::NiAVObject* avObject, RE::TESObjectREFR* object, int recursionDepth)
{
	if (!avObject)
		return false;

	if (avObject->userData && avObject->userData == object)
		return true;

	auto node = avObject->AsNode();
	if (node && recursionDepth <= FIND_CHILD_MAX_RECURSION)
	{
		for (auto child : node->children)
		{
			if (child && FindChildObjectREFR(child.get(), object, recursionDepth + 1))
				return true;
		}
	}

	return false;
}

void FocusManager::SetFocusObject(std::shared_ptr<FocusObject> focusObj)
{
	auto crosshair = RE::CrosshairPickData::GetSingleton();
	auto lastFocusObj = ActiveFocusObject;

	ActiveFocusObject = focusObj;
	LastIdealFocusObject = GetBestFocusObject();
	
	if (crosshair)
	{
		if (ActiveFocusObject)
			crosshair->target = focusObj->ObjectRef;
		else
			crosshair->target = RE::ObjectRefHandle();

		if ((bool)lastFocusObj != (bool)focusObj ||
			(lastFocusObj && focusObj && (lastFocusObj->ObjectRef != focusObj->ObjectRef || 
			focusObj->CollisionObject.CollisionObject != lastFocusObj->CollisionObject.CollisionObject)))
		{
			SelectionWidget::OnSelectionChanged();
		}
	}

	SelectionWidget::Update();
}

void FocusManager::SetFocusObject(RE::TESObjectREFR* focusRef)
{
	auto collisionObject = GetBestCollisionObject(focusRef);
	auto focusObj = std::make_shared<FocusObject>(0.0f, focusRef, collisionObject);

	SetFocusObject(focusObj);
}

void FocusManager::MakeSortedObjects(std::vector<RE::TESObjectREFR*> objects)
{
	SortedFocusObjects.clear();

	for (int i = 0; i < objects.size(); i++)
	{
		RE::TESObjectREFR* object = objects[i];

		auto collisionObject = GetBestCollisionObject(object);
		auto focusObject = std::make_shared<FocusObject>(0.0f, object, collisionObject);

		ObjectBound focusBound;
		if (!collisionObject.HasObjectBound && Util::GetBoundingBox(focusObject.get(), focusBound))
            focusObject->CollisionObject.SetObjectBound(focusBound);

		if (Settings::Draw_Bounds_Box)
			DebugAPI::DrawBoundsForMS(focusObject->CollisionObject.BoundingBox, 10, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f * Settings::DebugDrawAlpha), Settings::DebugDrawThickness);

		if (Settings::Draw_Bounds_MinMax)
		{
			auto& bound = focusObject->CollisionObject.BoundingBox;

			DebugAPI::DrawLineForMS(bound.worldBoundMin + glm::vec3(0.0f, 0.0f, -30.0f), bound.worldBoundMin + glm::vec3(0.0f, 0.0f, 30.0f), 20, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f * Settings::DebugDrawAlpha), Settings::DebugDrawThickness);
			DebugAPI::DrawLineForMS(bound.worldBoundMax + glm::vec3(0.0f, 0.0f, -30.0f), bound.worldBoundMax + glm::vec3(0.0f, 0.0f, 30.0f), 20, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f * Settings::DebugDrawAlpha), Settings::DebugDrawThickness);
		}

		if (!InitObjectPriority(focusObject.get()))
		{
			objects.erase(objects.begin() + i);
			i--;
			continue;
		}

		if (LastIdealFocusObject && LastIdealFocusObject->ObjectRef.get() && object == LastIdealFocusObject->ObjectRef.get().get())
		{
			focusObject->Priority -= PREVIOUS_SELECTION_PRIORITY_BONUS;
		}

		auto formType = focusObject->ObjectRef.get()->GetFormType();

		if (formType == RE::FormType::ActorCharacter)
		{
			focusObject->Priority -= NPC_SELECTION_PRIORITY_BONUS;
		}

		// don't trace for the object selected by Skyrim's native crosshair selector
		if (FocusRef_Orig && object == FocusRef_Orig)
			focusObject->Priority -= NATIVE_SELECTION_PRIORITY_BONUS;

		Util::insert_sorted(SortedFocusObjects, focusObject,
			[](std::shared_ptr<FocusObject> first, std::shared_ptr <FocusObject> second) {
				return first->Priority < second->Priority;
			});
	}
}

void FocusManager::CheckSortedObjectsOcclusion()
{
	int priorityIdx = GetSelectedPriorityIdx();

	// priorityIdx + 1 to make sure that the next object is also valid. Otherwise, cycling up can be unreliable
	for (int i = 0; i <= priorityIdx + 1 && i < SortedFocusObjects.size(); i++)
	{
		auto obj = SortedFocusObjects[i];
		// remove objects where no trace succeeds - probably occluded by something
		if ( !(obj && obj->ObjectRef.get().get() == FocusRef_Orig) && !HasSuccessfulRayTrace(obj.get()))
		{
			SortedFocusObjects.erase(SortedFocusObjects.begin() + i);
			i--;
		}
	}
}

std::shared_ptr<FocusObject> FocusManager::GetBestFocusObject()
{
	if (SortedFocusObjects.empty())
		return nullptr;

	return SortedFocusObjects[0];
}

std::shared_ptr<FocusObject> FocusManager::GetSelectedFocusObject()
{
	if (SortedFocusObjects.empty())
		return nullptr;

	int priorityIdx = GetSelectedPriorityIdx();

	return SortedFocusObjects[priorityIdx];
}

bool FocusManager::InitObjectPriority(FocusObject* focusObject)
{
    if (!focusObject || !focusObject->ObjectRef.get())
		return false;

	auto objectRef = focusObject->ObjectRef.get();

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	glm::vec3 cameraLoc = Util::GetCameraPos();
	glm::quat cameraRotQuat = Util::GetCameraRot();

	// --

	auto niPlayerLoc = playerCharacter->GetPosition();

	glm::vec3 playerLoc(niPlayerLoc.x, niPlayerLoc.y, niPlayerLoc.z + playerCharacter->GetHeight() / 2.0f);
	auto playerAngle = playerCharacter->GetAngle();

	auto targetLoc = Util::GetBoundingBoxCenter(focusObject);

	// --

	glm::vec3 toTarget = (playerLoc - targetLoc);
	glm::vec3 camToTarget = (targetLoc - cameraLoc);
	glm::vec3 camToTargetN = Util::NormalizeVector(camToTarget);
	glm::vec3 cameraForward = Util::NormalizeVector(Util::GetForwardVector(cameraRotQuat));

	// --

	float distToTarget = (float) glm::length(toTarget);
	float angleDif = (float) glm::length(camToTargetN - cameraForward);

	// --

	auto maxAngleDif = GetMaxAngleDif();

	// don't accept objects with a too big angle difference, except if the object is the native crosshair target
    if (angleDif > maxAngleDif && objectRef.get() != FocusRef_Orig)
		return false;

	float distPriority		= distToTarget / GetMaxInteractionRange()	* PRIORITY_DIST_MULT;
	float angleDifPriority	= angleDif / maxAngleDif					* PRIORITY_ANGLE_MULT;
	float typePriority		= GetObjectTypePriority(focusObject)		* PRIORITY_TYPE_MULT;
	float specificPriority	= GetSpecificObjectPriority(focusObject)	* PRIORITY_SPECIFIC_MULT;
	float valuePriority		= GetValuePriority(focusObject)				* PRIORITY_VALUE_MULT;

	focusObject->Priority = distPriority + angleDifPriority + typePriority + specificPriority + valuePriority;

	// apply priority modifiers
	ObjectOverride objOverride;
    Settings::GetObjectOverride(objectRef.get(), objOverride);

	if (objOverride.HasPriorityModifier)
	{
		objOverride.ApplyPriorityModifier(focusObject->Priority);
	}

	return true;
}

float formTypeNum = 24.0f;
float FocusManager::GetObjectTypePriority(FocusObject* focusObject)
{
    if (!focusObject || !focusObject->ObjectRef.get())
		return 1.0f;
	auto objectRef = focusObject->ObjectRef;

	switch (objectRef.get()->GetObjectReference()->formType.get())
	{
	case RE::FormType::KeyMaster:			return (1.0f / formTypeNum);
	case RE::FormType::Note:				return (2.0f / formTypeNum);
	case RE::FormType::Scroll:				return (3.0f / formTypeNum);
	case RE::FormType::SoulGem:				return (4.0f / formTypeNum);
	case RE::FormType::Ammo:				return (5.0f / formTypeNum);
	case RE::FormType::Ingredient:			return (6.0f / formTypeNum);
	case RE::FormType::AlchemyItem:			return (7.0f / formTypeNum);
	case RE::FormType::Weapon:				return (8.0f / formTypeNum);
	case RE::FormType::Armor:				return (9.0f / formTypeNum);
	case RE::FormType::Spell:				return (10.0f / formTypeNum);
	case RE::FormType::Light:				return (11.0f / formTypeNum);
	case RE::FormType::Book:				return (12.0f / formTypeNum);
	case RE::FormType::Activator:			return (13.0f / formTypeNum);
	case RE::FormType::Flora:				return (14.0f / formTypeNum);
	case RE::FormType::Tree:				return (15.0f / formTypeNum);
	case RE::FormType::ActorCharacter:		return (16.0f / formTypeNum);
	case RE::FormType::NPC:					return (17.0f / formTypeNum);
	case RE::FormType::Misc:				return (18.0f / formTypeNum);
	case RE::FormType::Container:			return (19.0f / formTypeNum);
	case RE::FormType::Furniture:			return (20.0f / formTypeNum);
	case RE::FormType::Door:				return (21.0f / formTypeNum);
	case RE::FormType::TalkingActivator:	return (22.0f / formTypeNum);
	case RE::FormType::Apparatus:			return (23.0f / formTypeNum);
	}

	return 0.0f;
}

float specificObjectsNum = 1.0f;
float FocusManager::GetSpecificObjectPriority(FocusObject* focusObject)
{
    if (!focusObject || !focusObject->ObjectRef.get())
		return 1.0f;
	auto objectRef = focusObject->ObjectRef;

	switch (objectRef.get()->GetObjectReference()->GetFormID())
	{
		// gold
		case 0xF: return 1.0f - (1.0f / specificObjectsNum);
	}

	return 1.0f;
}

float FocusManager::GetValuePriority(FocusObject* focusObject)
{
    if (!focusObject || !focusObject->ObjectRef.get())
		return 1.0f;
	auto objectRef = focusObject->ObjectRef;

	auto goldValue = objectRef.get()->GetObjectReference()->GetGoldValue();

	// object doesn't have a gold value
	if (goldValue >= 0xffffffff)
		return 1.0f;

	if (MAX_VALUE == 0.0)
		return 1.0f;

	return 1.0f - ( (float)(goldValue) / (float)MAX_VALUE );
}

CollisionFocusObject FocusManager::GetBestCollisionObject(RE::TESObjectREFR* object)
{
	CollisionFocusObject objOut;

	if (!object)
		return objOut;

	auto mesh = object->GetCurrent3D();
	if (!mesh || !mesh->AsNode())
		return objOut;

	auto node = mesh->AsNode();

	auto objCharacter = object->GetObjectReference()->As<RE::TESNPC>();
	if (objCharacter)
	{
		objOut.SetData(mesh->collisionObject.get(), node);

		ObjectBound bound;
		if (Util::GetBoundingBox(object, bound))
			objOut.SetObjectBound(bound);

		return objOut;
	}

    auto boundObject = object->GetObjectReference();
    if (!boundObject)
        return objOut;

    auto model = boundObject->As<RE::TESModel>();
    if (!model)
        return objOut;

	std::vector<CollisionFocusObject> collObjects;
	ObjectOverride clOverride;
	
	if (Settings::GetObjectOverride(object, clOverride) && clOverride.ClObjects.size() > 0)
        collObjects = Util::FindCollisionObjectListByOverride(node, clOverride);
    else
        collObjects = Util::FindCollisionObjectListRecursive(node);

	CollisionFocusObject lastObject;
	float lastPrio = std::numeric_limits<float>::max();

	ObjectOverride objOverride;
	Settings::GetObjectOverride(object, objOverride);

	auto pos = Util::GetObjectAccuratePosition(object);
	auto rot = object->GetAngle();

	glm::vec3 glmRot(rot.x, rot.y, rot.z);

	// objectoverrides aren't specific to an instance of the model, so I need to apply
	// the position and rotation of the object here
	for (auto& customClObject : objOverride.CustomClObjects)
	{
		customClObject.BoundingBox.worldBoundMin = pos + Util::RotateVector(-glmRot, customClObject.BoundingBox.boundMin);
		customClObject.BoundingBox.worldBoundMax = pos + Util::RotateVector(-glmRot, customClObject.BoundingBox.boundMax);

		customClObject.BoundingBox.rotation = customClObject.BoundingBox.rotation - glmRot;

		customClObject.IsValid = true;
		customClObject.HasObjectBound = true;
	}

	collObjects.insert(collObjects.end(), objOverride.CustomClObjects.begin(), objOverride.CustomClObjects.end());

	for (auto &currObj : collObjects)
	{
		float currPrio;
		if (GetCollisionObjectPriority(currObj, currPrio) && currPrio < lastPrio)
		{
			lastPrio = currPrio;
			lastObject = currObj;
		}
	}

	return lastObject;
}

bool FocusManager::GetCollisionObjectPriority(CollisionFocusObject& collisionObject, float& priorityOut)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	glm::vec3 cameraLoc = Util::GetCameraPos();
	glm::quat cameraRotQuat = Util::GetCameraRot();

	// --

	auto niPlayerLoc = playerCharacter->GetPosition();

	glm::vec3 playerLoc(niPlayerLoc.x, niPlayerLoc.y, niPlayerLoc.z + playerCharacter->GetHeight() / 2.0f);
	auto playerAngle = playerCharacter->GetAngle();

	auto targetLoc = Util::GetBoundingBoxCenter(collisionObject);

	// --

	glm::vec3 toTarget = (playerLoc - targetLoc);
	glm::vec3 camToTarget = (targetLoc - cameraLoc);
	glm::vec3 camToTargetN = Util::NormalizeVector(camToTarget);
	glm::vec3 cameraForward = Util::NormalizeVector(Util::GetForwardVector(cameraRotQuat));

	// --

	float distToTarget	= (float)glm::length(toTarget);
	float angleDif		= (float)glm::length(camToTargetN - cameraForward);

	// --

	float distPriority	= distToTarget / GetMaxInteractionRange()	* PRIORITY_DIST_MULT;
	float angleDifPriority = angleDif / GetMaxAngleDif()			* PRIORITY_ANGLE_MULT;

	priorityOut = distPriority + angleDifPriority;
	return true;
}

bool FocusManager::HasSuccessfulRayTrace(FocusObject* focusObj)
{
    if (!focusObj || !focusObj->ObjectRef.get())
        return false;

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter)
		return false;

	ObjectBound objectBound;
	if (focusObj->CollisionObject.HasObjectBound)
		objectBound = focusObj->CollisionObject.BoundingBox;
	else
		Util::GetBoundingBox(focusObj, objectBound);

	auto boundDiagonal = objectBound.boundMax - objectBound.boundMin;

	auto niPlayerPos = playerCharacter->GetPosition();
	glm::vec3 playerPos(niPlayerPos.x, niPlayerPos.y, niPlayerPos.z);

	auto playerHeight = playerCharacter->GetHeight() + CHARACTER_HEIGHT_BONUS;

	auto objectPos = Util::GetBoundingBoxCenter(focusObj);

	// ---

	auto longestAxis = Util::GetBoundsLongestAxis(objectBound);

	// origin of the object, centered on both axis except one, which is set to the bottom
	auto objectPosBottom = Util::GetAxisOrigin(objectBound, longestAxis);

	// full length of the axis determined by GetBoundsLongestAxis
	float axisLength = 0.0f;

	glm::vec3 targetAxis(0.0f, 0.0f, 0.0f);

	switch (longestAxis)
	{
	case EAxis::AxisX:
		targetAxis.x += 1.0f;
		axisLength = abs(boundDiagonal.x);
		break;
	case EAxis::AxisY:
		targetAxis.y += 1.0f;
		axisLength = abs(boundDiagonal.y);
		break;
	case EAxis::AxisZ:
		targetAxis.z += 1.0f;
		axisLength = abs(boundDiagonal.z);
		break;
	}

	// create trace end points along this vector - the longest axis of the bounds
	auto rotatedTargetAxis = Util::RotateVector(objectBound.rotation, targetAxis);

	// ---

	glm::vec3 toObject = playerPos - objectPos;

	auto toObjectEuler = Util::VectorToEulerRotation(toObject);
	auto toObjectRight = Util::GetRightVector(toObjectEuler);

	// perform multiple traces per object
	for (int x = 0; x < NUM_TRACES_TO_OBJECT_HORIZONTAL; x++)
	{
		auto horPercentage = x / (float)NUM_TRACES_TO_OBJECT_HORIZONTAL;
		float horizontalOffset = (TRACES_TOTAL_WIDTH * horPercentage) - (TRACES_TOTAL_WIDTH / 2.0f);

		// create trace start position along the character's width. This is the current trace start offset
		glm::vec3 currentRight(
			toObjectRight.x * horizontalOffset,
			toObjectRight.y * horizontalOffset,
			0.0f);

		for (int y = 0; y < NUM_TRACES_TO_OBJECT_VERTICAL; y++)
		{
			float totalPercentage = (x + y) / (float)(NUM_TRACES_TO_OBJECT_HORIZONTAL + NUM_TRACES_TO_OBJECT_VERTICAL - 2);
			float offsetLength = totalPercentage * axisLength;

			// to create traces along the object's longest axis, multiply rotatedTargetAxis (which is normalized)
			// with the current length from 0 - the max of this longest axis
			glm::vec3 currentAxisOffset(
				rotatedTargetAxis.x * offsetLength,
				rotatedTargetAxis.y * offsetLength,
				rotatedTargetAxis.z * offsetLength
			);

			float vertPercentage = ((y + 1) / (float)NUM_TRACES_TO_OBJECT_VERTICAL);
			float verticalOffsetStart = vertPercentage * playerHeight;

			// construct trace start and end positions from all the gathered data
			RE::NiPoint3 traceStart(playerPos.x + currentRight.x,
				playerPos.y + currentRight.y,
				playerPos.z + verticalOffsetStart);

			RE::NiPoint3 traceEnd(
				objectPosBottom.x + currentAxisOffset.x,
				objectPosBottom.y + currentAxisOffset.y,
				objectPosBottom.z + currentAxisOffset.z);

			const auto result = Util::CrosshairRaycast(playerCharacter, traceStart, traceEnd);

			const auto hitObj = Util::GetRayCastHitRef(result);

			if (!result.rayOutput.HasHit() || (IsPartOfObject(hitObj, focusObj->ObjectRef.get().get())))
			{
				RE::NiPoint3 hitPos = Util::GetRayCastHitPos(result, traceStart, traceEnd);

				if (Settings::Draw_RayTraces)
				{
					glm::vec3 glmTraceStart(traceStart.x, traceStart.y, traceStart.z);
					glm::vec3 glmTraceEnd(traceEnd.x, traceEnd.y, traceEnd.z);
					glm::vec3 glmHitPos(hitPos.x, hitPos.y, hitPos.z);

					if (result.rayOutput.HasHit())
						DebugAPI::DrawLineForMS(glmTraceStart, glmHitPos, 20, glm::vec4(0.0f, 1.0f, 0.0f, 0.50f * Settings::DebugDrawAlpha), Settings::DebugDrawAlpha);
					else
						DebugAPI::DrawLineForMS(glmTraceStart, glmTraceEnd, 20, glm::vec4(0.0f, 1.0f, 0.0f, 0.50f * Settings::DebugDrawAlpha), Settings::DebugDrawAlpha);
				}

				return true;
			}
			else if (Settings::Draw_RayTraces)
			{
				RE::NiPoint3 hitPos = Util::GetRayCastHitPos(result, traceStart, traceEnd);

				glm::vec3 glmTraceStart(traceStart.x, traceStart.y, traceStart.z);
				glm::vec3 glmTraceEnd(traceEnd.x, traceEnd.y, traceEnd.z);
				glm::vec3 glmHitPos(hitPos.x, hitPos.y, hitPos.z);

				if (result.rayOutput.HasHit())
					DebugAPI::DrawLineForMS(glmTraceStart, glmHitPos, 20, glm::vec4(1.0f, 0.9f, 1.0f, 0.2f * Settings::DebugDrawAlpha), Settings::DebugDrawAlpha);
				else
					DebugAPI::DrawLineForMS(glmTraceStart, glmTraceEnd, 20, glm::vec4(1.0f, 0.9f, 1.0f, 0.2f * Settings::DebugDrawAlpha), Settings::DebugDrawAlpha);
			}
		}
	}

	return false;
}
