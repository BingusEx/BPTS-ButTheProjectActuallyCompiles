#pragma once

#include "lib/FocusObject.h"
#include <vector>

class FocusManager
{
private:
	// is custom object selection enabled?
	static bool IsEnabled;

public:
	static bool GetIsEnabled();
	static void SetIsEnabled(bool enabled);

protected:
	// result of previous custom focus selection
	static std::shared_ptr<FocusObject> ActiveFocusObject;

public:
	static std::shared_ptr<FocusObject> GetActiveFocusObject();

	static std::shared_ptr<FocusObject> GetCustomFocusObject(RE::TESObjectREFR* nativeSelection);
	static void OnNativeSelectionChanged(RE::TESObjectREFR* nativeSelection);

	static void MakeSortedObjects(std::vector<RE::TESObjectREFR*> objects);
	static void CheckSortedObjectsOcclusion();
	static bool InitObjectPriority(FocusObject* focusObject);

	// eg. misc objects are less important than weapons
	// returns 0.0f-1.0f
	static float GetObjectTypePriority(FocusObject* focusObject);
	// additional priority
	// returns 0.0f-1.0f
	static float GetSpecificObjectPriority(FocusObject* focusObject);
	// priority based on item value
	// returns 0.0f-1.0f
	static float GetValuePriority(FocusObject* focusObject);

	static CollisionFocusObject GetBestCollisionObject(RE::TESObjectREFR* object);
	// used to determine which collision object to use
	static bool GetCollisionObjectPriority(CollisionFocusObject& collisionObject, float& priorityOut);

	/* returns true if the object has at least one successful ray trace from the player toward various
	* points within the bounds. Performs several traces - expensive! */
	static bool HasSuccessfulRayTrace(FocusObject* object);

	// returns object with lowest priority from last sweep
	static std::shared_ptr<FocusObject> GetBestFocusObject();
	// returns object out of the sorted priority list that is selected via object cycle
	static std::shared_ptr<FocusObject> GetSelectedFocusObject();

	static bool IsPartOfObject(RE::NiAVObject* avObject, RE::TESObjectREFR* object);

	// on skeletal meshes, traces sometimes return a child bone object, and often times those don't
	// have their userData (that is the TESObjectREFR* they belong to) set. To get at that, recursively
	// iterate objects' parent
	static bool FindParentObjectREFR(RE::NiAVObject* avObject, RE::TESObjectREFR* object, int recursionDepth = 0);
	static bool FindChildObjectREFR(RE::NiAVObject* avObject, RE::TESObjectREFR* object, int recursionDepth = 0);

/*	static bool IsPosInBounds(ObjectBound& bound, glm::vec3 position);*/

	static void SetFocusObject(std::shared_ptr<FocusObject> focusObj);
	static void SetFocusObject(RE::TESObjectREFR* focusRef);

	// --

	// same as the above, but always at priority idx 0, to avoid weirdness with priority of last selected object when
	// cycling through objects
	static std::shared_ptr<FocusObject> LastIdealFocusObject;

	// saving last focus object selected with mouse wheel cycle
	static std::shared_ptr<FocusObject> SelectedFocusObject;

	// get index of focus object in prioritized list
	static [[nodiscard]] int GetFocusObjectIdx(std::shared_ptr<FocusObject> focusObj);
	static [[nodiscard]] int GetSelectedPriorityIdx();

	// for mousewheel object cycling. FocusManager remembers, which object is currently cycled to in SelectedFocusObject.
	// This gets the index of it in
	// the table of focusable objects, increases/decreases the index and gets the object at that index in the table as new
	// remembered focus object. This way, after cycling, moving around the camera won't change the focused object anymore,
	// until the object is completely out of focus, or the player cycles back to the object at idx 0
	static void SelectIncreasePriority();
	static void SelectDecreasePriority();

	// result of the native crosshair sweep
	static RE::TESObjectREFR* FocusRef_Orig;

	// all valid objects nearby, sorted by priority in ascending order. Lower priority is better
	static std::vector<std::shared_ptr<FocusObject>> SortedFocusObjects;

	static float GetMaxInteractionRange();
	static float GetMaxAngleDif();

	// don't accept objects with angle difference larger than this
	static float MAX_ANGLE_DIFFERENCE;

	// don't accept objects farther away than this
	static float MAX_INTERACTION_RANGE;

	static float MAX_ANGLE_DIFFERENCE_MOUNTED;
	static float MAX_INTERACTION_RANGE_MOUNTED;

	// determines what value (and above) will return 1.0f in GetValuePriority
	static int32_t MAX_VALUE;

	// priorities by default are around 0.0-1.0. This multiplier determines how much the specific priority
	// contributes to the total priority. Lower values mean less conribution. 0.0f for any of the values for example
	// results in the priority not factoring in at all
	//
	// higher = more important
	static float PRIORITY_ANGLE_MULT;
	static float PRIORITY_DIST_MULT;
	static float PRIORITY_TYPE_MULT;
	static float PRIORITY_SPECIFIC_MULT;
	static float PRIORITY_VALUE_MULT;

	// to allow picking up objects on high shelves, this adds a bit to the height when performing traces.
	// In practice, this increases the vertical distance between traces, but also the height of the
	// highest trace. So don't set this too high, or you can pick up items through the ceiling
	static float CHARACTER_HEIGHT_BONUS;

	// native crosshair selection object gets a bonus to priority if it exists. Settings this very high
	// means that native selection is always preferred
	static float NATIVE_SELECTION_PRIORITY_BONUS;

	// whatever was selected previously gets a slight bonus to priority to avoid "flickering", where through
	// small changes in the camera position, while pointing right at the border between two objects, the
	// focus quickly changes between the two. The smaller this value, the "stickier" the focus
	static float PREVIOUS_SELECTION_PRIORITY_BONUS;

	// allows giving NPCs a bonus to priority, making them easier to select next to other activatables.
	// useful to make talking to shopkeepers easier for example
	static float NPC_SELECTION_PRIORITY_BONUS;

	// total traces to perform per otherwise valid object is 
	// NUM_TRACES_TO_OBJECT_HORIZOTAL * NUM_TRACES_TO_OBJECT_VERTICAL
	static int NUM_TRACES_TO_OBJECT_HORIZONTAL;
	static int NUM_TRACES_TO_OBJECT_VERTICAL;

	// amount of space all NUM_TRACES_TO_OBJECT_HORIZOTAL take up. Vertically, character height is used
	static float TRACES_TOTAL_WIDTH;
};
