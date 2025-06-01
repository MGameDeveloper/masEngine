#if 0
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")

#include "EngC_Input.h"
#include "masTime.h"

#include <stdio.h>

static EngC_Gamepad Gamepads[4] = {};

bool EngC_Input_Init()
{
	EngC_Input_CheckDevices();
}

void EngC_Input_CheckDevices()
{
	XINPUT_STATE State;
	for (int i = 0; i < 4; ++i) 
	{
		if (XInputGetState(i, &State) == ERROR_SUCCESS)
		{
			if (Gamepads[i].bConnected == false)
			{
				printf("CONTROLLER[ %d ]: Connected\n", i);
				Gamepads[i].bConnected = true;
			}
		}
		else
		{
			if (Gamepads->bConnected == true)
			{
				printf("CONTROLLER[ %d ]: Disconnected\n", i);
				Gamepads[i].bConnected = false;
			}
		}
	}
}

const EngC_Gamepad* EngC_Input_GetGamepad(int32_t GamepadIdx)
{
	return &Gamepads[GamepadIdx];
}

int32_t EngC_Input_GetModKeyState()
{
	EngC_KeyMod KeyMod = {};
	KeyMod.Keys.LCtrl  |= (VK_LCONTROL & 0x8000) ? EKeyMod_LCtrl  : 0;
	KeyMod.Keys.LShift |= (VK_LSHIFT   & 0x8000) ? EKeyMod_LShift : 0;
	KeyMod.Keys.LAlt   |= (VK_LMENU    & 0x8000) ? EKeyMod_LAlt   : 0;
	KeyMod.Keys.RCtrl  |= (VK_RCONTROL & 0x8000) ? EKeyMod_LCtrl  : 0;
	KeyMod.Keys.RShift |= (VK_RSHIFT   & 0x8000) ? EKeyMod_LShift : 0;
	KeyMod.Keys.RAlt   |= (VK_RMENU    & 0x8000) ? EKeyMod_LAlt   : 0;

	return KeyMod.Value;
}

/***************************************************************************************************************
*
****************************************************************************************************************/
enum EngC_EGamepad
{
    EGamepad_Square,
    EGamepad_Cross,
    EGamepad_Circle,
    EGamepad_Triangle,	  
    EGamepad_Start,
    EGamepad_Select,
    EGamepad_DpadUp,
    EGamepad_DpadDown,	  
    EGamepad_DpadRight ,
    EGamepad_DpadLeft,	  
    EGamepad_L1,
    EGamepad_L2,
    EGamepad_L3,
    EGamepad_R1,
    EGamepad_R2,
    EGamepad_R3,
    EGamepad_LAnalogUp,
    EGamepad_LAnalogDown,
    EGamepad_LAnalogLeft,
    EGamepad_LAnalogRight,
    EGamepad_RAnalogUp,
    EGamepad_RAnalogDown,
    EGamepad_RAnalogLeft,
    EGamepad_RAnalogRight,

	EGamepad_Count
};


