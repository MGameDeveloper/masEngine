#if 0
#include "masEngine.h"

#include <stdio.h>

masGame_InitData masGame_GetInitData()
{
	masGame_InitData InitData = {};
	InitData.Name         = L"TestEngine";
	InitData.AssetPath    = L"Assets/";
	InitData.MajorVersion = 1;
	InitData.MinorVersion = 0;
	InitData.Width        = 800;
	InitData.Height       = 600;

	return InitData;
}

bool masGame_Init()
{
	return true;
}

void masGame_DeInit()
{

}

void masGame_Update(double ElapsedTime)
{
	double GameTime = masGame_Time();
	//printf("GameTime: %lf\n", GameTime);
}


/************************************************************************************************
* ENGINE INPUT SERVICE
*************************************************************************************************/
void masGame_Input_OnAction(masGame_Input_Action* Action)
{
	//printf("InputAction:\n");
	//printf("    KEY:   %d\n", Action->Key);
	//printf("    State: %d\n", Action->KeyState);
	//printf("    Mod:   %d\n", Action->KeyMod.Value);
	//printf("    Vec2:  [ %.2f, %.2f ]\n", Action->Vec2.x, Action->Vec2.y);
}

void masGame_Input_OnAxis(masGame_Input_Axis* Axis)
{
	//printf("InputAxis:\n");
	//printf("    KEY: %d\n", Axis->Key);
	//printf("    Mod: %d\n", Axis->KeyMod.Value);
	//printf("    Value: %.5f\n", Axis->Value);
}

void masGame_Input_OnTextEnter(wchar_t Letter)
{
	printf("%lc", Letter);
}



/***********************************************************************************************
* ENGINE SYSATEM EVENT SERVICE
************************************************************************************************/
void masGame_Event_OnSystemEvent(masGame_SystemEvent Event)
{
	//switch (Event.Type)
	//{
	//case ESystemEvent_DeviceChanges:   printf("DevicesChanges\n");  break;
	//case ESystemEvent_Mouse_Move:      printf("MouseMove\n");       break;
	//case ESystemEvent_Mouse_Enter:     printf("MouseEnter\n");      break;
	//case ESystemEvent_Mouse_Leave:     printf("MouseLeave\n");      break;
	//case ESystemEvent_Window_Move:     printf("WindowMove\n");      break;
	//case ESystemEvent_Window_Resize:   printf("WindowResize\n");    break;
	//case ESystemEvent_Window_Maximize: printf("WindowMaximized\n"); break;
	//case ESystemEvent_Window_Minimize: printf("WindowMinimized\n"); break;
	//}
}
#endif