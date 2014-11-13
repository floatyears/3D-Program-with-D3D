#include "d3dApp.h"
#include "d3dx11effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class HillsApp:public D3DApp
{
public:
	HillsApp(HINSTANCE hInstance);
	~HillsApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState,int x,int y);
	void OnMouseMove(WPARAM btnState,int x,int y);

private:
	float GetHeight(float x, float z)const;
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mGridWorld;
	UINT mGridIndexCount;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE prevInstance, PSTR cmdLine,int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	HillsApp theApp(hInstance);
	if(!theApp.Init())
	{
		return 0;
	}
	return theApp.Run();
}

HillsApp::HillsApp(HINSTANCE hInstance):D3DApp(hInstance),mVB(0),mIB(0),mFX(0),mTech(0),mfxWorldViewProj(0),mInputLayout(0),mGridIndexCount(0),mTheta(1.5f*MathHelper::Pi),mPhi(0.1f*MathHelper::Pi),mRadius(200.0f)
{
	mMainWndCaption = L"Hills Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld,I);
	XMStoreFloat4x4(&mView,I);
	XMStoreFloat4x4(&mProj,I);
}

HillsApp::~HillsApp()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool HillsApp::Init()
{
	if(!D3DApp::Init())
	{
		return false;
	}
	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	return true;
}

void HillsApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi,AspectRatio(),1.0f,1000.0f);
	XMStoreFloat4x4(&mProj,P);

}

void HillsApp::UpdateScene(float dt)
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x,y,z,1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up  = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos,target,up);
	XMStoreFloat4x4(&mView,V);
}

void HillsApp::OnMouseDown(WPARAM btnState,int x,int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
};

void HillsApp::OnMouseMove(WPARAM btnState,int x, int y)
{
	if((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
		
		mTheta += dx;
		mPhi   += dy;

		mPhi = MathHelper::Clamp(mPhi,0.1f,MathHelper::Pi-0.1f);
	}
	else if((btnState & ! MK_RBUTTON)!= 0)
	{
		float dx = 0.2f*static_cast<float>(x-mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y-mLastMousePos.y);

		mRadius += dx - dy;
		mRadius = MathHelper::Clamp(mRadius,50.0f,500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void HillsApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0,1,&mVB,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(mIB,DXGI_FORMAT_R32_UINT,0);

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
	XMMATRIX worldViewProj = world*view*proj;

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount,0,0);
	}
	HR(mSwapChain->Present(0,0));
};



float HillsApp::GetHeight(float x, float z)const
{
	return 0.3f*(z * sinf(0.1f*x) + x*cosf(0.1f*z));
}

void HillsApp::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;
	geoGen.CreateGrid(160.0f,160.0f,50,50,grid);
	mGridIndexCount = grid.Indices.size();

	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); i++)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		p.y = GetHeight(p.x,p.z);

		vertices[i].Pos = p;

		if(p.y < -10.0f)
		{
			vertices[i].Color = XMFLOAT4(1.0f,0.96f,0.62f,1.0f);
		}else if(p.y < 5.0f)
		{
			vertices[i].Color = XMFLOAT4(0.48f,0.77f,0.46f,1.0f);
		}else if(p.y < 12.0f)
		{
			vertices[i].Color = XMFLOAT4(0.1f,0.48f,0.19f,1.0f);
		}else if(p.y < 20.f)
		{
			// Dark brown.
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)*grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd,&vinitData,&mVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags=  0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd,&iinitData,&mIB));
}

void HillsApp::BuildFX()
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


	//std::ifstream fin("fx/color.fxo", std::ios::binary);

	//fin.seekg(0,std::ios_base::end);
	//int size = (int)fin.tellg();

	//fin.seekg(0,std::ios_base::beg);
	//std::vector<char> compiledShader(size);

	//fin.read(&compiledShader[0], size);
	//fin.close();

	//HR(D3DX11CreateEffectFromMemory(&compiledShader[0],size,0,md3dDevice,&mFX));

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void HillsApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc,2,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize,&mInputLayout));
}