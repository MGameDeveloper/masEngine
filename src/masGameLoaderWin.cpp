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
	HANDLE           Handle;
	HANDLE           ReloadEvent;
	CRITICAL_SECTION CriticalSection;
	DWORD            Id;
};

struct masGame
{
	masChangesMonitorThread MonitorThread;
    char            *Dir;
	char            *Name;
    HMODULE          DLL;
};
static masGame *Game = NULL;


/*************************************************************************
*
**************************************************************************/
static bool masGame_Compile(const char* GameDir, const char* BuildFolder)
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
	    	CloseHandle(GameBuildFile);
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
    sprintf(BuildGame, "\"%s %s\"", GameBuildPath, BuildFolder);
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

static bool masGameInternal_Recompile(masGame* Game)
{
	// 
	MAS_LOG_INFO("RE_COMPILE: %s\n", Game->Dir);
	
	if(!masGame_Compile(Game->Dir, "NewBuild"))
	{
		MAS_LOG_ERROR("Rebuilding game dll %s\n", Game->Dir);
		return false;
	}	
	
	EnterCriticalSection(&Game->MonitorThread.CriticalSection);
	SetEvent(Game->MonitorThread.ReloadEvent);
	LeaveCriticalSection(&Game->MonitorThread.CriticalSection);
	MAS_LOG_INFO("RE COMPILE SUCCESS\n");
    return true;
}

bool masGameInternal_ShouldReloadOnFile(const wchar_t* FilePath, const wchar_t** Ext, uint32_t ExtCount)
{
	const wchar_t* FileExt = wcsrchr(FilePath, L'.');
	//printf("\nFileExt: %ls\n", FileExt);
	if(!FileExt)
		return false;
	
	for(int32_t i = 0; i < ExtCount; ++i)
	{
		if(_wcsicmp(FileExt, Ext[i]) == 0)
			return true;
	}
	
	return false;
}

DWORD WINAPI masGameInternal_MonitorAndCompileOnChanges(LPVOID Param)
{
	masGame* pGame = (masGame*)Param;
	if(!pGame)
	{
		MAS_LOG_ERROR("Param is NULL");
		return -1;
	}
	
	HANDLE DirHandle = CreateFileA(pGame->Dir, FILE_LIST_DIRECTORY,
	    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if(DirHandle == INVALID_HANDLE_VALUE)
	{
		MAS_LOG_ERROR("Opengin directory %s to wach for changes\n", pGame->Dir);
		return false;
	}
	
	const uint32_t BufferSize   = 4096;
	DWORD          ByteReturned = 0;
	BYTE Buffer[BufferSize]     = {};
	OVERLAPPED Overlapped       = {};
	Overlapped.hEvent           = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	const wchar_t* Extensions[] = 
	{
		L".cpp", L".h"
	};
	uint32_t ExtCount = sizeof(Extensions)/sizeof(Extensions[0]);
	wchar_t FilePath[MAX_PATH] = {};
	
	while(TRUE)
	{
		bool bReadChange = ReadDirectoryChangesW(DirHandle, Buffer, BufferSize,
            TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &ByteReturned, &Overlapped, NULL);
        if(bReadChange)
		{
			DWORD WaitResult = WaitForSingleObject(Overlapped.hEvent, INFINITE);
			if(WaitResult == WAIT_OBJECT_0)
			{
				FILE_NOTIFY_INFORMATION* NotifyInfo = (FILE_NOTIFY_INFORMATION*)Buffer;
				do
				{
					if(NotifyInfo->Action == FILE_ACTION_MODIFIED)
					{
						memcpy(FilePath, NotifyInfo->FileName, NotifyInfo->FileNameLength);
						//printf("CHANGES[ %u ]: %ls\n", NotifyInfo->Action, FilePath); 
						if(masGameInternal_ShouldReloadOnFile(FilePath, Extensions, ExtCount))
						{
							if(!masGameInternal_Recompile(pGame))
								MAS_LOG_ERROR("Compiling Game[ %s ] on changes\n", pGame->Dir);
							else
								MAS_LOG_INFO("COMPILE_ON_CHANGES_SUCCESS\n");
						}
						memset(FilePath, 0, sizeof(wchar_t) * MAX_PATH);
					}
					
					if(NotifyInfo->NextEntryOffset == 0)
						break;
					NotifyInfo = MAS_ADDR_FROM(FILE_NOTIFY_INFORMATION, NotifyInfo, NotifyInfo->NextEntryOffset);
				}while(TRUE);
			}
		}		
	}
	
	CloseHandle(Overlapped.hEvent);
	CloseHandle(DirHandle);
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
        if(!masGame_Compile(GameDir, "Build"))
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
	HANDLE ThreadHandle = CreateThread(NULL, 0, 
	    masGameInternal_MonitorAndCompileOnChanges, (LPVOID)Game, 0, &ThreadId);
    if(ThreadHandle == INVALID_HANDLE_VALUE)
	{
		printf("Creating Thread to monitor game directory for changes\n");
		masGame_UnLoad();
		return false;
	}
	
	Game->MonitorThread.Handle      = ThreadHandle;
	Game->MonitorThread.Id          = ThreadId;
	Game->MonitorThread.ReloadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&Game->MonitorThread.CriticalSection);
	
	// Log
    printf("%s: %s\n", MAS_FUNC_NAME, GameName);
	printf("    MonitorThread:\n");
	printf("        -Handle: 0x%p\n", Game->MonitorThread.Handle);
	printf("        -Id    : %u\n", Game->MonitorThread.Id);
    printf("    GameDir: %s\n",   Game->Dir);
    printf("    Handle:    0x%p\n", Game->DLL);

    return true;
}

