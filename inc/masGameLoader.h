

struct masGameAPI;
bool   masGame_Load(const char* GameName, masGameAPI* GameAPI);
bool   masGame_ReloadOnChanges(masGameAPI* GameAPI);
void   masGame_UnLoad();

