#include "InputCore.h"

CInputCore* CInputCore::Instance = nullptr;

CInputCore* CInputCore::Get()
{
	return Instance;
}

CInputCore* CInputCore::Init()
{
	if (Instance == nullptr)
	{
		Instance = new CInputCore();
	}

	return Instance;
}
void CInputCore::Shutdown()
{
	if (Instance != nullptr)
	{
		delete Instance;
	}
}