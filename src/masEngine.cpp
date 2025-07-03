#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "masEngine.h"
#include "masWindow.h"
#include "masTime.h"
#include "masInputController.h"
#include "masInputKeyMap.h"
#include "masGameLoader.h"
#include "GameAPI/masEngineAPI.h"

#include <stdio.h>

/*
* Initial Design -> this interface would be called by engine systems to provide needed information/data for the game implementer
*                   but this will result in any change happen in the game api (EngC.h) would trigger the compilation of all the system
*                   files since they all include it to provide information/data directly to user of this api
* 
* Desired Design -> this interface would use the engine's systems to query information/data and convert them and provide them 
*                   to game implementer and this would result in any change happen in game api (EngC.h) would trigger the compilation
*                   of this api only and changed to any engine's system file would invoke compilation of this api eleminating compilation
*                   of the entire engine's files as stated in Inital Design above.
* 
*/

struct masEngine
{
	masWindow  *Window;
	//masGameAPI  Game;
};
static masEngine Engine = {};

/*******************************************************************************************************************
*
********************************************************************************************************************/
double masGame_Time_DeltaTime()
{
	return masTime_DeltaTime();
}

double masGame_Time()
{
	return masTime();
}

/*******************************************************************************************************************
*
********************************************************************************************************************/
void masEngine_Window_OnKey(int32_t KeyCode, masWindowKeyState KeyState, masWindowKeyMod KeyMod)
{
	masGame_Input_Action Action = {};
	//Action.Key                = (masGame_EKey)KeyCode; // converter from platform key to engine agnostic key
	Action.Key                = (masGame_EKey)masEngine_Input_MapKey(EInputDevice_Keyboard, KeyCode);
	Action.KeyMod.Keys.LCtrl  = KeyMod.Keys.LCtrl ;
	Action.KeyMod.Keys.LShift = KeyMod.Keys.LShift;
	Action.KeyMod.Keys.LAlt   = KeyMod.Keys.LAlt  ;
	Action.KeyMod.Keys.RCtrl  = KeyMod.Keys.RCtrl ;
	Action.KeyMod.Keys.RShift = KeyMod.Keys.RShift;
	Action.KeyMod.Keys.RAlt   = KeyMod.Keys.RAlt  ;
	Action.KeyState           = (masGame_EKeyState)KeyState; // converter from source provider keystate to engine agnostic keystate
	Action.Vec2               = {};

	//masGame_Input_OnAction(&Action);
}

void masEngine_Window_OnTextEnter(wchar_t Letter)
{
	//masGame_Input_OnTextEnter(Letter);
}

void masEngine_Window_OnMouseButton(masWindowMouseKey Button, masWindowKeyState KeyState, masWindowKeyMod KeyMod, masPoint MousePos)
{
	masGame_Input_Action Action = {};
	Action.Key                = (masGame_EKey)Button; // converter from platform key to engine agnostic key
	Action.KeyMod.Keys.LCtrl  = KeyMod.Keys.LCtrl;
	Action.KeyMod.Keys.LShift = KeyMod.Keys.LShift;
	Action.KeyMod.Keys.LAlt   = KeyMod.Keys.LAlt;
	Action.KeyMod.Keys.RCtrl  = KeyMod.Keys.RCtrl;
	Action.KeyMod.Keys.RShift = KeyMod.Keys.RShift;
	Action.KeyMod.Keys.RAlt   = KeyMod.Keys.RAlt;
	Action.KeyState           = (masGame_EKeyState)KeyState; // converter from source provider keystate to engine agnostic keystate
	Action.Vec2               = {(float)MousePos.x, (float)MousePos.y};

	//masGame_Input_OnAction(&Action);
}

