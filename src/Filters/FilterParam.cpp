#include "FilterParam.h"

#include "lib/Util.h"

FilterParam::FilterParam(std::string sValue)
{
	CacheValues(sValue);
}

void FilterParam::SetValue(bool value)
{
	BoolValue = value;
}

void FilterParam::SetValue(int value)
{
	IntValue = value;
}

void FilterParam::SetValue(float value)
{
	FloatValue = value;
}

void FilterParam::SetValue(std::string value)
{
	StringValue = value;
}

bool FilterParam::GetValue(bool& valueOut)
{
	if (!BoolValue)
		return false;

	valueOut = BoolValue.value();
	return true;
}

bool FilterParam::GetValue(int& valueOut)
{
	if (!IntValue)
		return false;

	valueOut = IntValue.value();
	return true;
}

bool FilterParam::GetValue(float& valueOut)
{
	if (!FloatValue)
		return false;

	valueOut = FloatValue.value();
	return true;
}

bool FilterParam::GetValue(std::string& valueOut)
{
	if (!StringValue)
		return false;

	valueOut = StringValue.value();
	return true;
}

void FilterParam::CacheValues(std::string sValue)
{
	TrySetValue_Bool(sValue);
	TrySetValue_Int(sValue);
	TrySetValue_Float(sValue);
	TrySetValue_String(sValue);
}

bool FilterParam::TrySetValue_Bool(std::string sValue)
{
	std::string sParamLower = sValue;
	Util::ToLowerString(sParamLower);

	if (sValue == "true")
		return true;
	if (sValue == "false")
		return false;

	try
    {
        BoolValue = (bool)std::stoi(sValue);
    }
    catch (std::exception e)
    {
        return false;
    }

	return true;
}

bool FilterParam::TrySetValue_Int(std::string sValue)
{
	bool isHex = sValue.find("0x") == 0;

	if (isHex)
    {
		sValue.erase(0, 2);

		try
		{
			IntValue = std::stoi(sValue, nullptr, 16);
		}
		catch (std::exception e)
		{
			return false;
		}
    }
    else
	{
        try
		{
			IntValue = std::stoi(sValue);
		}
		catch (std::exception e)
		{
			return false;
		}
	}

	return true;
}

bool FilterParam::TrySetValue_Float(std::string sValue)
{
	try
    {
        FloatValue = std::stof(sValue);
    }
    catch (std::exception e)
    {
        return false;
    }

	return true;
}

void FilterParam::TrySetValue_String(std::string sValue)
{
	StringValue = sValue;
}