void EngC_Input_Update()
{
	XINPUT_STATE    XState   = {};
	XINPUT_GAMEPAD *XGamepad = nullptr;
	EngC_Gamepad   *Gamepad  = nullptr;

	for (int32_t Idx = 0; Idx < 4; ++Idx)
	{
		Gamepad = &Gamepads[Idx];
		if (!Gamepad->bConnected)
			continue;

		memset(&XState, 0, sizeof(XINPUT_STATE));
		if (XInputGetState(Idx, &XState) != ERROR_SUCCESS)
		{
			Gamepad->bConnected = false;
			continue;
		}

		XGamepad = &XState.Gamepad;

		static bool Buttons[EGamepad_Count];
		memset(Buttons, 0, sizeof(bool) * EGamepad_Count);

		Buttons[EGamepad_Square]       = XGamepad->wButtons & XINPUT_GAMEPAD_X;
		Buttons[EGamepad_Cross]        = XGamepad->wButtons & XINPUT_GAMEPAD_A;
		Buttons[EGamepad_Circle]       = XGamepad->wButtons & XINPUT_GAMEPAD_B;
		Buttons[EGamepad_Triangle]     = XGamepad->wButtons & XINPUT_GAMEPAD_Y;
		Buttons[EGamepad_Start]        = XGamepad->wButtons & XINPUT_GAMEPAD_START;
		Buttons[EGamepad_Select]       = XGamepad->wButtons & XINPUT_GAMEPAD_BACK;
		Buttons[EGamepad_DpadUp]       = XGamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
		Buttons[EGamepad_DpadDown]     = XGamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		Buttons[EGamepad_DpadRight]    = XGamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		Buttons[EGamepad_DpadLeft]     = XGamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		Buttons[EGamepad_L1]           = XGamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		Buttons[EGamepad_R1]           = XGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		Buttons[EGamepad_L3]           = XGamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
		Buttons[EGamepad_R3]           = XGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
		/////////////////////////////////////////////////////////////////////////////////////
		Buttons[EGamepad_L2]           = XGamepad->bLeftTrigger >   Gamepad->Threshold.LTrigger;
		Buttons[EGamepad_R2]           = XGamepad->bRightTrigger >  Gamepad->Threshold.RTrigger;
		Buttons[EGamepad_LAnalogUp]    = XGamepad->sThumbLY >       Gamepad->Deadzone.LAnalog;
		Buttons[EGamepad_LAnalogDown]  = XGamepad->sThumbLY <      -Gamepad->Deadzone.LAnalog;
		Buttons[EGamepad_LAnalogRight] = XGamepad->sThumbLX >       Gamepad->Deadzone.LAnalog;
		Buttons[EGamepad_LAnalogLeft]  = XGamepad->sThumbLX <      -Gamepad->Deadzone.LAnalog;
		Buttons[EGamepad_RAnalogUp]    = XGamepad->sThumbRY >       Gamepad->Deadzone.RAnalog;
		Buttons[EGamepad_RAnalogDown]  = XGamepad->sThumbRY <      -Gamepad->Deadzone.RAnalog;
		Buttons[EGamepad_RAnalogLeft]  = XGamepad->sThumbRX <      -Gamepad->Deadzone.RAnalog;
		Buttons[EGamepad_RAnalogRight] = XGamepad->sThumbRX >       Gamepad->Deadzone.RAnalog;

		/*
		* Dispatch axis keys L2, R2, LAnalog, RAnalog
		*/
		if (Buttons[EGamepad_L2])            Gamepads[Idx].Axes.LTrigger = XGamepad->bLeftTrigger  / 255.f;
		if (Buttons[EGamepad_R2])            Gamepads[Idx].Axes.RTrigger = XGamepad->bRightTrigger / 255.f;
		if (Buttons[EGamepad_LAnalogUp])     Gamepads[Idx].Axes.LAnalogY = XGamepad->sThumbLY / 32767.f;
		if (Buttons[EGamepad_LAnalogDown])   Gamepads[Idx].Axes.LAnalogY = XGamepad->sThumbLY / 32768.f;
		if (Buttons[EGamepad_LAnalogRight])  Gamepads[Idx].Axes.LAnalogX = XGamepad->sThumbLX / 32767.f;
		if (Buttons[EGamepad_LAnalogLeft])   Gamepads[Idx].Axes.LAnalogX = XGamepad->sThumbLX / 32768.f;
		if (Buttons[EGamepad_RAnalogUp])	 Gamepads[Idx].Axes.RAnalogY = XGamepad->sThumbRY / 32767.f;
		if (Buttons[EGamepad_RAnalogDown])   Gamepads[Idx].Axes.RAnalogY = XGamepad->sThumbRY / 32768.f;
		if (Buttons[EGamepad_RAnalogLeft])   Gamepads[Idx].Axes.RAnalogX = XGamepad->sThumbRX / 32768.f;
		if (Buttons[EGamepad_RAnalogRight])  Gamepads[Idx].Axes.RAnalogX = XGamepad->sThumbRX / 32767.f;

		// TO DO TIME FOR EMULATING REPEATING
		float AppTime = (float)EngC_Time();

		/*
		* Dispatch Buttons
		*/
		for (int32_t ButtonIdx = 0; ButtonIdx < EGamepad_Count; ++ButtonIdx)
		{
			EngC_EKey Key   = EngC_Input_MapGamepadButton(ButtonIdx);
			bool CurrState  = Buttons[ButtonIdx];
			bool LastState  = Gamepad->LastState[ButtonIdx];
			bool IsPressed  = !LastState && CurrState;
			bool IsReleased = LastState && !CurrState;
			bool IsRepeated = LastState && CurrState;

			if (IsRepeated)
			{
				if (Gamepad->RepeatTime[ButtonIdx] <= AppTime)
				{
					EventHandler->OnGamepadButton(Idx, Key, EKeyState_Repeat);
					Gamepad->RepeatTime[ButtonIdx] = AppTime + RepeatTime;
				}
			}
			else if (IsReleased)
				EventHandler->OnGamepadButton(Idx, Key, EKeyState_Release);
			else if (IsPressed)
			{
				EventHandler->OnGamepadButton(Idx, Key, EKeyState_Press);
				Gamepad->RepeatTime[ButtonIdx] = AppTime + RepeatInitTime;
			}
		}

		::memcpy(Gamepad->LastState, Buttons, sizeof(bool) * EGamepad_Count);
	}
}


