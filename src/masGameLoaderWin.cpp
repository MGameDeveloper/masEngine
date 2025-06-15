#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "masGameLoader.h"
#include "masDefs.h"


struct masGame
{
    char            *Dir;
    HMODULE          DLL;
    masGameInitFunc  InitFunc;
    masGameTickFunc  TickFunc;
};



/*************************************************************************
*
**************************************************************************/
static bool masGame_Compile(const char* GameDir)
{
    const char* GameBuildTemplateFile = "GameBuildTemplate.bat";
    if(!PathFileExistsA(GameBuildTemplateFile))
    {
        MAS_LOG_ERROR("%s not found\n", GameBuildTemplateFile);
        return false;
    }
    
    char    GameBuildPath[256] = {};
    int32_t GameBuildPathLen   = sprintf(GameBuildPath, "%s/Build.bat", GameDir);
    if(!CopyFileA(GameBuildTemplateFile, GameBuildPath, false))
    {
        MAS_LOG_ERROR("Copying %s to %s\n", GameBuildTemplateFile, GameBuildPath);
        return false;
    }  
   
    char RunGameBuild[256] = {};
    sprintf(RunGameBuild, "call %s", GameBuildPath);
    printf("GAME_BUILD_COMMAND: %s\n", RunGameBuild);

    //
    int32_t CmdRes = system(RunGameBuild);
    if(CmdRes != 0)
    {
        MAS_LOG_ERROR("Wasn't able to compile the game [ %s ]\n", GameDir);
        return false;
    }

    return true;
}

static bool masGameInternal_Recompile()
{
    return true;
}


/*************************************************************************
*
**************************************************************************/
masGame* masGame_Load(const char* GameName)
{
    char GameDir[256] = {};
    sprintf(GameDir, "../Projects/%s", GameName);
    if(!PathFileExistsA(GameDir))
    {
        MAS_LOG_ERROR("No directory found [ %s ]\n", GameDir);
        return false;
    }
    

    //
    char    GamePath[256] = {};
    int32_t GamePathLen   = sprintf(GamePath, "%s/build/masGame.dll", GameDir);
    HMODULE GameDLL       = LoadLibraryA(GamePath);
    if(!GameDLL)
    {
        if(!masGame_Compile(GameDir))
        {
            MAS_LOG_ERROR("Compiling [ %s ]\n", GameDir);
            return NULL;
        }
        else
            MAS_LOG_INFO("Compiling... [ %s ]\n", GameDir);
    }  
    MAS_LOG_INFO("Loading... [ %s ]\n", GamePath);
          

    //
    int32_t   Len     = (int32_t)strlen(GameDir) + 1; // NULL Terminator
    uint64_t  MemSize = (sizeof(masGame) + (sizeof(char) * Len));
    masGame  *Game    = (masGame*)malloc(MemSize);
    if(!Game)
        return NULL;
    else
    {
        memset(Game, 0, MemSize);
        Game->Dir = MAS_ADDR_FROM(char, Game, sizeof(masGame));
    }


    //
    memcpy(Game->Dir, GameDir, sizeof(char) * (Len - 1));
    Game->DLL = GameDLL;
    Game->InitFunc = (masGameInitFunc)GetProcAddress(Game->DLL, MAS_GAME_INIT_FUNC_NAME);
    Game->TickFunc = (masGameTickFunc)GetProcAddress(Game->DLL, MAS_GAME_TICK_FUNC_NAME);


    //
    if(!Game->InitFunc || !Game->TickFunc)
    {
        MAS_LOG_ERROR("GetProcAddress for ( %s or %s)\n", 
            MAS_GAME_INIT_FUNC_NAME, 
            MAS_GAME_TICK_FUNC_NAME);

        FreeLibrary(Game->DLL);
        free(Game);
        Game = NULL;
        return NULL;
    }

    printf("%s: %s\n", MAS_FUNC_NAME, GameName);
    printf("    GameDir:  %s\n",   Game->Dir);
    printf("    Handle:   0x%p\n", Game->DLL);
    printf("    InitFunc: 0x%p\n", Game->InitFunc);
    printf("    TickFunc: 0x%p\n", Game->TickFunc);

    return Game;
}

void masGame_ReloadOnChanges(masGame** GameRef)
{
    if(!GameRef || !(*GameRef))
        return;

    masGame* Game = *GameRef;

    HANDLE ChangeHandle = FindFirstChangeNotificationA(Game->Dir, TRUE, 
        FILE_NOTIFY_CHANGE_LAST_WRITE |  
        FILE_NOTIFY_CHANGE_DIR_NAME   | 
        FILE_NOTIFY_CHANGE_FILE_NAME);

    if(ChangeHandle == INVALID_HANDLE_VALUE || !ChangeHandle)
        return;
    
    DWORD WaitState;
    while(1)
    {
        WaitState = WaitForSingleObject(ChangeHandle, INFINITE);
        switch(WaitState)
        {
        case WAIT_OBJECT_0:
            if(!masGameInternal_Recompile())
            {
                MAS_LOG_ERROR("Recompile %s when directory changes\n", Game->Dir);
            }

            if(!FindNextChangeNotification(ChangeHandle))
            {
                DWORD Error = GetLastError();
                MAS_LOG_ERROR("FindNextChangeNotification [ %u ] error code\n", Error);
            }
            break;
        }
    }
}

bool masGame_Init(masGame* Game)
{
    if(!Game || !Game->InitFunc)
        return false;

    return Game->InitFunc();
}

void masGame_Tick(masGame* Game)
{
    if(!Game || !Game->TickFunc)
        return;

    Game->TickFunc();
}
