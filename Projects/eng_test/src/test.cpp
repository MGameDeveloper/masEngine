#include "GameAPI/masEngineAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectxMath.h>
#include <D3DCompiler.h>


using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct Dx11
{
	ComPtr<IDXGISwapChain1>     pSwapChain1;
	ComPtr<ID3D11Device>        pDevice;
	ComPtr<ID3D11DeviceContext> pImmediateContext;

	ComPtr<ID3D11RenderTargetView> pRTV;

	D3D_FEATURE_LEVEL FeatureLevel;
	bool bTearingSupport;
};
static Dx11 dx = {};

bool init_dx()
{
	uint32_t bTearing = 0;
	uint32_t CreateFactoryFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
    CreateFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    ComPtr<IDXGIFactory2> pFactory2 = NULL;
	HRESULT hr = CreateDXGIFactory2(CreateFactoryFlag, IID_PPV_ARGS(&pFactory2));
	if(FAILED(hr))
	{
		printf("Creating DXGIFactory2 Failed\n");
		return false;
	}
	ComPtr<IDXGIFactory7> pFactory7 = NULL;
	hr = pFactory2.As(&pFactory7);
	if(FAILED(hr))
	{
		printf("Acquiring DXGIFactory7 Failed\n");
		return false;
	}
	hr = pFactory7->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bTearing, sizeof(uint32_t));
	if(FAILED(hr))
	    printf("NO_TEARING_SUPPORT\n");

    
	ComPtr<IDXGIAdapter4> pAdapter4 = NULL;
	hr = pFactory7->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter4));
	if(FAILED(hr))
	{
		printf("ADAPTER_AQUIRE_FAILED\n");
		return false;
	}
    

    uint32_t CreateDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Required for Direct2D interoperability with Direct3D resources.
