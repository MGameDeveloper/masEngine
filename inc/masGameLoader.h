
//#define MAS_GAME_INIT_FUNC_NAME   "masInit"
//#define MAS_GAME_DEINIT_FUNC_NAME "masDeInit"
//#define MAS_GAME_TICK_FUNC_NAME   "masTick"
//
//
//typedef bool(*masGameInitFunc)();
//typedef void(*masGameDeInitFunc)();
//typedef void(*masGameTickFunc)();

struct masGameAPI;

bool   masGame_Load(const char* GameName, masGameAPI* GameAPI);
void   masGame_UnLoad();
//struct masGame* masGame_GetAPI();

//bool masGame_Init();
//void masGame_Tick();
