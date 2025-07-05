#include "GameAPI/masEngineAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <DirectXMath.h>
using namespace DirectX;

#include "masD3D11.h"


/**************************************************************************************
*
***************************************************************************************/
bool tut4_init();
void tut4_cleanup();
void tut4_render();

MAS_GAME_API bool masStart()
{
	void    *WindowHandle = masEngine_Window_GetHandle();
	int32_t  Width        = 400;
	int32_t  Height       = 400;
	masEngine_Window_SetSize(Width, Height);

    if(!masEngine_D3D11_Init(WindowHandle, Width, Height))
	    return false;

	if(!tut4_init())
	{
		printf("tut4_init failed\n");
		return false;
	}

	printf("\nTHIS IS GAME OF BYTES & BITS \n");
	printf("Hello my angel\n");


	return true;
}

MAS_GAME_API void masTick()
{
	masEngine_D3D11_Clear(NULL, 40, 100, 70, 255);

	tut4_render();

    masEngine_D3D11_Present(0, true);
}

MAS_GAME_API void masStop()
{
	printf("\nTHIS IS GAME SAYING GOODBAY\n");
	tut4_cleanup();
	masEngine_D3D11_DeInit();
}





/**************************************************************************************
*
***************************************************************************************/
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld, mView, mProjection;
};

struct tut4
{
	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3D11PixelShader>  pPixelShader;
	ComPtr<ID3D11InputLayout>  pVertexLayout;
	ComPtr<ID3D11Buffer>       pVertexBuffer;
	ComPtr<ID3D11Buffer>       pIndexBuffer;
	ComPtr<ID3D11Buffer>       pConstantBuffer;
	XMMATRIX mWorld, mView, mProjection;
};
static tut4 t4 = {};

bool tut4_init()
{
	ComPtr<ID3D11Device> pDevice = masEngine_D3D11_GetDevice();

	// Load Vertex Shader
	ComPtr<ID3DBlob> pVSBlob = NULL;
	if(!masEngine_D3D11_CompileShaderFromFile(&pVSBlob, L"projects/eng_test/assets/shader/tut4.fx", "VS", "vs_4_0"))
		return false;
	HRESULT hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL,
        t4.pVertexShader.GetAddressOf());
	if(FAILED(hr))
		return false;

    // Create Geometry Data Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	hr = pDevice->CreateInputLayout(layout, _countof(layout), pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), t4.pVertexLayout.GetAddressOf());
	if(FAILED(hr))
		return false;


	// Load Pixel Shader
	ComPtr<ID3DBlob> pPSBlob = NULL;
	if(!masEngine_D3D11_CompileShaderFromFile(&pPSBlob, L"projects/eng_test/assets/shader/tut4.fx", "PS", "ps_4_0"))
	    return false;
    hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, t4.pPixelShader.GetAddressOf());
	if(FAILED(hr))
	    return false;

    // Create Geometry Data & Upload it 	
	SimpleVertex vertices[] =
    {
        { XMFLOAT3( -1.0f,  1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3(  1.0f,  1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3(  1.0f,  1.0f,  1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f,  1.0f,  1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3(  1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3(  1.0f, -1.0f,  1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f,  1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
    };
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = pDevice->CreateBuffer(&bd, &InitData, t4.pVertexBuffer.GetAddressOf());
	if(FAILED(hr))
	    return false;
	

	// Create Vertices's Indices
	uint32_t indices[] = 
	{
		3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
	};
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(uint32_t) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = pDevice->CreateBuffer(&bd, &InitData, t4.pIndexBuffer.GetAddressOf());
	if(FAILED(hr))
	    return false;

	
	// Create Transform Constant Buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, NULL, t4.pConstantBuffer.GetAddressOf());
	if(FAILED(hr))
	    return false;

	
	// Initialize Transform Data
	t4.mWorld = XMMatrixIdentity();
    
	float AspectRatio = 0.f;
	masEngine_Window_GetAspectRatio(&AspectRatio);
	XMVECTOR Eye = XMVectorSet(0.f, 1.f, -5.f, 0.f);
	XMVECTOR At = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	t4.mView = XMMatrixLookAtLH(Eye, At, Up);
    t4.mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio, 0.01f, 100.f);

	return true;
}


void tut4_cleanup()
{
    if(t4.pVertexShader)   t4.pVertexShader->Release();  
    if(t4.pPixelShader)    t4.pPixelShader->Release();    
    if(t4.pVertexLayout)   t4.pVertexLayout->Release();    
    if(t4.pVertexBuffer)   t4.pVertexBuffer->Release();    
    if(t4.pIndexBuffer)    t4.pIndexBuffer->Release();    
    if(t4.pConstantBuffer) t4.pConstantBuffer->Release(); 
}

void tut4_render()
{
	ComPtr<ID3D11DeviceContext> pDeviceContext = masEngine_D3D11_GetImmediateContext();

	double t = masEngine_Time();

	t4.mWorld = XMMatrixRotationY(t);
    

	uint32_t stride = sizeof(SimpleVertex);
	uint32_t offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, t4.pVertexBuffer.GetAddressOf(), &stride, &offset);
    pDeviceContext->IASetIndexBuffer(t4.pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    pDeviceContext->IASetInputLayout(t4.pVertexLayout.Get());
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Update Transform Constant Buffer
	ConstantBuffer cb = {};
	cb.mWorld = XMMatrixTranspose(t4.mWorld);
	cb.mView  = XMMatrixTranspose(t4.mView);
	cb.mProjection = XMMatrixTranspose(t4.mProjection);
	pDeviceContext->UpdateSubresource(t4.pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);

    
	pDeviceContext->VSSetShader(t4.pVertexShader.Get(), NULL, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, t4.pConstantBuffer.GetAddressOf());
	pDeviceContext->PSSetShader(t4.pPixelShader.Get(), NULL, 0);
	pDeviceContext->DrawIndexed(36, 0, 0);
}