#pragma once

#include "Core/Core.h"

class CEngine
{
public:

	static CEngine* Get();
	static CEngine* Init();
	static void Shutdown();

private:
	static CEngine* Instance;
};