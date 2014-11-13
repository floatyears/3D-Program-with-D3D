#pragma comment(lib,"d3dcompiler")

#include "d3dApp.h"
#include "d3dx11effect.h"
#include "MathHelper.h"

#include <D3Dcompiler.h>
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class BoxApp:public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeomeryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	/// <summary>
	/// 顶点缓冲
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	/// <summary>
	/// x轴方向旋转的角度
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	float mTheta;
	/// <summary>
	/// y轴方向旋转的角度
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	float mPhi;

	/// <summary>
	/// 旋转的半径
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	float mRadius;

	POINT mLastMousePos;
};

/// <summary>
/// Insert description of method
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine,int showCmd)
{
	// Enable run-time memory check for debug builds.debug下允许运行时内存检查
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	BoxApp theApp(hInstance);
	if(!theApp.Init())
	{
		return 0;
	}
	return theApp.Run();
};

BoxApp::BoxApp(HINSTANCE hInstance):D3DApp(hInstance),mBoxVB(0),mBoxIB(0),mFX(0),mTech(0),mfxWorldViewProj(0),mInputLayout(0),mTheta(1.5f*MathHelper::Pi),mPhi(0.25f*MathHelper::Pi),mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld,I);
	XMStoreFloat4x4(&mView,I);
	XMStoreFloat4x4(&mProj,I);
}

BoxApp::~BoxApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool BoxApp::Init()
{
	if(!D3DApp::Init())
	{
		return false;
	}

	BuildGeomeryBuffers();
	BuildFX();
	BuildVertexLayout();
	return true;

}

void BoxApp::OnResize()
{
	D3DApp::OnResize();
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi,AspectRatio(),1.0f,1000.0f);
	XMStoreFloat4x4(&mProj,P);
}

/// <summary>
/// 根据鼠标移动更新视图矩阵
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
void BoxApp::UpdateScene(float dt)
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x,y,z,1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

	//Builds a view matrix for a left-handed coordinate system using a camera position, an up direction, and a focal point.
	//使用相机位置，向上的方向，和焦点创建一个view matrix
	XMMATRIX V = XMMatrixLookAtLH(pos,target,up);
	XMStoreFloat4x4(&mView,V);
}


void BoxApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strid = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0,1,&mBoxVB,&strid,&offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB,DXGI_FORMAT_R32_UINT,0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		//将pass中的状态设置到device中
		mTech->GetPassByIndex(p)->Apply(0,md3dImmediateContext);
		
		//一个盒子有36个点
		md3dImmediateContext->DrawIndexed(36,0,0);
	}

	HR(mSwapChain->Present(0,0));
};

void BoxApp::OnMouseDown(WPARAM btnState,int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::OnMouseUp(WPARAM btnState,int x,int y)
{
	ReleaseCapture();
}
/// <summary>
/// 鼠标移动时，
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	//旋转
	if((btnState & MK_LBUTTON ) != 0)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x-mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y-mLastMousePos.y));
	
		mTheta +=dx;
		mPhi += dy;

		//如果不限制角度，y会产生负值，导致旋转的时候会产生跳跃。
		mPhi = MathHelper::Clamp(mPhi,0.1f,MathHelper::Pi-0.1f);
	}
	//缩放
	else if((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.005f*static_cast<float>(x-mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y-mLastMousePos.y);

		mRadius += dx- dy;

		mRadius = MathHelper::Clamp(mRadius,3.0f,15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
/// <summary>
/// 创建几何缓冲
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
void BoxApp::BuildGeomeryBuffers()
{
	//创建顶点缓冲
	Vertex vertics[] = 
	{
		{XMFLOAT3(-1.0f,-1.0f,-1.0f),(const float*)&Colors::White},
		{XMFLOAT3(-1.0f,1.0f,-1.0f),(const float*)&Colors::Black},
		{XMFLOAT3(1.0f,1.0f,-1.0f),(const float*)&Colors::Red},
		{XMFLOAT3(1.0f,-1.0f,-1.0f),(const float*)&Colors::Green},
		{XMFLOAT3(-1.0f,-1.0f,1.0f),(const float*)&Colors::Blue},
		{XMFLOAT3(-1.0f,-1.0f,1.0f),(const float*)&Colors::Yellow},
		{XMFLOAT3(1.0f,1.0f,1.0f),(const float*)&Colors::Cyan},
		{XMFLOAT3(1.0f,-1.0f,1.0f),(const float*)&Colors::Magenta}
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)*8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags =0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertics;
	HR(md3dDevice->CreateBuffer(&vbd,&vinitData,&mBoxVB));

	//创建索引缓冲
	UINT indices[] = 
	{
		//front 面
		0,1,2,
		0,2,3,

		//back face
		4,6,5,
		4,7,6,

		//left face
		4,5,1,
		4,1,0,

		//right face
		3,2,6,
		3,6,7,

		//top face
		1,5,6,
		1,6,2,

		//bottom face
		4,0,3,
		4,3,7
	};

	//创建索引缓冲
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT)*36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd,&iinitData,&mBoxIB));

};

/// <summary>
/// 创建效果
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
void BoxApp::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx",0,0,0,"fx_5_0",shaderFlags,0,0,&compiledShader,&compilationMsgs,0);

	if(compilationMsgs != 0)
	{
		MessageBoxA(0,(char*)compilationMsgs->GetBufferPointer(),0,0);
		ReleaseCOM(compilationMsgs);
	}

	if(FAILED(hr))
	{
		DXTrace(__FILE__,(DWORD)__LINE__,hr,L"D3DX11CompileFromFile",true);
	}

	//从二进制的effect或者file创建一个effect
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),compiledShader->GetBufferSize(),0,md3dDevice,&mFX));

	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

/// <summary>
/// 创建输入布局
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
void BoxApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	//根据effectTech获取到pass，根据pass获取到输入签名，根据签名创建输入布局
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc,2,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize,&mInputLayout));
}
