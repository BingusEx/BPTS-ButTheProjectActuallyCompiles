#include "ObjectOverride.h"

#include "Util.h"

ObjectOverride::EPriorityModifier ObjectOverride::MakePriorityModifier(std::string effectName)
{
	Util::ToLowerString(effectName);

	if (effectName == "add")
		return EPriorityModifier::Add;
	if (effectName == "subtract")
		return EPriorityModifier::Subtract;
	if (effectName == "multiply" || effectName == "mult")
		return EPriorityModifier::Multiply;
	if (effectName == "divide" || effectName == "div")
		return EPriorityModifier::Divide;
	
	return EPriorityModifier::None;
}

void ObjectOverride::ApplyPriorityModifier(double & valueIn)
{
	switch (PriorityModifier)
	{
		case EPriorityModifier::Add:
			valueIn += PriorityValue;
			return;
		case EPriorityModifier::Subtract:
			valueIn -= PriorityValue;
			return;
		case EPriorityModifier::Multiply:
			valueIn *= PriorityValue;
			return;
		case EPriorityModifier::Divide:
			valueIn /= PriorityValue;
			return;
	}
}

void ObjectOverride::ApplyPriorityModifier(float & valueIn)
{
	switch (PriorityModifier)
	{
		case EPriorityModifier::Add:
			valueIn += (float)PriorityValue;
			return;
		case EPriorityModifier::Subtract:
			valueIn -= (float)PriorityValue;
			return;
		case EPriorityModifier::Multiply:
			valueIn *= (float)PriorityValue;
			return;
		case EPriorityModifier::Divide:
			valueIn /= (float)PriorityValue;
			return;
	}
}
