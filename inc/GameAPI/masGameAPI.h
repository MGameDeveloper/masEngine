

#define MAS_API extern "C" __declspec(dllexport)


typedef bool(*masInitFunc)();
typedef void(*masDeInitFunc)();
typedef void(*masTickFunc)();


struct masGameAPI
{
	const char *Name;
	const char *AssetPath;
	int32_t     Width;
	int32_t     Height;
	int32_t     Version;
	
	masInitFunc   masInit;
	masDeInitFunc masDeInit;
	masTickFunc   masTick;
};