/***************************************************************************************************************
*
****************************************************************************************************************/
int32_t EngC_Input_MapKeyboardKey(int32_t OSKey)
{
	switch (OSKey)
	{
	case 'A': return EKey_A;
	case 'B': return EKey_B;
	case 'C': return EKey_C;
	case 'D': return EKey_D;
	case 'E': return EKey_E;
	case 'F': return EKey_F;
	case 'G': return EKey_G;
	case 'H': return EKey_H;
	case 'I': return EKey_I;
	case 'J': return EKey_J;
	case 'K': return EKey_K;
	case 'L': return EKey_L;
	case 'M': return EKey_M;
	case 'N': return EKey_N;
	case 'O': return EKey_O;
	case 'P': return EKey_P;
	case 'Q': return EKey_Q;
	case 'R': return EKey_R;
	case 'S': return EKey_S;
	case 'T': return EKey_T;
	case 'U': return EKey_U;
	case 'V': return EKey_V;
	case 'W': return EKey_W;
	case 'X': return EKey_X;
	case 'Y': return EKey_Y;
	case 'Z': return EKey_Z;

	case VK_NUMLOCK: return EKey_NumLock;
	case VK_NUMPAD0: return EKey_Numpad0;
	case VK_NUMPAD1: return EKey_Numpad1;
	case VK_NUMPAD2: return EKey_Numpad2;
	case VK_NUMPAD3: return EKey_Numpad3;
	case VK_NUMPAD4: return EKey_Numpad4;
	case VK_NUMPAD5: return EKey_Numpad5;
	case VK_NUMPAD6: return EKey_Numpad6;
	case VK_NUMPAD7: return EKey_Numpad7;
	case VK_NUMPAD8: return EKey_Numpad8;
	case VK_NUMPAD9: return EKey_Numpad9;

	case VK_F1:  return EKey_F1;
	case VK_F2:  return EKey_F2;
	case VK_F3:  return EKey_F3;
	case VK_F4:  return EKey_F4;
	case VK_F5:  return EKey_F5;
	case VK_F6:  return EKey_F6;
	case VK_F7:  return EKey_F7;
	case VK_F8:  return EKey_F8;
	case VK_F9:  return EKey_F9;
	case VK_F10: return EKey_F10;
	case VK_F11: return EKey_F11;
	case VK_F12: return EKey_F12;

	case VK_DECIMAL:  return EKey_Decimal;
	case VK_PRIOR:    return EKey_PageUp;
	case VK_NEXT:     return EKey_PageDown;
	case VK_SPACE:    return EKey_Space;
	case VK_RETURN:   return EKey_Enter;
	case VK_BACK:     return EKey_Backspace;
	case VK_TAB:      return EKey_Tab;
	case VK_SNAPSHOT: return EKey_PrintScreen;
	case VK_INSERT:   return EKey_Insert;
	case VK_DELETE:   return EKey_Delete;
	case VK_DIVIDE:   return EKey_Divide;
	case VK_MULTIPLY: return EKey_Multipy;
	case VK_SUBTRACT: return EKey_Subtract;
	case VK_ADD:      return EKey_Addition;
	case VK_HOME:     return EKey_Home;
	case VK_END:      return EKey_End;
	case VK_ESCAPE:   return EKey_Escape;
	case VK_CAPITAL:  return EKey_CapsLock;
	case VK_UP:       return EKey_ArrowUp;
	case VK_DOWN:     return EKey_ArrowDown;
	case VK_LEFT:     return EKey_ArrowLeft;
	case VK_RIGHT:    return EKey_ArrowRight;

	case VK_LCONTROL: return EKey_LeftCtrl;
	case VK_LSHIFT:	  return EKey_LeftShift;
	case VK_LMENU:	  return EKey_LeftAlt;
	case VK_RCONTROL: return EKey_RightCtrl;
	case VK_RSHIFT:	  return EKey_RightShift;
	case VK_RMENU:	  return EKey_RightAlt;

	case VK_OEM_102:
	case VK_OEM_NEC_EQUAL:
	case VK_OEM_PLUS:
	case VK_OEM_COMMA:
	case VK_OEM_MINUS:
	case VK_OEM_PERIOD:
	case VK_OEM_1:
	case VK_OEM_2:
	case VK_OEM_3:
	case VK_OEM_4:
	case VK_OEM_5:
	case VK_OEM_6:
	case VK_OEM_7:
	case VK_OEM_8:
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		UINT CharCode = MapVirtualKey(OSKey, MAPVK_VK_TO_CHAR);
		bool bShift = false;
		if ((GetKeyState(VK_SHIFT) & 0x8000))
			bShift = true;
#define MAS_CHAR_KEY(UNSHIFTED, SHIFTED, UNSHIFTED_KEY, SHIFTED_KEY) case UNSHIFTED: if(bShift) return SHIFTED_KEY; else return UNSHIFTED_KEY; break
		switch (CharCode)
		{
			MAS_CHAR_KEY(';', ':', EKey_SemiColon, EKey_Colon);
			MAS_CHAR_KEY('\'', '\"', EKey_Apostrophe, EKey_DoubleQuote);
			MAS_CHAR_KEY('[', '{', EKey_LeftSquareBracket, EKey_LeftCurlyBrace);
			MAS_CHAR_KEY(']', '}', EKey_RightSquareBracket, EKey_RightCurlyBrace);
			MAS_CHAR_KEY('\\', '|', EKey_BackSlash, EKey_VerticalBar);
			MAS_CHAR_KEY(',', '<', EKey_Comma, EKey_LessThan);
			MAS_CHAR_KEY('.', '>', EKey_Period, EKey_GreaterThan);
			MAS_CHAR_KEY('/', '?', EKey_ForwardSlash, EKey_QuestionMark);
			MAS_CHAR_KEY('`', '~', EKey_GraveAccent, EKey_Tilde);
			MAS_CHAR_KEY('1', '!', EKey_Num1, EKey_ExclamationMark);
			MAS_CHAR_KEY('2', '@', EKey_Num2, EKey_At);
			MAS_CHAR_KEY('3', '#', EKey_Num3, EKey_Hash);
			MAS_CHAR_KEY('4', '$', EKey_Num4, EKey_Dollar);
			MAS_CHAR_KEY('5', '%', EKey_Num5, EKey_Percent);
			MAS_CHAR_KEY('6', '^', EKey_Num6, EKey_Caret);
			MAS_CHAR_KEY('7', '&', EKey_Num7, EKey_Ampersand);
			MAS_CHAR_KEY('8', '*', EKey_Num8, EKey_Asterisk);
			MAS_CHAR_KEY('9', '(', EKey_Num9, EKey_LeftParenthesis);
			MAS_CHAR_KEY('0', ')', EKey_Num0, EKey_RightParenthesis);
			MAS_CHAR_KEY('-', '_', EKey_Hyphen, EKey_UnderScore);
			MAS_CHAR_KEY('=', '+', EKey_Equals, EKey_Plus);
		}
#undef MAS_CHAR_KEY
	}
	break;
	}

	return EKey_Unknown;
}
#endif