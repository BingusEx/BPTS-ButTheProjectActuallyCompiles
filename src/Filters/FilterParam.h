#pragma once

class FilterParam
{
public:

	FilterParam(std::string sValue);

	std::optional<bool> BoolValue;
	std::optional<int> IntValue;
	std::optional<float> FloatValue;
	std::optional<std::string> StringValue;

	void SetValue(bool value);
	void SetValue(int value);
	void SetValue(float value);
	void SetValue(std::string value);

	bool GetValue(bool& valueOut);
	bool GetValue(int& valueOut);
	bool GetValue(float& valueOut);
	bool GetValue(std::string& valueOut);

	void CacheValues(std::string sValue);

	bool TrySetValue_Bool(std::string sValue);
	bool TrySetValue_Int(std::string sValue);
	bool TrySetValue_Float(std::string sValue);
	void TrySetValue_String(std::string sValue);
};
