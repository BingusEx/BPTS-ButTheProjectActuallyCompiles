class FilterFormType
{
public:
    FilterFormType(RE::FormType formType, bool remove);
    FilterFormType(std::string formTypeName, bool remove);

    RE::FormType FormType;
    bool Remove;

	static RE::FormType MakeFormType(std::string formTypeName);
};
