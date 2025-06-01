#pragma once

#include <stdint.h>

enum masEngine_EInputDevice
{
    EInputDevice_Keyboard,
    EInputDevice_Mouse,
    EInputDevice_Controller,

    EInputDevice_Count
};

int32_t masEngine_Input_MapKey(masEngine_EInputDevice InputDevice, int32_t KeyCode);