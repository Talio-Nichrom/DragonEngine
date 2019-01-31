#pragma once

#include "Core/Core.h"

class CInputCore
{
public:

	static CInputCore* Get();
	static CInputCore* Init();
	static void Shutdown();

private:
	static CInputCore* Instance;

};