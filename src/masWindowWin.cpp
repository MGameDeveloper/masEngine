#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>

#include <stdlib.h>
#include <string.h>

#include "masWindow.h"

#define WND_CLASSNAME TEXT("EngC_Window")


/*****************************************************************************************
* 
******************************************************************************************/
struct masWindow
{
	masWindowCallbacks Callbacks;
	HINSTANCE      Instance;
	HWND           Handle;
	const wchar_t *Title;
	masWindowSize  Size;
	masWindowSize  ClientSize;
	masWindowPos   Position;
	bool           bClose;
	bool           bTrackMouse;
	bool           bMouseInWindow;
};

/*****************************************************************************************
* MAIN INTERFACE
******************************************************************************************/
LRESULT CALLBACK masWindow_Proc(HWND, UINT, WPARAM, LPARAM);

masWindow* masWindow_Create(const wchar_t* Title, int32_t Width, int32_t Height)
{
	HINSTANCE Instance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { 0 };
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.hInstance     = Instance;
	wc.lpszClassName = WND_CLASSNAME;
	wc.lpfnWndProc   = &masWindow_Proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = DLGWINDOWEXTRA;
	wc.hIcon         = NULL; //LoadIcon(Instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIconSm       = NULL; //LoadIcon(Instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor       = LoadCursor(Instance, IDC_ARROW);
	wc.lpszMenuName  = NULL;

	if (!RegisterClassEx(&wc))
		return NULL;

	masWindowSize ScreenSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	masWindowPos  Pos        = { (ScreenSize.x - Width) / 2, (ScreenSize.y - Height) / 2 };
	masWindowSize Size       = { Width, Height };

	HWND Handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, WND_CLASSNAME, Title, WS_OVERLAPPEDWINDOW,
		Pos.x, Pos.y, Size.x, Size.y, NULL, NULL, Instance, NULL);
	if (!Handle)
	{
		UnregisterClass(WND_CLASSNAME, Instance);
		return NULL;
	}

	RECT Rect = {};
	GetClientRect(Handle, &Rect);
	masWindowSize ClientSize = { Rect.right - Rect.left, Rect.bottom - Rect.top };

	/*
	*/
	masWindow* Wnd = (masWindow*)malloc(sizeof(masWindow));
	if (!Wnd)
	{
		DestroyWindow(Handle);
		UnregisterClass(WND_CLASSNAME, Instance);
		return NULL;
	}
	memset(Wnd, 0, sizeof(masWindow));

	Wnd->Instance   = Instance;
	Wnd->Handle     = Handle;
	Wnd->Position   = Pos;
	Wnd->Size       = Size;
	Wnd->ClientSize = ClientSize;
	Wnd->Title      = Title;

	SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)Wnd);

	return Wnd;
}

void masWindow_Destroy(masWindow** Wnd)
{
	if (!Wnd || !(*Wnd))
		return;

	masWindow* WndData = *Wnd;
	DestroyWindow(WndData->Handle);
	UnregisterClass(WND_CLASSNAME, WndData->Instance);
	
	free(*Wnd);
	*Wnd = NULL;
}