void masEngine_Window_OnMouseMove(masPoint MousePos)
{
	masGame_SystemEvent Event = {};
	Event.Type           = ESystemEvent_Mouse_Move;
	Event.Data.Mouse.Pos = { MousePos.x, MousePos.y };
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnMouseEnter()
{
	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_Mouse_Enter;
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnMouseLeave()
{
	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_Mouse_Leave;
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnWindowResize(masWindowSize Size, masWindowSize ClientSize)
{
	masWindowPos Pos = masWindow_GetPosition(Engine.Window);

	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_Window_Move;
	Event.Data.Window.Size       = { Size.x,       Size.y };
	Event.Data.Window.ClientSize = { ClientSize.x, ClientSize.y };
	Event.Data.Window.Pos        = { Pos.x,        Pos.y };
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnWindowMaximize()
{
	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_Window_Maximize;
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnWindowMinimize()
{
	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_Window_Minimize;
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnWindowMove(masPoint WindowPos)
{
	masWindowSize Size       = masWindow_GetSize(Engine.Window);
	masWindowSize ClientSize = masWindow_GetClientSize(Engine.Window);

	masGame_SystemEvent Event = {};
	Event.Type            = ESystemEvent_Window_Move;
	Event.Data.Window.Size       = { Size.x,       Size.y };
	Event.Data.Window.ClientSize = { ClientSize.x, ClientSize.y };
	Event.Data.Window.Pos        = { WindowPos.x,  WindowPos.y };
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnDevicesChange()
{
	masGame_SystemEvent Event = {};
	Event.Type = ESystemEvent_DeviceChanges;
	//masGame_Event_OnSystemEvent(Event);
}

void masEngine_Window_OnClose()
{
	
}


/*******************************************************************************************************************
*
********************************************************************************************************************/
static bool masEngine_Create()
{
	masTime_Init();

	//masGame_InitData InitData = masGame_GetInitData();
	Engine.Window = masWindow_Create(L"masEngine", 1200, 800);
	if (!Engine.Window)
		return false;
	else
	{
		masWindowCallbacks WndCallbacks = {};
		WndCallbacks.OnKey            = &masEngine_Window_OnKey;
		WndCallbacks.OnTextEnter      = &masEngine_Window_OnTextEnter;
		WndCallbacks.OnMouseButton    = &masEngine_Window_OnMouseButton;
		WndCallbacks.OnMouseMove      = &masEngine_Window_OnMouseMove;
		WndCallbacks.OnMouseEnter     = &masEngine_Window_OnMouseEnter;
		WndCallbacks.OnMouseLeave     = &masEngine_Window_OnMouseLeave;
		WndCallbacks.OnWindowResize   = &masEngine_Window_OnWindowResize;
		WndCallbacks.OnWindowMaximize = &masEngine_Window_OnWindowMaximize;
		WndCallbacks.OnWindowMinimize = &masEngine_Window_OnWindowMinimize;
		WndCallbacks.OnWindowMove     = &masEngine_Window_OnWindowMove;
		WndCallbacks.OnDevicesChange  = &masEngine_Window_OnDevicesChange;
		WndCallbacks.OnClose          = &masEngine_Window_OnClose;
		masWindow_SetCallbacks(Engine.Window, WndCallbacks); 
	}


    //
    if(!masGame_Load(MAS_TEXT("eng_test")))
		return false;
	
	if(!masGame_Start())
		return false;
	
	masWindow_Show(Engine.Window, true);

	return true;
}

static void masEngine_Destroy()
{
	masGame_Stop();
	masGame_UnLoad();
	masWindow_Destroy(&Engine.Window);
}


/*******************************************************************************************************************
*
********************************************************************************************************************/
static void masEngine_DispatchEvents()
{
	masWindow_DispatchEvents();
}

int main(int argc, const char** argv)
{
	if (!masEngine_Create())
	{
		masEngine_Destroy();
		return -1;
	}

	while (!masWindow_IsClosed(Engine.Window))
	{
		if(masGame_ReloadOnChanges())
			masGame_Start();
		
		masTime_Update();
		masEngine_DispatchEvents();
		

		masGame_Tick();
	}

	masEngine_Destroy();
	return 0;
}




/*******************************************************************************************************************
* MAS ENGINE API IMPL
********************************************************************************************************************/


/*****************************************************************************
* Window API
*****************************************************************************/
MAS_ENGINE_API masEngine_Window masEngine_Window_GetHandle()
{
	return masWindow_GetHandle(Engine.Window);
}

MAS_ENGINE_API void masEngine_Window_SetTitle(const masChar* InTitle)
{
	masWindow_SetTitle(Engine.Window, InTitle);
}

MAS_ENGINE_API void masEngine_Window_SetSize(int32_t InW, int32_t InH)
{
	masWindow_SetSize(Engine.Window, { InW, InH });
}

MAS_ENGINE_API void masEngine_Window_GetSize(int32_t* OutW, int32_t* OutH)
{
	masWindowSize Size = masWindow_GetSize(Engine.Window);
	if(OutW)
		*OutW = Size.x;
	if(OutH)
		*OutH = Size.y;
}

MAS_ENGINE_API void masEngine_Window_GetClientSize(int32_t* OutW, int32_t* OutH)
{
	masWindowSize ClientSize = masWindow_GetClientSize(Engine.Window);
	if(OutW)
		*OutW = ClientSize.x;
	if(OutH)
		*OutH = ClientSize.y;
}

MAS_ENGINE_API void masEngine_Window_GetAspectRatio(float* OutAspectRatio)
{
	if(!OutAspectRatio)
		return;
	
	masWindowSize ClientSize = masWindow_GetClientSize(Engine.Window);
	*OutAspectRatio = (float)ClientSize.x / (float)ClientSize.y;
}

/*****************************************************************************
* Time API
*****************************************************************************/
MAS_ENGINE_API double masEngine_Time_Delta()
{
	return masTime_DeltaTime();
}

MAS_ENGINE_API double masEngine_Time()
{
	return masTime();
}































