#pragma once

#include <stdint.h>
#include "masDefs.h"

/*****************************************************************************************
* INTERFACE DATA
******************************************************************************************/
typedef struct
{
	int32_t x, y;
}masWindowSize, masWindowPos, masPoint;

struct masWindow;


/*****************************************************************************************
* MAIN INTERFACE
******************************************************************************************/
masWindow* masWindow_Create(const masChar* Title, int32_t Width, int32_t Height);
void       masWindow_Destroy(masWindow** Wnd);
void       masWindow_DispatchEvents();
bool       masWindow_IsClosed(masWindow* Wnd);
void       masWindow_Show(masWindow* Wnd, bool Enable);


/*****************************************************************************************
* QUERY INTERFACE
******************************************************************************************/
masWindowSize masWindow_GetSize(masWindow* Wnd);
masWindowSize masWindow_GetClientSize(masWindow* Wnd);
masWindowPos  masWindow_GetPosition(masWindow* Wnd);
void*         masWindow_GetHandle(masWindow* Wnd);


/*****************************************************************************************
* MODIFY INTERFACE
******************************************************************************************/
void masWindow_SetPosition(masWindow* Wnd, masWindowPos Pos);
void masWindow_SetSize    (masWindow* Wnd, masWindowSize Size);
void masWindow_SetTitle   (masWindow* Wnd, const masChar* Title);


/*****************************************************************************************
* 
******************************************************************************************/
enum masWindowMouseKey
{
	WindowMouseKey_Left,
	WindowMouseKey_Right,
	WindowMouseKey_Middle,
	WindowMouseKey_WheelUp,
	WindowMouseKey_WheelDown,
	WindowMouseKey_X1,
	WindowMouseKey_X2,
};

enum masWindowKeyState
{
	WindowKeyState_Release,
	WindowKeyState_Press,
	WindowKeyState_Repeat,
	WindowKeyState_DoubleClick
};

union masWindowKeyMod
{
	uint8_t Value;
	struct
	{
		uint8_t LCtrl  : 1;
		uint8_t LShift : 1;
		uint8_t LAlt   : 1;
		uint8_t LCmd   : 1;
		uint8_t RCtrl  : 1;
		uint8_t RShift : 1;
		uint8_t RAlt   : 1;
		uint8_t RCmd   : 1;
	}Keys;
};

typedef void(*masWindow_OnKeyFunc)            (int32_t KeyCode, masWindowKeyState KeyState, masWindowKeyMod KeyMod);
typedef void(*masWindow_OnTextEnterFunc)      (masChar Letter);
typedef void(*masWindow_OnMouseButtonFunc)    (masWindowMouseKey Button, masWindowKeyState KeyState, masWindowKeyMod KeyMod, masPoint MousePos);
typedef void(*masWindow_OnMouseMoveFunc)      (masPoint MousePos);
typedef void(*masWindow_OnMouseEnterFunc)     ();
typedef void(*masWindow_OnMouseLeaveFunc)     ();
typedef void(*masWindow_OnWindowResizeFunc)   (masWindowSize Size, masWindowSize ClientSize);
typedef void(*masWindow_OnWindowMaximizeFunc) ();
typedef void(*masWindow_OnWindowMinimizeFunc) ();
typedef void(*masWindow_OnWindowMoveFunc)     (masPoint WindowPos);
typedef void(*masWindow_OnDevicesChangeFunc)  ();
typedef void(*masWindow_OnCloseFunc)          ();

struct masWindowCallbacks
{
	masWindow_OnKeyFunc            OnKey;
	masWindow_OnTextEnterFunc      OnTextEnter;
	masWindow_OnMouseButtonFunc    OnMouseButton;
	masWindow_OnMouseMoveFunc      OnMouseMove;
	masWindow_OnMouseEnterFunc     OnMouseEnter;
	masWindow_OnMouseLeaveFunc     OnMouseLeave;
	masWindow_OnWindowResizeFunc   OnWindowResize;
	masWindow_OnWindowMaximizeFunc OnWindowMaximize;
	masWindow_OnWindowMinimizeFunc OnWindowMinimize;
	masWindow_OnWindowMoveFunc     OnWindowMove;
	masWindow_OnDevicesChangeFunc  OnDevicesChange;
	masWindow_OnCloseFunc          OnClose;
};

void masWindow_SetCallbacks(masWindow* Wnd, masWindowCallbacks Callbacks);