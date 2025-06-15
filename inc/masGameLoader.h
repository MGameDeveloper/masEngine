
#define MAS_GAME_INIT_FUNC_NAME "masInit"
#define MAS_GAME_TICK_FUNC_NAME "masTick"

typedef bool(*masGameInitFunc)();
typedef void(*masGameTickFunc)();

struct masGame;

masGame* masGame_Load(const char* GameName);
void masGame_ReloadOnChanges(masGame** GameRef);
bool masGame_Init(masGame* Game);
void masGame_Tick(masGame* Game);