#if defined(DEBUG) || defined(_DEBUG)
    CreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL FeatureLevelList[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	D3D_FEATURE_LEVEL           FeatureLevel;
	ComPtr<ID3D11Device>        pDevice           = NULL;
	ComPtr<ID3D11DeviceContext> pImmediateContext = NULL;
	hr = D3D11CreateDevice(pAdapter4.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, CreateDeviceFlag, FeatureLevelList,
	    _countof(FeatureLevelList), D3D11_SDK_VERSION, &pDevice, &FeatureLevel, &pImmediateContext);
	if(FAILED(hr))
	{
		printf("DEVICE_CREATION_FAILED\n");
		return false;
	}


	DXGI_SWAP_CHAIN_DESC1 sc_desc =
	{
		0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, FALSE, {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT, 3, DXGI_SCALING_NONE,
		DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, DXGI_ALPHA_MODE_UNSPECIFIED,
		(bTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
	};
	masEngine_Window_GetClientSize((int32_t*)&sc_desc.Width, (int32_t*)&sc_desc.Height);
    ComPtr<IDXGISwapChain1> pSwapChain1 = NULL;
	hr = pFactory7->CreateSwapChainForHwnd(pDevice.Get(), (HWND)masEngine_Window_GetHandle(), &sc_desc, NULL, NULL, pSwapChain1.GetAddressOf());
	if(FAILED(hr))
	{
		printf("SWAPCHAIN_CREATION_FAILED\n");
		return false;
	}


	ComPtr<ID3D11Texture2D> pBackBuffer = NULL;
	hr = pSwapChain1->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if(FAILED(hr))
	{
		printf("BAKC_BUFFER_ACQUIRE_FAILED\n");
		return false;
	}


	ComPtr<ID3D11RenderTargetView> pRTV = NULL;
	hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRTV);
	if(FAILED(hr))
	{
		printf("CREATE_RENDER_TARGET_VIEW_FAILED\n");
		return false;
	}

    
	//
	dx.pSwapChain1       = pSwapChain1;
	dx.pDevice           = pDevice;
	dx.pImmediateContext = pImmediateContext;
	dx.pRTV              = pRTV;
	dx.bTearingSupport   = bTearing;
	dx.FeatureLevel      = FeatureLevel;

	return true;
}


/**************************************************************************************
*
***************************************************************************************/
HRESULT tut4_init();
void tut4_cleanup();
void tut4_render();

MAS_GAME_API bool masStart()
{
	masEngine_Window_SetSize(450, 300);

	if(!init_dx())
	    return false;

	HRESULT hr = tut4_init();
	if(FAILED(hr))
	{
		printf("tut4_init failed: %d\n", hr);
		return false;
	}

	printf("\nTHIS IS GAME OF BYTES & BITS \n");
	printf("Hello my angel\n");
	return true;
}

MAS_GAME_API void masTick()
{
	//printf("TICK_START\n");

	int32_t W, H;
	masEngine_Window_GetClientSize(&W, &H);
	D3D11_VIEWPORT Viewport = 
	{
		0, 0, (float)W, (float)H, 0.f, 1.f
	};

	dx.pImmediateContext->OMSetRenderTargets(1, dx.pRTV.GetAddressOf(), NULL);
	dx.pImmediateContext->RSSetViewports(1, &Viewport);

	float ClearColor[4] = {0.2, 0.2, 0.3, 1.f};
	dx.pImmediateContext->ClearRenderTargetView(dx.pRTV.Get(), ClearColor);
	

	tut4_render();


    DXGI_PRESENT_PARAMETERS PresentParams = {};
    HRESULT hr = dx.pSwapChain1->Present1(0, (dx.bTearingSupport) ? DXGI_PRESENT_ALLOW_TEARING : 0, &PresentParams);
    if(FAILED(hr))
	{
		printf("SWAP_CHAIN_PRESENT_FAILED\n");
	}
	//else 
	//    printf("SWAPPING_SUCCESS\n");;
}

MAS_GAME_API void masStop()
{
	printf("\nTHIS IS GAME SAYING GOODBAY\n");
	tut4_cleanup();
	dx.pSwapChain1       ->Release();
	dx.pDevice           ->Release();
	dx.pImmediateContext ->Release();
	dx.pRTV              ->Release();
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

HRESULT compile_shader_from_file(const wchar_t* FileName, const char* EntryPoint, const char* ShaderModel, ComPtr<ID3DBlob>* pBlobOut)
{
    uint32_t ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> pErrorBlob = NULL;
	HRESULT hr = D3DCompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0,
	    pBlobOut->GetAddressOf(), pErrorBlob.GetAddressOf());
	if(FAILED(hr))
	{
		if(pErrorBlob)
		    printf("%s", (char*)pErrorBlob->GetBufferPointer());
		return hr;
	}

	return S_OK;
}

HRESULT tut4_init()
{
	// Load Vertex Shader
	ComPtr<ID3DBlob> pVSBlob = NULL;
	HRESULT hr = compile_shader_from_file(L"projects/eng_test/assets/shader/tut4.fx", "VS", "vs_4_0", &pVSBlob);
	if(FAILED(hr))
		return hr;
	hr = dx.pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL,
        t4.pVertexShader.GetAddressOf());
	if(FAILED(hr))
		return hr;

    // Create Geometry Data Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	hr = dx.pDevice->CreateInputLayout(layout, _countof(layout), pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), t4.pVertexLayout.GetAddressOf());
	if(FAILED(hr))
		return hr;


	// Load Pixel Shader
	ComPtr<ID3DBlob> pPSBlob = NULL;
	hr = compile_shader_from_file(L"projects/eng_test/assets/shader/tut4.fx", "PS", "ps_4_0", &pPSBlob);
	if(FAILED(hr))
	    return hr;
    hr = dx.pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, t4.pPixelShader.GetAddressOf());
	if(FAILED(hr))
	    return hr;

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
	hr = dx.pDevice->CreateBuffer(&bd, &InitData, t4.pVertexBuffer.GetAddressOf());
	if(FAILED(hr))
	    return hr;
	

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
	hr = dx.pDevice->CreateBuffer(&bd, &InitData, t4.pIndexBuffer.GetAddressOf());
	if(FAILED(hr))
	    return hr;

	
	// Create Transform Constant Buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = dx.pDevice->CreateBuffer(&bd, NULL, t4.pConstantBuffer.GetAddressOf());
	if(FAILED(hr))
	    return hr;

	
	// Initialize Transform Data
	t4.mWorld = XMMatrixIdentity();
    
	float AspectRatio = 0.f;
	masEngine_Window_GetAspectRatio(&AspectRatio);
    printf("AspectRatio: %f\n", AspectRatio);
	XMVECTOR Eye = XMVectorSet(0.f, 1.f, -5.f, 0.f);
	XMVECTOR At = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	t4.mView = XMMatrixLookAtLH(Eye, At, Up);
    t4.mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio, 0.01f, 100.f);

	return S_OK;
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
	static double t = masEngine_Time();

	t4.mWorld = XMMatrixRotationY(t);
    

	uint32_t stride = sizeof(SimpleVertex);
	uint32_t offset = 0;
	dx.pImmediateContext->IASetVertexBuffers(0, 1, &t4.pVertexBuffer, &stride, &offset);
    dx.pImmediateContext->IASetIndexBuffer(t4.pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dx.pImmediateContext->IASetInputLayout(t4.pVertexLayout.Get());
    dx.pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Update Transform Constant Buffer
	ConstantBuffer cb = {};
	cb.mWorld = XMMatrixTranspose(t4.mWorld);
	cb.mView  = XMMatrixTranspose(t4.mView);
	cb.mProjection = XMMatrixTranspose(t4.mProjection);
	dx.pImmediateContext->UpdateSubresource(t4.pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);

    
	dx.pImmediateContext->VSSetShader(t4.pVertexShader.Get(), NULL, 0);
	dx.pImmediateContext->VSSetConstantBuffers(0, 1, t4.pConstantBuffer.GetAddressOf());
	dx.pImmediateContext->PSSetShader(t4.pPixelShader.Get(), NULL, 0);
	dx.pImmediateContext->DrawIndexed(36, 0, 0);
}