void masWindow_DispatchEvents()
{
	MSG Msg = {};

	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

bool masWindow_IsClosed(masWindow* Wnd)
{
	if (!Wnd)
		return true;
	return Wnd->bClose;
}

void masWindow_Show(masWindow* Wnd, bool Enable)
{
	if (!Wnd)
		return;

	ShowWindow(Wnd->Handle, (Enable) ? 1 : 0);
}

/*****************************************************************************************
* QUERY INTERFACE
******************************************************************************************/
masWindowSize masWindow_GetSize(masWindow* Wnd)
{
	if (!Wnd)
		return { 0, 0 };
	return Wnd->Size;
}
masWindowSize masWindow_GetClientSize(masWindow* Wnd)
{
	if (!Wnd)
		return { 0, 0 };
	return Wnd->ClientSize;
}
masWindowPos masWindow_GetPosition(masWindow* Wnd)
{
	if (!Wnd)
		return { 0, 0 };
	return Wnd->Position;
}

void* masWindow_GetHandle(masWindow* Wnd)
{
	return (void*)Wnd->Handle;
}

/*****************************************************************************************
* MODIFY INTERFACE
******************************************************************************************/
void masWindow_SetPosition(masWindow* Wnd, masWindowPos Pos)
{
	if (!Wnd)
		return;
	Wnd->Position = Pos;
	SetWindowPos(Wnd->Handle, NULL, Pos.x, Pos.y, Wnd->Size.x, Wnd->Size.y, SWP_NOZORDER | SWP_NOSIZE);
}
void masWindow_SetSize(masWindow* Wnd, masWindowSize Size)
{
	if (!Wnd)
		return;
	Wnd->Size = Size;
	SetWindowPos(Wnd->Handle, NULL, Wnd->Position.x, Wnd->Position.y, Size.x, Size.y, SWP_NOMOVE | SWP_NOZORDER);

	RECT Rect = {};
	GetClientRect(Wnd->Handle, &Rect);
	Wnd->ClientSize.x = Rect.right - Rect.left;
	Wnd->ClientSize.y = Rect.bottom - Rect.top;
}
void masWindow_SetTitle(masWindow* Wnd, const wchar_t* Title)
{
	if (!Wnd)
		return;

	SetWindowText(Wnd->Handle, Title);
	Wnd->Title = Title;
}

void masWindow_SetCallbacks(masWindow* Wnd, masWindowCallbacks Callbacks)
{
	if (!Wnd)
		return;
	Wnd->Callbacks = Callbacks;
}


/*****************************************************************************************
* 
******************************************************************************************/
LRESULT masWindow_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	masWindow* Wnd = (masWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!Wnd)
		return DefWindowProc(hwnd, msg, wparam, lparam);
	masWindowCallbacks* Funcs = &Wnd->Callbacks;

	masWindowKeyMod KeyMod = {};
	KeyMod.Keys.LCtrl  = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? 1 : 0;
	KeyMod.Keys.LShift = (GetAsyncKeyState(VK_LSHIFT  ) & 0x8000) ? 1 : 0;
	KeyMod.Keys.LAlt   = (GetAsyncKeyState(VK_LMENU   ) & 0x8000) ? 1 : 0;
	KeyMod.Keys.LCmd   = 0;
	KeyMod.Keys.RCtrl  = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? 1 : 0;
	KeyMod.Keys.RShift = (GetAsyncKeyState(VK_RSHIFT  ) & 0x8000) ? 1 : 0;
	KeyMod.Keys.RAlt   = (GetAsyncKeyState(VK_RMENU   ) & 0x8000) ? 1 : 0;
	KeyMod.Keys.RCmd   = 0;

	/*
	* Data InCase of Mouse XButton event
	*/
	masPoint          MousePos = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	WORD              XButton = HIWORD(wparam);
	masWindowMouseKey MouseButton = {};
	if (XButton == XBUTTON1)
		MouseButton = WindowMouseKey_X1;
	else if (XButton == XBUTTON2)
		MouseButton = WindowMouseKey_X2;

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		if (Funcs->OnClose)
			Funcs->OnClose();
		Wnd->bClose = true;
		return 0;

	case WM_SIZE:
	{
		RECT Rect = {};
		GetWindowRect(Wnd->Handle, &Rect);
		Wnd->Size       = { Rect.right - Rect.left, Rect.bottom - Rect.top };
		Wnd->ClientSize = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

		switch (wparam)
		{
		case SIZE_MAXIMIZED: if (Funcs->OnWindowMaximize) Funcs->OnWindowMaximize();   break;
		case SIZE_MINIMIZED: if (Funcs->OnWindowMinimize) Funcs->OnWindowMinimize();   break;
		case SIZE_RESTORED:  if (Funcs->OnWindowResize)	  Funcs->OnWindowResize(Wnd->Size, Wnd->ClientSize); break;
		}
	}
		return 0; 

	case WM_MOVE:
		if (Funcs->OnWindowMove)
		{
			masWindowPos Pos = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
			Wnd->Position = Pos;
			Funcs->OnWindowMove(Pos);
		}
		return 0;

	case WM_MOUSEMOVE:
		if (!Wnd->bTrackMouse)
		{
			TRACKMOUSEEVENT tme = { sizeof(tme) };
			tme.dwFlags      = TME_LEAVE;
			tme.hwndTrack    = hwnd;
			Wnd->bTrackMouse = true;
			TrackMouseEvent(&tme);

			if (!Wnd->bMouseInWindow)
			{
				Wnd->bMouseInWindow = true;
				if (Funcs->OnMouseEnter)
					Funcs->OnMouseEnter();
			}
		}

		if (Funcs->OnMouseMove)
			Funcs->OnMouseMove(MousePos);
		return 0;

	case WM_MOUSELEAVE:
		Wnd->bTrackMouse    = false;
		Wnd->bMouseInWindow = false;
		if (Funcs->OnMouseLeave)
			Funcs->OnMouseLeave();
		return 0;

	case WM_MOUSEWHEEL:
		if (Funcs->OnMouseButton)
		{
			int32_t  Delta    = GET_WHEEL_DELTA_WPARAM(wparam);

			if (Delta < 0)
				Funcs->OnMouseButton(WindowMouseKey_WheelDown, WindowKeyState_Press, KeyMod, MousePos);
			else
				Funcs->OnMouseButton(WindowMouseKey_WheelUp,   WindowKeyState_Press, KeyMod, MousePos);
		}
	    return 0;

	case WM_LBUTTONDOWN:   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Left,   WindowKeyState_Press,       KeyMod, MousePos); return 0;
	case WM_LBUTTONUP:	   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Left,   WindowKeyState_Release,     KeyMod, MousePos); return 0;
	case WM_LBUTTONDBLCLK: if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Left,   WindowKeyState_DoubleClick, KeyMod, MousePos); return 0;
	case WM_MBUTTONDOWN:   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Middle, WindowKeyState_Press,       KeyMod, MousePos); return 0;
	case WM_MBUTTONUP:	   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Middle, WindowKeyState_Release,     KeyMod, MousePos); return 0;
	case WM_MBUTTONDBLCLK: if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Middle, WindowKeyState_DoubleClick, KeyMod, MousePos); return 0;
	case WM_RBUTTONDOWN:   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Right,  WindowKeyState_Press,       KeyMod, MousePos); return 0;
	case WM_RBUTTONUP:	   if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Right,  WindowKeyState_Release,     KeyMod, MousePos); return 0;
	case WM_RBUTTONDBLCLK: if (Funcs->OnMouseButton) Funcs->OnMouseButton(WindowMouseKey_Right,  WindowKeyState_DoubleClick, KeyMod, MousePos); return 0;
	case WM_XBUTTONDOWN:   if (Funcs->OnMouseButton) Funcs->OnMouseButton(MouseButton,           WindowKeyState_Press,       KeyMod, MousePos); return 0;
	case WM_XBUTTONUP:     if (Funcs->OnMouseButton) Funcs->OnMouseButton(MouseButton,           WindowKeyState_Release,     KeyMod, MousePos); return 0;
	case WM_XBUTTONDBLCLK: if (Funcs->OnMouseButton) Funcs->OnMouseButton(MouseButton,           WindowKeyState_DoubleClick, KeyMod, MousePos); return 0;
	

	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		if (Funcs->OnKey)
		{
			WORD VKCode        = LOWORD(wparam);                          // virtual-key code
			WORD KeyFlags      = HIWORD(lparam);
			WORD ScanCode      = LOBYTE(KeyFlags);                        // scan code
			BOOL WasKeyDown    = (KeyFlags & KF_REPEAT) == KF_REPEAT;     // previous key-state flag, 1 on autorepeat
			BOOL IsKeyReleased = (KeyFlags & KF_UP) == KF_UP;             // transition-state flag, 1 on keyup
			BOOL IsExtendedKey = (KeyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
			if (IsExtendedKey)
				ScanCode = MAKEWORD(ScanCode, 0xE0);

			WORD ExtKeyCode = 0;
			switch (VKCode)
			{
			case VK_CONTROL: 
			case VK_SHIFT:
			case VK_MENU:
				ExtKeyCode = LOWORD(MapVirtualKey(ScanCode, MAPVK_VSC_TO_VK_EX));
				switch (ExtKeyCode)
				{
				case VK_LCONTROL: VKCode = VK_LCONTROL; break;
				case VK_LSHIFT:   VKCode = VK_LSHIFT;   break;
				case VK_LMENU:    VKCode = VK_LMENU;    break;
				case VK_RCONTROL: VKCode = VK_RCONTROL; break;
				case VK_RSHIFT:   VKCode = VK_RSHIFT;   break;
				case VK_RMENU:    VKCode = VK_RMENU;    break;
				}
				break;
			}

			if (IsKeyReleased)
				Funcs->OnKey(VKCode, WindowKeyState_Release, KeyMod);
			else if (WasKeyDown)
				Funcs->OnKey(VKCode, WindowKeyState_Repeat, KeyMod);
			else
				Funcs->OnKey(VKCode, WindowKeyState_Press, KeyMod);
		}
		return 0;

	case WM_CHAR:
	case WM_SYSCHAR:
		if (Funcs->OnTextEnter)
			Funcs->OnTextEnter((wchar_t)wparam);
		return 0;

	case WM_DEVICECHANGE:
		if (Funcs->OnDevicesChange)
			Funcs->OnDevicesChange();
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}