#ifdef _WIN32
#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#include "EffectPlatformGL.h"
#elif defined(__APPLE__)
#include "EffectPlatformGL.h"
#else
#include "EffectPlatformGL.h"
#endif

#include "../TestHelper.h"

void BasicRuntimeTestPlatform(EffectPlatform* platform, std::string baseResultPath)
{
	srand(0);

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

	for (size_t i = 0; i < 30; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot((baseResultPath + "SimpleLaser.png").c_str());
}

void BasicRuntimeTest()
{
#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		BasicRuntimeTestPlatform(platform.get(), "DX11_");
	}

#elif defined(__APPLE__)

#else

#endif
}