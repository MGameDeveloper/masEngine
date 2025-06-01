#pragma once

#include <stdint.h>

struct EngC_Gamepad
{
	struct
	{
		float LTrigger, RTrigger;
		float LAnalogY, LAnalogX;
		float RAnalogY, RAnalogX;
	}Axes;

	struct
	{
		float LTrigger;
		float RTrigger;
	}Threshold;

	struct
	{
		float LAnalog;
		float RAnalog;
	}Deadzone;

	struct
	{
		float Duration;
		short LeftMotorSpeed;
		short RightMotorSpeed;
	}Feedback;

	bool  LastState[24];
	bool  bConnected;
};

bool EngC_Input_Init();
void EngC_Input_CheckDevices();
void EngC_Input_Update();

int32_t EngC_Input_GetModKeyState();



/*
* Mappers
*/
int32_t EngC_Input_MapKeyboardKey(int32_t OSKey);


/*
* Gamepad
*/
const EngC_Gamepad* EngC_Input_GetGamepad(int32_t GamepadIdx);