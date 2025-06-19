class BTPS_API
{
public:
	
	static BTPS_API* GetSingleton();

	virtual bool SelectionEnabled();
    virtual bool Widget3DEnabled();

	virtual void HideSelectionWidget(std::string source);
    virtual void ShowSelectionWidget(std::string source);

	virtual void GetSelectionWidgetPos2D(float& x, float &y);
	virtual void GetSelectionWidgetPos3D(double& x, double &y, double &z);

	virtual void StartFadeIn();
	virtual void StartFadeOut();

	virtual void FadeOutInstant();
	virtual void FadeInInstant();
};
