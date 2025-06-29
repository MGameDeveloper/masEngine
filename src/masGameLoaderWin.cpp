#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <tchar.h>
#include "masGameLoader.h"


#define MAS_GET_PROC_ADDRESS(DLL, FUNC_TYPE) (FUNC_TYPE)GetProcAddress(DLL, #FUNC_TYPE)

#define MAS_CHECK_PROC_ADDRESS(FUNC_TYPE, FUNC_VAR)                                                    \
    if(FUNC_VAR == NULL)                                                                               \
	{                                                                                                  \
	    MAS_LOG_ERROR("%Ts = NULL; [ %Ts ] address couldn't be found", _T(#FUNC_VAR), _T(#FUNC_TYPE)); \
	    return false;                                                                                  \
	}


/*************************************************************************
*
**************************************************************************/
MAS_FUNC_TYPE(bool, masStart);
MAS_FUNC_TYPE(void, masTick);
MAS_FUNC_TYPE(void, masStop);


/*************************************************************************
*
**************************************************************************/
struct masMonitorThread
{
	HANDLE           Handle;
	HANDLE           ReloadEvent;
	CRITICAL_SECTION CriticalSection;
	DWORD            Id;
};

struct masGame
{
	masMonitorThread MonitorThread;
    TCHAR    Dir[MAX_PATH];
	TCHAR    Name[32];
    HMODULE  DLL;
	masStart masStartFunc;
	masTick  masTickFunc;
	masStop  masStopFunc;
};
static masGame Game = {};


/*************************************************************************
*
**************************************************************************/
static bool masGame_Compile(const TCHAR* BuildFolder)
{
    const TCHAR* GameBuildTemplate = _T("BuildGameTemplate.bat");
    if(!PathFileExists(GameBuildTemplate))
    {
        MAS_LOG_ERROR("%Ts not found\n", GameBuildTemplate);
        return false;
    }	

	TCHAR GameBuildPath[MAX_PATH] = {};
    uint32_t GameBuildPathLen  = _stprintf(GameBuildPath, _T("%Ts\\Build.bat"), Game.Dir);
	if(!PathFileExists(GameBuildPath))
	{
		HANDLE GameBuildFile = CreateFile(GameBuildPath, GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	    if(GameBuildFile == INVALID_HANDLE_VALUE)
	    {
	    	MAS_LOG_ERROR("WIN32_ERROR_CODE[ %u ] - Creating Build.bat for %Ts\n", GetLastError(), Game.Dir);
	    	CloseHandle(GameBuildFile);
			return false;
	    }
	    CloseHandle(GameBuildFile);
	}
 
	if(!CopyFile(GameBuildTemplate, GameBuildPath, FALSE))
	{
		MAS_LOG_ERROR("Copying %Ts to [ %Ts ]: %Ts\n", GameBuildTemplate, Game.Dir, _T("Build.bat"));
		return false;
	}
	
    TCHAR BuildGame[MAX_PATH] = {};
    _stprintf(BuildGame, _T("\"%Ts %Ts\""), GameBuildPath, BuildFolder);

	MAS_ASSERT(_tsystem(BuildGame) != -1, "Wasn't able to compile the game [ %Ts ]\n", Game.Dir);  
    return true;
}

static bool masGameInternal_Recompile()
{
	MAS_LOG_INFO("RE_COMPILE: %Ts\n", Game.Dir);
	
	if(!masGame_Compile(_T("NewBuild")))
	{
		MAS_LOG_ERROR("Rebuilding game dll %Ts\n", Game.Dir);
		// TODO: Remove NewBuild Created Folder
		return false;
	}	
	
	EnterCriticalSection(&Game.MonitorThread.CriticalSection);
	SetEvent(Game.MonitorThread.ReloadEvent);
	LeaveCriticalSection(&Game.MonitorThread.CriticalSection);
	MAS_LOG_INFO("RE COMPILE SUCCESS\n");
    return true;
}

bool masGameInternal_ShouldReloadOnFile(const TCHAR* FilePath, const TCHAR** Ext, uint32_t ExtCount)
{
	const TCHAR* FileExt = _tcsrchr(FilePath, _T('.'));
	if(!FileExt)
		return false;
	
	for(int32_t i = 0; i < ExtCount; ++i)
	{
		if(_tcsicmp(FileExt, Ext[i]) == 0)
		{
			_tprintf(_T("OnChangeFile[ %Ts ]\n"), FilePath);
			return true;
		}
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
	
	HANDLE DirHandle = CreateFile(pGame->Dir, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if(DirHandle == INVALID_HANDLE_VALUE)
	{
		MAS_LOG_ERROR("Opengin directory %Ts to wach for changes\n", pGame->Dir);
		return false;
	}
	
	const uint32_t BufferSize   = 4096;
	DWORD          ByteReturned = 0;
	BYTE Buffer[BufferSize]     = {};
	OVERLAPPED Overlapped       = {};
	Overlapped.hEvent           = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	const TCHAR* Extensions[] = 
	{
		_T(".cpp"), _T(".h")
	};
	uint32_t ExtCount = sizeof(Extensions)/sizeof(Extensions[0]);
	TCHAR FilePath[MAX_PATH] = {};
	
	while(TRUE)
	{
		bool bReadChange = ReadDirectoryChangesW(DirHandle, Buffer, BufferSize,
            TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &ByteReturned, &Overlapped, NULL);

        DWORD WaitResult = WaitForSingleObject(Overlapped.hEvent, INFINITE);
		if(WaitResult == WAIT_OBJECT_0)
		{
			FILE_NOTIFY_INFORMATION* NotifyInfo = (FILE_NOTIFY_INFORMATION*)Buffer;
		    do
		    {
		    	if(NotifyInfo->Action == FILE_ACTION_MODIFIED)
		    	{
		    		memcpy(FilePath, NotifyInfo->FileName, NotifyInfo->FileNameLength);
		    		if(masGameInternal_ShouldReloadOnFile(FilePath, Extensions, ExtCount))
		    		{
		    			if(!masGameInternal_Recompile())
		    				MAS_LOG_ERROR("Compiling Game[ %Ts ] on changes\n", pGame->Dir);
		    			else
		    				MAS_LOG_INFO("COMPILE_ON_CHANGES_SUCCESS\n");
		    		}
		    		memset(FilePath, 0, sizeof(TCHAR) * MAX_PATH);
		    	}
		    	
		    	if(NotifyInfo->NextEntryOffset == 0)
		    		break;
		    	NotifyInfo = MAS_ADDR_FROM(FILE_NOTIFY_INFORMATION, NotifyInfo, NotifyInfo->NextEntryOffset);
		    }while(TRUE);
		}   		
	}
	
	CloseHandle(Overlapped.hEvent);
	CloseHandle(DirHandle);
}


/*************************************************************************
*
**************************************************************************/
bool masGame_Load(const TCHAR* GameName)
{
	DWORD CwdLen = GetCurrentDirectory(MAX_PATH, Game.Dir);
	_stprintf(Game.Name, _T("%Ts"), GameName);
	_stprintf(Game.Dir + CwdLen, _T("\\..\\Projects\\%Ts"), GameName); 
	
    MAS_ASSERT(PathFileExists(Game.Dir), "No directory found [ %Ts ]\n", Game.Dir);
	
	TCHAR GamePath[MAX_PATH]  = {};
    _stprintf(GamePath, _T("%Ts\\Build\\masGame.dll"), Game.Dir);
    Game.DLL   = LoadLibrary(GamePath);
    if(!Game.DLL)
    {
		MAS_ASSERT(masGame_Compile(/*Game.Dir,*/ _T("Build")), "Failed Compiling [ %Ts ]\n", Game.Dir);
		
		Game.DLL = LoadLibrary(GamePath);
		MAS_ASSERT(Game.DLL, "Loading Failed [ %Ts ]\n", Game.Dir);
    }      
	
	Game.masStartFunc = MAS_GET_PROC_ADDRESS(Game.DLL, masStart);
	Game.masTickFunc  = MAS_GET_PROC_ADDRESS(Game.DLL, masTick);
	Game.masStopFunc  = MAS_GET_PROC_ADDRESS(Game.DLL, masStop);
	
	MAS_CHECK_PROC_ADDRESS(masStart, Game.masStartFunc);
	MAS_CHECK_PROC_ADDRESS(masTick, Game.masTickFunc);
	MAS_CHECK_PROC_ADDRESS(masStop, Game.masStopFunc);

	Game.MonitorThread.Handle = CreateThread(NULL, 0, masGameInternal_MonitorAndCompileOnChanges, (LPVOID)&Game, 0, &Game.MonitorThread.Id);
    if(Game.MonitorThread.Handle == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Creating Thread to monitor game directory for changes\n"));
		masGame_UnLoad();
		return false;
	}
	Game.MonitorThread.ReloadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&Game.MonitorThread.CriticalSection);

    _tprintf(_T("%Ts: %Ts\n"), _T(MAS_FUNC_NAME), GameName);
	_tprintf(_T("    MonitorThread:\n"));
	_tprintf(_T("        -Handle: 0x%p\n"), Game.MonitorThread.Handle);
	_tprintf(_T("        -Id    : %u\n"), Game.MonitorThread.Id);
    _tprintf(_T("    GameDir: %Ts\n"),   Game.Dir);
    _tprintf(_T("    Handle:    0x%p\n"), Game.DLL);

    return true;
}

bool masGame_ReloadOnChanges(/*masGameAPI* GameAPI*/)
{
	bool  Ret               = false;
	TCHAR DLLPath[MAX_PATH] = {};
	
	EnterCriticalSection(&Game.MonitorThread.CriticalSection);
	if(WaitForSingleObject(Game.MonitorThread.ReloadEvent, 0) == WAIT_OBJECT_0)
	{
	    FreeLibrary(Game.DLL);
        Game.DLL = NULL;
		MAS_LOG_INFO("DLL UNLOADED\n");
		
        //
		TCHAR SwapCmd[MAX_PATH] = {};
		_stprintf(SwapCmd,
		    _T("cmd /c "
			   "\""
			   "cd /d \"%Ts\" && "
			   "(if exist Build ren Build OldBuild) && "
			   "(if exist NewBuild ren NewBuild Build) && "
			   "(if exist OldBuild rmdir /s /q OldBuild) "
			   "\""), Game.Dir);
		_tprintf(_T("GameDir: %Ts\n"), Game.Dir);
		
		MAS_ASSERT(_tsystem(SwapCmd) != -1, "Could not remove OldBuild win32_error %u\n", GetLastError());
	
		_stprintf(DLLPath, _T("%Ts\\Build\\masGame.dll"), Game.Dir);
		Game.DLL = LoadLibrary(DLLPath);
		MAS_ASSERT(Game.DLL, "LOADING NEW GAME DLL FAILED\n");

		Game.masStartFunc = MAS_GET_PROC_ADDRESS(Game.DLL, masStart);
	    Game.masTickFunc  = MAS_GET_PROC_ADDRESS(Game.DLL, masTick);
	    Game.masStopFunc  = MAS_GET_PROC_ADDRESS(Game.DLL, masStop);

	    MAS_CHECK_PROC_ADDRESS(masStrt, Game.masStartFunc);
	    MAS_CHECK_PROC_ADDRESS(masTick, Game.masTickFunc);
	    MAS_CHECK_PROC_ADDRESS(masStop, Game.masStopFunc);
		
		MAS_LOG_INFO("DLL LOADED\n");
	
		ResetEvent(Game.MonitorThread.ReloadEvent);	
		Ret = true;
	}
	LeaveCriticalSection(&Game.MonitorThread.CriticalSection);
	
	return Ret;
}

void masGame_UnLoad()
{
	if(Game.DLL)
		FreeLibrary(Game.DLL);	
	DeleteCriticalSection(&Game.MonitorThread.CriticalSection);
	TerminateThread(Game.MonitorThread.Handle, 0);
	CloseHandle(Game.MonitorThread.Handle);
	memset(&Game, 0, sizeof(masGame));
}

bool masGame_Start()
{
	if(Game.masStartFunc)
		return Game.masStartFunc();
	return false;
}

void masGame_Tick()
{
	if(Game.masTickFunc)
		Game.masTickFunc();
}

void masGame_Stop()
{
	if(Game.masStopFunc)
		Game.masStopFunc();
}