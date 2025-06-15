#pragma once

#include <stdint.h>

/*****************************************************************************
* MAIN API & UTILS SECTIONS
*****************************************************************************/
struct masGame_InitData
{
	const wchar_t *Name;
	const wchar_t *AssetPath;
	int32_t        MajorVersion;
	int32_t        MinorVersion;
	int            Width;
	int            Height;
};

extern masGame_InitData masGame_GetInitData();
extern bool masGame_Init();
extern void masGame_DeInit();
extern void masGame_Update(double ElapsedTime);


/*****************************************************************************
* 
*****************************************************************************/
double masGame_Time_DeltaTime();
double masGame_Time();




/*****************************************************************************
* INPUT SECTION
*****************************************************************************/
enum masGame_EKey
{
	EKey_Unknown = 0,

	/*
	* GAMEPAD BUTTONS & AXES & TRIGGERS
	*/
	EKey_Square,
	EKey_Cross,
	EKey_Circle,
	EKey_Triangle,
	EKey_Start,
	EKey_Select,
	EKey_DpadUp,
	EKey_DpadDown,
	EKey_DpadRight,
	EKey_DpadLeft,
	EKey_L1,
	EKey_L2,
	EKey_L3,
	EKey_R1,
	EKey_R2,
	EKey_R3,
	EKey_LAnalogUp,
	EKey_LAnalogDown,
	EKey_LAnalogLeft,
	EKey_LAnalogRight,
	EKey_RAnalogUp,
	EKey_RAnalogDown,
	EKey_RAnalogLeft,
	EKey_RAnalogRight,

	EKey_Anykey,

	EKey_A,
	EKey_B,
	EKey_C,
	EKey_D,
	EKey_E,
	EKey_F,
	EKey_G,
	EKey_H,
	EKey_I,
	EKey_J,
	EKey_K,
	EKey_L,
	EKey_M,
	EKey_N,
	EKey_O,
	EKey_P,
	EKey_Q,
	EKey_R,
	EKey_S,
	EKey_T,
	EKey_U,
	EKey_V,
	EKey_W,
	EKey_X,
	EKey_Y,
	EKey_Z,

	EKey_F1,
	EKey_F2,
	EKey_F3,
	EKey_F4,
	EKey_F5,
	EKey_F6,
	EKey_F7,
	EKey_F8,
	EKey_F9,
	EKey_F10,
	EKey_F11,
	EKey_F12,

	EKey_NumLock,
	EKey_Numpad0,
	EKey_Numpad1,
	EKey_Numpad2,
	EKey_Numpad3,
	EKey_Numpad4,
	EKey_Numpad5,
	EKey_Numpad6,
	EKey_Numpad7,
	EKey_Numpad8,
	EKey_Numpad9,

	EKey_Num0,
	EKey_Num1,
	EKey_Num2,
	EKey_Num3,
	EKey_Num4,
	EKey_Num5,
	EKey_Num6,
	EKey_Num7,
	EKey_Num8,
	EKey_Num9,

	EKey_Decimal,
	EKey_PageUp,
	EKey_PageDown,
	EKey_Space,
	EKey_Enter,
	EKey_Backspace,
	EKey_Tab,
	EKey_PrintScreen,
	EKey_Insert,
	EKey_Delete,
	EKey_Divide,
	EKey_Multipy,
	EKey_Subtract,
	EKey_Addition,
	EKey_Home,
	EKey_End,
	EKey_Escape,
	EKey_CapsLock,

	EKey_Colon,
	EKey_SemiColon,
	EKey_Apostrophe,
	EKey_DoubleQuote,
	EKey_LeftSquareBracket,
	EKey_LeftCurlyBrace,
	EKey_RightSquareBracket,
	EKey_RightCurlyBrace,
	EKey_Comma,
	EKey_LessThan,
	EKey_Period,
	EKey_VerticalBar,
	EKey_GreaterThan,
	EKey_QuestionMark,
	EKey_ForwardSlash,
	EKey_BackSlash,
	EKey_Tilde,
	EKey_GraveAccent,
	EKey_ExclamationMark,
	EKey_At,
	EKey_Hash,
	EKey_Dollar,
	EKey_Percent,
	EKey_Caret,
	EKey_Ampersand,
	EKey_Asterisk,
	EKey_LeftParenthesis,
	EKey_RightParenthesis,
	EKey_Hyphen,
	EKey_UnderScore,
	EKey_Plus,
	EKey_Equals,

	EKey_ArrowUp,
	EKey_ArrowDown,
	EKey_ArrowLeft,
	EKey_ArrowRight,

	EKey_LeftCtrl,
	EKey_LeftShift,
	EKey_LeftAlt,
	EKey_RightCtrl,
	EKey_RightShift,
	EKey_RightAlt,

	/*
	* MOUSE BUTTONS
	*/
	EKey_MouseWheelUp,
	EKey_MouseWheelDown,
	EKey_MouseLeft,
	EKey_MouseRight,
	EKey_MouseMiddle,
	EKey_MouseX1,
	EKey_MouseX2,

	EKey_Count
};

enum masGame_EKeyState
{
	EKeyState_None,
	EKeyState_Release,
	EKeyState_Press,
	EKeyState_Repeat,
	EKeyState_DoubleClick
};

union masGame_KeyMod
{
	uint8_t Value;
	struct
	{
		uint8_t LCtrl  : 1;
		uint8_t LShift : 1;
		uint8_t LAlt   : 1;
		uint8_t RCtrl  : 1;
		uint8_t RShift : 1;
		uint8_t RAlt   : 1;
	}Keys;
};

struct masGame_Input_Action
{
	struct
	{
		float x, y;
	}Vec2;
	masGame_EKey      Key;
	masGame_EKeyState KeyState;	
	masGame_KeyMod    KeyMod;
};
struct masGame_Input_Axis
{
	masGame_EKey   Key;
	masGame_KeyMod KeyMod;
	float          Value;
};
void masGame_Input_OnAction(masGame_Input_Action* Action);
void masGame_Input_OnAxis(masGame_Input_Axis* Axis);
void masGame_Input_OnTextEnter(wchar_t Letter);


/*****************************************************************************
* SYSTEM EVENT SECTION
*****************************************************************************/
enum masGame_ESystemEvent
{
	ESystemEvent_DeviceChanges,
	ESystemEvent_Mouse_Move,
	ESystemEvent_Mouse_Enter,
	ESystemEvent_Mouse_Leave,
	ESystemEvent_Window_Move,
	ESystemEvent_Window_Resize,
	ESystemEvent_Window_Maximize,
	ESystemEvent_Window_Minimize,

};
struct masGame_SystemEvent
{
	masGame_ESystemEvent Type;
	union
	{
		struct
		{
			struct
			{
				int32_t x, y;
			}Size, Pos, ClientSize;
		}Window;

		struct
		{
			struct
			{
				int32_t x, y;
			}Pos;
		}Mouse;

	}Data;
};
void masGame_Event_OnSystemEvent(masGame_SystemEvent Event);

