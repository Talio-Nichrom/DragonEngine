#pragma onceCWidgetCore

#include "Core/Core.h"

class CWidgetCore
{
public:

	static CWidgetCore* Get();
	static CWidgetCore* Init();
	static void Shutdown();

private:
	static CWidgetCore* Instance;

};