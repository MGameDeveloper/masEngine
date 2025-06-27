
#include "masDefs.h"

//struct masGameAPI;
bool   masGame_Load(const masChar* GameName/*, masGameAPI* GameAPI*/);
bool   masGame_ReloadOnChanges(/*masGameAPI* GameAPI*/);
void   masGame_UnLoad();


void masGame_Start();
void masGame_Tick();
void masGame_Stop();