bool masGame_ReloadOnChanges(masGameAPI* GameAPI)
{
	bool Ret = false;
	
	EnterCriticalSection(&Game->MonitorThread.CriticalSection);
	if(WaitForSingleObject(Game->MonitorThread.ReloadEvent, 0) == WAIT_OBJECT_0)
	{
	    FreeLibrary(Game->DLL);
        Game->DLL = NULL;
		MAS_LOG_INFO("DLL UNLOADED\n");
		
        //
		char SwapCmd[MAX_PATH] = {};
		int32_t SwapCmdLen = sprintf(SwapCmd,
		    "cmd /c "
			"\""
			"cd /d \"%s\" && "
			"(if exist Build ren Build OldBuild) && "
			"(if exist NewBuild ren NewBuild Build) && "
			"(if exist OldBuild rmdir /s /q OldBuild) "
			"\"", Game->Dir);
		
		int32_t SwapCmdRes = system(SwapCmd);
		if(SwapCmdRes == -1)
			MAS_LOG_ERROR("Could not remove OldBuild win32_error %u\n", GetLastError());
		else
			MAS_LOG_INFO("Removed OldBuild\n");
		MAS_LOG_INFO("SWAP_CMD_RES: %d\n", SwapCmdRes);
		
		//
		char DLLPath[MAX_PATH] = {};
		sprintf(DLLPath, "%s\\Build\\masGame.dll", Game->Dir);
		MAS_LOG_INFO("DLL_PATH: %s\n", DLLPath);
		HMODULE GameDLL = LoadLibraryA(DLLPath);
		if(!GameDLL)
			MAS_LOG_ERROR("LOADING NEW GAME DLL FAILED\n");
		else
		{
			Game->DLL = GameDLL;
			const char* FuncName = "masGame_GetAPI";
            masGame_GetAPI GetGameAPI = (masGame_GetAPI)GetProcAddress(Game->DLL, FuncName);
	        if(!GetGameAPI)
	        	MAS_LOG_ERROR("LOADING masEngine_GetGameAPI");
			else
				GetGameAPI(GameAPI);
		}
		
		MAS_LOG_INFO("DLL LOADED\n");
		
		ResetEvent(Game->MonitorThread.ReloadEvent);	
		Ret = true;
	}
	LeaveCriticalSection(&Game->MonitorThread.CriticalSection);
	
	return Ret;
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