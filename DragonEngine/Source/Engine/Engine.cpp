#include "Engine.h"

CEngine* CEngine::Instance = nullptr;

CEngine* CEngine::Get()
{
	return Instance;
}

CEngine* CEngine::Init()
{
	if (Instance == nullptr)
	{
		Instance = new CEngine();
	}

	return Instance;
}

void CEngine::Shutdown()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}