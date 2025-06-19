#include "BTPS_API.h"

#include "FocusManager.h"
#include "Settings.h"
#include "UI/SelectionWidget.h"

std::mutex m_API;

BTPS_API* BTPS_API::GetSingleton()
{
	std::lock_guard lock(m_API);

	static BTPS_API singleton;
	return std::addressof(singleton);
}

bool BTPS_API::SelectionEnabled()
{
	std::lock_guard lock(m_API);
	return FocusManager::GetIsEnabled();
}

bool BTPS_API::Widget3DEnabled()
{
	std::lock_guard lock(m_API);
	return Settings::GetWidget3DEnabled();
}

void BTPS_API::HideSelectionWidget(const std::string source)
{
	std::lock_guard lock(m_API);
	SelectionWidgetMenu::Hide(source);
}

void BTPS_API::ShowSelectionWidget(const std::string source)
{
	std::lock_guard lock(m_API);
	SelectionWidgetMenu::Show(source);
}

void BTPS_API::GetSelectionWidgetPos2D(float& x, float& y)
{
	std::lock_guard lock(m_API);

	glm::vec2 widgetPos = SelectionWidget::GetWidgetPos();
	x = widgetPos.x;
	y = widgetPos.y;
}

void BTPS_API::GetSelectionWidgetPos3D(double& x, double& y, double& z)
{
	std::lock_guard lock(m_API);

	std::optional<glm::dvec3> widgetPos = SelectionWidget::LastWidgetPos;
	if (widgetPos.has_value())
	{
		x = widgetPos.value().x;
		y = widgetPos.value().y;
		z = widgetPos.value().z;
	}
}

void BTPS_API::StartFadeIn()
{
	std::lock_guard lock(m_API);
	SelectionWidget::StartFadeIn();
}

void BTPS_API::StartFadeOut()
{
	std::lock_guard lock(m_API);
	SelectionWidget::StartFadeOut();
}

void BTPS_API::FadeOutInstant()
{
	std::lock_guard lock(m_API);
	SelectionWidget::FadeOutInstant();
}

void BTPS_API::FadeInInstant()
{
	std::lock_guard lock(m_API);
	SelectionWidget::FadeInInstant();
}
