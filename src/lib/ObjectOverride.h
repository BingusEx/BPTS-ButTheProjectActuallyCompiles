#pragma once

#include "ObjectBound.h"
#include "FocusObject.h"

class ClObjectOverride
{
public:
    std::vector<std::string> Path;
};

class ObjectOverride
{
public:
    enum WidgetPos
    {
        Auto,  // default: depends on object size
        Center,
        Top,
        Bottom,
        Root
    };

	enum EPriorityModifier
	{
		None,
		Add,
		Subtract,
		Multiply,
		Divide
	};

    std::string ModelName;
	bool HasModelName = false;

	std::string DisplayName;
    bool HasDisplayName = false;

    std::vector<ClObjectOverride> ClObjects;
	WidgetPos WidgetBasePos = WidgetPos::Auto;

	glm::vec3 WidgetPosOffset;
    bool HasWidgetPosOffset = false;

	glm::vec3 WidgetPosOffsetRel;
    bool HasWidgetPosOffsetRel = false;

	EPriorityModifier PriorityModifier = EPriorityModifier::None;
	bool HasPriorityModifier = false;

	double PriorityValue;

	std::vector<CollisionFocusObject> CustomClObjects;

	static EPriorityModifier MakePriorityModifier(std::string effectName);
	void ApplyPriorityModifier(double &valueIn);
	void ApplyPriorityModifier(float &valueIn);
};
