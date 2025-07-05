#pragma once

#include <stdint.h>

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <D3DCompiler.h>

using Microsoft::WRL::ComPtr;


/*****************************************************************************************************
*
******************************************************************************************************/
bool masEngine_D3D11_Init(void* Handle, int32_t Width, int32_t Height);
void masEngine_D3D11_DeInit();

ComPtr<ID3D11Device>        masEngine_D3D11_GetDevice();
ComPtr<ID3D11DeviceContext> masEngine_D3D11_GetImmediateContext();
bool                        masEngine_D3D11_Present(uint32_t Interval, bool AllowTearing);
void                        masEngine_D3D11_Clear(ComPtr<ID3D11RenderTargetView>* Target, uint8_t R, uint8_t G, uint8_t B, uint8_t A);

bool masEngine_D3D11_CompileShaderFromFile  (ComPtr<ID3DBlob>* Out, const wchar_t* FilePath, const char* EntryPoint, const char* ShaderModel);
bool masEngine_D3D11_CompileShaderFromSource(ComPtr<ID3DBlob>* Out, const wchar_t* Source, const char* EntryPoint, const char* ShaderModel);
