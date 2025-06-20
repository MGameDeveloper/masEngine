#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "masGameLoader.h"
#include "masDefs.h"

// 
MAS_FUNC_TYPE(void, masGame_GetAPI, masGameAPI* GameAPI);


/*************************************************************************
*
**************************************************************************/
struct masChangesMonitorThread
{
	HANDLE Handle;
	DWORD  Id;
	CRITICAL_SECTION CriticalSection;
	bool bReadyToSwapDLL;
};

struct masGame
{
	masChangesMonitorThread MonitorThread;
    char            *Dir;
	char            *Name;
    HMODULE          DLL;
    //masGameInitFunc  InitFunc;
    //masGameTickFunc  TickFunc;
	//masGameDeInitFunc DeInitFunc;
};
static masGame *Game = NULL;


/*************************************************************************
*
**************************************************************************/
static bool masGame_Compile(const char* GameDir)
{
	DWORD CwdLen = 0;
	char Cwd[256] = {};
	GetCurrentDirectoryA(256, Cwd);
	
	
	// Check if BuildGameTemplate exists
    const char* GameBuildTemplate = "BuildGameTemplate.bat";
    if(!PathFileExistsA(GameBuildTemplate))
    {
        MAS_LOG_ERROR("%s not found\n", GameBuildTemplate);
        return false;
    }
	
	
	// Create Build.bat in Game directory if not exists
	char    GameBuildPath[256] = {};
    int32_t GameBuildPathLen   = sprintf(GameBuildPath, "%s\\%s\\Build.bat", Cwd, GameDir);
	if(!PathFileExistsA(GameBuildPath))
	{
		HANDLE GameBuildFile = CreateFileA(GameBuildPath, GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	    if(GameBuildFile == INVALID_HANDLE_VALUE)
	    {
	    	MAS_LOG_ERROR("WIN32_ERROR_CODE[ %u ] - Creating Build.bat for %s\n", GetLastError(), GameDir);
	    	return false;
	    }
	    CloseHandle(GameBuildFile);
	}
 
	
    // Copy BuiltGameTemplate Content to the created above in Game Directory 
	if(!CopyFileA(GameBuildTemplate, GameBuildPath, FALSE))
	{
		MAS_LOG_ERROR("Copying %s to [ %s ]: %s\n", GameBuildTemplate, GameDir, "Build.bat");
		return false;
	}
	
    char BuildGame[256] = {};
    sprintf(BuildGame, "call \"%s\"", GameBuildPath);
    printf("GAME_BUILD_COMMAND: %s\n", BuildGame);

    //
    int32_t CmdRes = system(BuildGame);
    if(CmdRes != 0)
    {
        MAS_LOG_ERROR("Wasn't able to compile the game [ %s ]\n", GameDir);
        return false;
    }

    return true;
}

static bool masGameInternal_Recompile()
{
	MAS_LOG_INFO("RE_COMPILE\n");
    return true;
}

DWORD WINAPI masGameInternal_MonitorAndCompileOnChanges(LPVOID Param)
{
	masGame* pGame = (masGame*)Param;
	
	while(TRUE)
	{
		
	}
}

/*************************************************************************
*
**************************************************************************/
bool masGame_Load(const char* GameName, masGameAPI* GameAPI)
{
    char GameDir[256] = {};
    sprintf(GameDir, "..\\Projects\\%s", GameName);
    if(!PathFileExistsA(GameDir))
    {
        MAS_LOG_ERROR("No directory found [ %s ]\n", GameDir);
        return false;
    }
    

    //
    char    GamePath[256] = {};
    int32_t GamePathLen   = sprintf(GamePath, "%s\\Build\\masGame.dll", GameDir);
    HMODULE GameDLL       = LoadLibraryA(GamePath);
    if(!GameDLL)
    {
        if(!masGame_Compile(GameDir))
        {
            MAS_LOG_ERROR("Compiling [ %s ]\n", GameDir);
            return false;
        }
        else
		{
			MAS_LOG_INFO("Compiled [ %s ]\n", GameDir);		
			MAS_LOG_INFO("Loading... [ %s ]\n", GamePath);
			GameDLL = LoadLibraryA(GamePath);
			if(!GameDLL)
			{
				MAS_LOG_ERROR("Loading Failed [ %s ]\n", GameDir);
				return false;
			}
			MAS_LOG_INFO("Loaded [ %s ]\n", GameDir);
		}
    }  
    
          

    //
    int32_t   Len     = (int32_t)strlen(GameDir) + 1; // NULL Terminator
    uint64_t  MemSize = (sizeof(masGame) + (sizeof(char) * Len));
    Game = (masGame*)malloc(MemSize);
    if(!Game)
        return NULL;

    //
    memset(Game, 0, MemSize);
    Game->Dir = MAS_ADDR_FROM(char, Game, sizeof(masGame));


    //
    memcpy(Game->Dir, GameDir, sizeof(char) * (Len - 1));
    Game->DLL        = GameDLL;
	
	
	//
	const char* FuncName = "masGame_GetAPI";
    masGame_GetAPI GetGameAPI = (masGame_GetAPI)GetProcAddress(Game->DLL, FuncName);
	if(!GetGameAPI)
	{
		MAS_LOG_ERROR("LOADING masEngine_GetGameAPI");
		return false;
	}
	
	// 
	GetGameAPI(GameAPI);


    // Spawn thread
    DWORD  ThreadId = 0;
	HANDLE ThreadHandle = CreateThread(NULL, 0, masGameInternal_MonitorAndCompileOnChanges, (LPVOID)Game, 0, &ThreadId);
    if(ThreadHandle == INVALID_HANDLE_VALUE)
	{
		printf("Creating Thread to monitor game directory for changes\n");
		masGame_UnLoad();
		return false;
	}
	
	Game->MonitorThread.Handle = ThreadHandle;
	Game->MonitorThread.Id     = ThreadId;
	
	
	    // Log
    printf("%s: %s\n", MAS_FUNC_NAME, GameName);
	printf("    MonitorThread:\n");
	printf("        -Handle: 0x%p\n", Game->MonitorThread.Handle);
	printf("        -Id    : %u\n", Game->MonitorThread.Id);
    printf("    GameDir: %s\n",   Game->Dir);
    printf("    Handle:    0x%p\n", Game->DLL);

    return true;
}


void masGame_UnLoad()
{
	if(Game)
	{
		//if(Game->DeInitFunc)
		//	Game->DeInitFunc();
		
		FreeLibrary(Game->DLL);
		free(Game);
		Game = NULL;
	};
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


//bool masGame_Init()
//{
//    if(!Game || !Game->InitFunc)
//        return false;
//
//    return Game->InitFunc();
//}
//
//void masGame_Tick()
//{
//    if(!Game || !Game->TickFunc)
//        return;
//
//    Game->TickFunc();
//}





