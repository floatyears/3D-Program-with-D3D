#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LigthHelper.h"
#include "Effect.h"
#include "Vertexs.h"
#include "RenderStates.h"
#include "Waves.h"

class BasicTessellation : public D3DApp
{
public:
	BasicTessellation(HINSTANCE hInstance);
	~BasicTessellation();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState,int x, int y);
	void OnMouseUp(WPARAM btnState,int x,int y);
	void OnMouseMove(WPARAM btnState,int x, int y);

private:
	void BuildQuadPathBuffer();
	void BuildQuadPatchBuffer();

private:
	ID3D11Buffer* mQuadPathVB;
	ID3D11Buffer* mQuadPathIB;

	ID3D11Buffer* mQuadPatchVB;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BasicTessellation theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}


BasicTessellation::BasicTessellation(HINSTANCE hInstance)
: D3DApp(hInstance), mQuadPatchVB(0),  
  mEyePosW(0.0f, 0.0f, 0.0f), mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
	mMainWndCaption = L"Basic Tessellation Demo";

	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj,I);
}

BasicTessellation::~BasicTessellation()
{
	md3dImmediateContext->ClearState();

	ReleaseCOM(mQuadPathVB);
	ReleaseCOM(mQuadPatchVB);
	
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool BasicTessellation::Init()
{
	if(!D3DApp::Init())
		return false;

	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	
	//BuildQuadPatchBuffer();
	BuildQuadPathBuffer();

	return true;
}

void BasicTessellation::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj,P);
}

void BasicTessellation::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void BasicTessellation::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH| D3D11_CLEAR_STENCIL,1.0f,0);

	//md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	//md3dImmediateContext->IASetInputLayout(InputLayouts::Pos);
	//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = {0.0f,0.0f,0.0f,0.0f};

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = view*proj;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Pos);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);

	UINT stride = sizeof(Vertex::Pos);
	UINT offset = 0;

	Effects::TessellationFX->SetEyePosW(mEyePosW);
	Effects::TessellationFX->SetFogColor(Colors::Silver);
	Effects::TessellationFX->SetFogStart(15.0f);
	Effects::TessellationFX->SetFogRange(175.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::TessellationFX->TessTech->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; i++)
	{
	//	md3dImmediateContext->IASetVertexBuffers(0,1,&mQuadPatchVB,&stride,&offset);
		md3dImmediateContext->IASetVertexBuffers(0,1,&mQuadPathVB,&stride,&offset);
	//	md3dImmediateContext->IASetIndexBuffer(mQuadPathIB,DXGI_FORMAT_R32_UINT,0);

		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::TessellationFX->SetWorld(world);
		Effects::TessellationFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::TessellationFX->SetWorldViewProj(worldViewProj);
		Effects::TessellationFX->SetTexTransform(XMMatrixIdentity());
		Effects::TessellationFX->SetDiffuseMap(0);

		Effects::TessellationFX->TessTech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);

		md3dImmediateContext->RSSetState(RenderStates::WireFrameRS);
		//md3dImmediateContext->Draw(16,0);
		md3dImmediateContext->Draw(4,0);
	}

	HR(mSwapChain->Present(0,0));
}

void BasicTessellation::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BasicTessellation::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BasicTessellation::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 300.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BasicTessellation::BuildQuadPathBuffer()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3)*4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	XMFLOAT3 vertices[4] = 
	{
		XMFLOAT3(-10.0f, 0.0f, +10.0f),
		XMFLOAT3(+10.0f, 0.0f, +10.0f),
		XMFLOAT3(-10.0f, 0.0f, -10.0f),
		XMFLOAT3(+10.0f, 0.0f, -10.0f)
	};

	D3D11_SUBRESOURCE_DATA vinitDat;
	vinitDat.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd,&vinitDat,&mQuadPathVB));

	UINT indics[] = 
	{
		0,1,2,0,2,3
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indics;
	HR(md3dDevice->CreateBuffer(&ibd,&iinitData,&mQuadPathIB));
}

void BasicTessellation::BuildQuadPatchBuffer()
{
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * 16;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

	XMFLOAT3 vertices[16] = 
	{
		// Row 0
		XMFLOAT3(-10.0f, -10.0f, +15.0f),
		XMFLOAT3(-5.0f,  0.0f, +15.0f),
		XMFLOAT3(+5.0f,  0.0f, +15.0f),
		XMFLOAT3(+10.0f, 0.0f, +15.0f), 

		// Row 1
		XMFLOAT3(-15.0f, 0.0f, +5.0f),
		XMFLOAT3(-5.0f,  0.0f, +5.0f),
		XMFLOAT3(+5.0f,  20.0f, +5.0f),
		XMFLOAT3(+15.0f, 0.0f, +5.0f), 

		// Row 2
		XMFLOAT3(-15.0f, 0.0f, -5.0f),
		XMFLOAT3(-5.0f,  0.0f, -5.0f),
		XMFLOAT3(+5.0f,  0.0f, -5.0f),
		XMFLOAT3(+15.0f, 0.0f, -5.0f), 

		// Row 3
		XMFLOAT3(-10.0f, 10.0f, -15.0f),
		XMFLOAT3(-5.0f,  0.0f, -15.0f),
		XMFLOAT3(+5.0f,  0.0f, -15.0f),
		XMFLOAT3(+25.0f, 10.0f, -15.0f)
	};

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mQuadPatchVB));
}