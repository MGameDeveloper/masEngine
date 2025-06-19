
#define MAS_GAME_INIT_FUNC_NAME   "masInit"
#define MAS_GAME_DEINIT_FUNC_NAME "masDeInit"
#define MAS_GAME_TICK_FUNC_NAME   "masTick"


typedef bool(*masGameInitFunc)();
typedef void(*masGameDeInitFunc)();
typedef void(*masGameTickFunc)();


bool masGame_Load(const char* GameName);
void masGame_UnLoad();


bool masGame_Init();
void masGame_Tick();
