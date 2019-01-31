#include "WidgetCore.h"

CWidgetCore* CWidgetCore::Instance = nullptr;

CWidgetCore* CWidgetCore::Get()
{
	return Instance;
}

CWidgetCore* CWidgetCore::Init()
{
	if (Instance == nullptr)
	{
		Instance = new CWidgetCore();
	}

	return Instance;
}
void CWidgetCore::Shutdown()
{
	if (Instance != nullptr)
	{
		delete Instance;
	}
}