#include "masD3D11.h"

#include <stdlib.h>
#include <stdio.h>


/*****************************************************************************************************
*
******************************************************************************************************/
struct masD3D11
{
    ComPtr<IDXGISwapChain1>         pSwapChain1;
    ComPtr<ID3D11Device>            pDevice;
    ComPtr<ID3D11DeviceContext>     pImmediateContext;
    ComPtr<ID3D11RenderTargetView>  pRTV;
	D3D_FEATURE_LEVEL               FeatureLevel;
    int32_t                         Width;
    int32_t                         Height;
	bool                            bTearingSupport;
};
static masD3D11* D3D11 = NULL;


/*****************************************************************************************************
*
******************************************************************************************************/
bool masEngine_D3D11_Init(void* Handle, int32_t Width, int32_t Height)
{
    if(D3D11)
        return true;

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
		(uint32_t)Width, (uint32_t)Height, DXGI_FORMAT_R8G8B8A8_UNORM, FALSE, {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT, 3, DXGI_SCALING_NONE,
		DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, DXGI_ALPHA_MODE_UNSPECIFIED,
		(bTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
	};
    ComPtr<IDXGISwapChain1> pSwapChain1 = NULL;
	hr = pFactory7->CreateSwapChainForHwnd(pDevice.Get(), (HWND)Handle, &sc_desc, NULL, NULL, pSwapChain1.GetAddressOf());
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

    
    D3D11 = (masD3D11*)malloc(sizeof(masD3D11));
    if(!D3D11)
        return false;
    memset(D3D11, 0, sizeof(masD3D11));

	//
	D3D11->pSwapChain1       = pSwapChain1;
	D3D11->pDevice           = pDevice;
	D3D11->pImmediateContext = pImmediateContext;
	D3D11->pRTV              = pRTV;
	D3D11->bTearingSupport   = bTearing;
	D3D11->FeatureLevel      = FeatureLevel;
    D3D11->Width             = Width;
    D3D11->Height            = Height;

	return true;
}

void masEngine_D3D11_DeInit()
{
    if(!D3D11)
        return;
    if(D3D11->pSwapChain1)       D3D11->pSwapChain1->Release();
	if(D3D11->pDevice)           D3D11->pDevice->Release();
	if(D3D11->pImmediateContext) D3D11->pImmediateContext->Release();
	if(D3D11->pRTV)              D3D11->pRTV->Release();
}

ComPtr<ID3D11Device> masEngine_D3D11_GetDevice()
{
    return (D3D11) ? D3D11->pDevice : NULL;
}

ComPtr<ID3D11DeviceContext> masEngine_D3D11_GetImmediateContext()
{
    return (D3D11) ? D3D11->pImmediateContext : NULL;
}

bool masEngine_D3D11_Present(uint32_t Interval, bool AllowTearing)
{
    if(!D3D11)
        return false;
    
    DXGI_PRESENT_PARAMETERS PresentParams = {};
    HRESULT hr = D3D11->pSwapChain1->Present1(Interval, (AllowTearing && D3D11->bTearingSupport) ? DXGI_PRESENT_ALLOW_TEARING : 0, &PresentParams);
    if(FAILED(hr))
	{
		printf("SWAP_CHAIN_PRESENT_FAILED[ HRESULT = 0x%08lX ]\n", hr);
        return false;
	}

    return true;
}

void masEngine_D3D11_Clear(ComPtr<ID3D11RenderTargetView>* Target, uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	D3D11_VIEWPORT Viewport = 
	{
		0, 0, (float)D3D11->Width, (float)D3D11->Height, 0.f, 1.f
	};
    
	D3D11->pImmediateContext->OMSetRenderTargets(1, D3D11->pRTV.GetAddressOf(), NULL);
	D3D11->pImmediateContext->RSSetViewports(1, &Viewport);

    static float F = 1.f/255;
    
    ID3D11RenderTargetView* pRTV = (Target) ? (*Target).Get() : D3D11->pRTV.Get();

	float ClearColor[4] = { R * F, G * F, B * F, A * F };
	D3D11->pImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
}


bool masEngine_D3D11_CompileShaderFromFile(ComPtr<ID3DBlob>* Out, const wchar_t* FilePath, const char* EntryPoint, const char* ShaderModel)
{
    uint32_t ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> pErrorBlob  = NULL;
	HRESULT hr = D3DCompileFromFile(FilePath, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0, &(*Out), &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob)
		    printf("%s", (char*)pErrorBlob->GetBufferPointer());
		return false;
	}

	return true;
}

bool masEngine_D3D11_CompileShaderFromSource(ComPtr<ID3DBlob>* Out, const wchar_t* Source, uint64_t SizeInByte, const char* EntryPoint, const char* ShaderModel)
{
    if(!Out)
        return false;

    uint32_t ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> pErrorBlob  = NULL;
	HRESULT hr = D3DCompile(Source, SizeInByte, NULL, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0, &(*Out), &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob)
		    printf("%s", (char*)pErrorBlob->GetBufferPointer());
		return false;
	}

	return true;
}