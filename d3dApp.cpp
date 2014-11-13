#include "d3dApp.h"
#include <Windows.h>
#include <sstream>

namespace
{
	D3DApp* gd3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam)
{
	return gd3dApp->MsgProc(hwnd,msg,wParam,lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
	:mhAppInst(hInstance),
	mMainWndCaption(L"D3D11 Application"),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientHeight(600),
	mClientWidth(800),
	mEnable4xMsaa(false),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mResizing(false),
	m4xMsaaQuality(0),
	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mRenderTargetView(0),
	mDepthStencilView(0)
{
	ZeroMemory(&mScreenViewPort, sizeof(D3D11_VIEWPORT));

	gd3dApp = this;
}


D3DApp::~D3DApp()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	if(md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
}


HINSTANCE D3DApp::AppInst()const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd()const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(mClientWidth)/mClientHeight;
}

/// <summary>
/// 启动
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
int D3DApp::Run()
{
	MSG msg = {0};
	int i = 0;

	//消息处理，处于无限循环
	while (msg.message != WM_QUIT)
	{
		i++;
		printf("the frame is %d",i);
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else
		{
			mTimer.Tick();
			if(!mAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				DrawScene(); //每一帧都调用一次drawScene()
			}else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

bool D3DApp::Init()
{
	if(!InitMainWindow())
	{
		return false;
	}
	if(!InitDirect3D())
		return false;
}

void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	//resize之后要清除掉旧的视图
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	//数据交换链要resize，并重新创建target view
	HR(mSwapChain->ResizeBuffers(1,mClientWidth,mClientHeight,DXGI_FORMAT_R8G8B8A8_UNORM,0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer,0,&mRenderTargetView));
	ReleaseCOM(backBuffer);

	//创建深度模板缓冲
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if(mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality-1;
	}else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc,0,&mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer,0,&mDepthStencilView));

	//将renderTargetView绑定到OM阶段。
	md3dImmediateContext->OMSetRenderTargets(1,&mRenderTargetView,mDepthStencilView);

	//设置视口的变换
	mScreenViewPort.TopLeftX = 0;
	mScreenViewPort.TopLeftY = 0;
	mScreenViewPort.Width = static_cast<float>(mClientWidth);
	mScreenViewPort.Height = static_cast<float>(mClientHeight);
	mScreenViewPort.MinDepth = 0.0f;
	mScreenViewPort.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1,&mScreenViewPort);
}
/// <summary>
/// 消息处理函数
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
LRESULT D3DApp::MsgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
		case WM_ACTIVATE:
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				mAppPaused = true;
				mTimer.Stop();
			}else
			{
				mAppPaused = false;
				mTimer.Start();
			}
			return 0;
		case WM_SIZE:
			mClientHeight = LOWORD(lParam);
			mClientWidth = LOWORD(lParam);
			if(md3dDevice)
			{
				if(wParam == SIZE_MINIMIZED)
				{
					mAppPaused = true;
					mMinimized = true;
					mMaximized = false;
				}else if(wParam == SIZE_MAXIMIZED)
				{
					mAppPaused = false;
					mMinimized = false;
					mMaximized = true;
					OnResize();
				}else if(wParam == SIZE_RESTORED)
				{
					if(mMinimized)
					{
						mAppPaused = false;
						mMinimized = false;
						OnResize();
					}else if(mMaximized)
					{
						mAppPaused = false;
						mMaximized = false;
						OnResize();
					}else if(mResizing)
					{
						
					}else
					{
						OnResize();
					}
				}
			}
			return 0;
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			mResizing = true;
			mTimer.Stop();
			return 0;
		case WM_EXITSIZEMOVE:
			mAppPaused = false;
			mResizing = false;
			mTimer.Start();
			OnResize();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MENUCHAR:
			return MAKELRESULT(0,MNC_CLOSE);
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
			return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	//
	wc.lpfnWndProc  = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0,IDI_APPLICATION);
	wc.hCursor = LoadCursor(0,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if(!RegisterClass(&wc))
	{
		MessageBox(0,L"RegisterClass Failed.",0,0);
		return false;
	}

	int mWidth = mClientWidth;
	int mHeight = mClientHeight;
	RECT R = {0,0,mWidth,mHeight};
	AdjustWindowRect(&R,WS_OVERLAPPEDWINDOW,false);
	int width = R.right- R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName",mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,width,height,0,0,mhAppInst,0);
	if(!mhMainWnd)
	{
		MessageBox(0,L"CreateWindow Failed.",0,0);
		return false;
	}
	ShowWindow(mhMainWnd,SW_SHOW);
	mClientHeight = mHeight;
	UpdateWindow(mhMainWnd);

	return true;
}

//初始化D3D对象。device,
bool D3DApp::InitDirect3D()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0, //默认的视频卡
		md3dDriverType,0,
		createDeviceFlags,
		0,0,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);
	if(FAILED(hr))
	{
		MessageBoxW(0,L"D3D11CreateDevice Failed.",0,0);
		return false;
	}

	if(featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0,L"Direct3D Feature Level 11 unsupported.",0,0);
		return false;
	}

	//检查显卡对4X MSAA质量的支持，所有支持D3D 11的显卡都支持4X MSAA，所以只要检查支持的等级。
	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,4,&m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if(mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality -1;
	}
	else
	{
		sd.SampleDesc.Count= 1;
		sd.SampleDesc.Quality =0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//要正确的创建数据交换链，必须用创建device的IDXGIFactory，如果使用不同的IDXGIFactory，就会出现错误：IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice),(void**)&dxgiDevice));
	
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory),(void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice,&sd,&mSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	//每次resize之后都要调用OnResize函数，所以这里也调用，避免代码复制。
	OnResize();
	return true;

}

//计算每秒内的帧频，还有每秒的平均时间，并将其显示在窗口的标题栏上
void D3DApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;
	if((mTimer.TotalTime()-timeElapsed)>=1000.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f/fps;
		std::wostringstream outs;
		outs.precision(6);
		outs<< mMainWndCaption << L"    "
			<<L"FPS: "<< fps << L"    "
			<<L"Frame Time: "<< mspf << L" (ms)";
		SetWindowText(mhMainWnd,outs.str().c_str());
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}