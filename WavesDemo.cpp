#include "d3dApp.h"
#include "d3dx11effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "Waves.h"


struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class WavesDemo: public D3DApp
{
public :
	WavesDemo(HINSTANCE hInstance);
	~WavesDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private :
	float GetHeight(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildWavesGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	//地面
	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mLandIB;
	//波浪
	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;
	
	ID3D11RasterizerState* mWireFrameRS;

	//定义本地到世界的变换
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mWavesWorld;

	UINT mGridIndexCount;

	Waves mWaves;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	WavesDemo theApp(hInstance);
	if(!theApp.Init())
		return 0;
	return theApp.Run();
}

WavesDemo::WavesDemo(HINSTANCE hInstance):D3DApp(hInstance),mLandVB(0),mLandIB(0),mWavesIB(0),mFX(0),mTech(0),mfxWorldViewProj(0),mInputLayout(0),mWireFrameRS(0),mGridIndexCount(0),mTheta(1.5f*MathHelper::Pi),mPhi(1.5f*MathHelper::Pi),mRadius(200.0f)
{
	mMainWndCaption = L"Waves Demo";
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld,I);
	XMStoreFloat4x4(&mWavesWorld,I);
	XMStoreFloat4x4(&mView,I);
	XMStoreFloat4x4(&mProj,I);
}

WavesDemo::~WavesDemo()
{
	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireFrameRS);
}

bool WavesDemo::Init()
{
	if(!D3DApp::Init())
		return false;
	mWaves.Init(200,200,0.8f,0.03f,3.25f,0.4f);

	BuildLandGeometryBuffers();
	BuildWavesGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc,sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc,&mWireFrameRS));

	return true;
}

void WavesDemo::OnResize()
{
	D3DApp::OnResize();
	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi,AspectRatio(),1.0f,1000.0f);
	XMStoreFloat4x4(&mProj,p);
}

void WavesDemo::UpdateScene(float dt)
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x,y,z,1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos , target, up);
	XMStoreFloat4x4(&mView,V);

	static float t_base = 0.0f;
	if((mTimer.TotalTime()-t_base) >= 0.25f)
	{
		t_base += 0.25f;
		DWORD i = 5 + rand()%190;
		DWORD j = 5 + rand()%190;

		float r = MathHelper::RandF(1.0f,2.0f);
		mWaves.Disturb(i,j,r);
	}

	mWaves.Update(dt);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mWavesVB,0,D3D11_MAP_WRITE_DISCARD,0,&mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); i++)
	{
		v[i].Pos = mWaves[i];
		v[i].Color = XMFLOAT4(0.4f,0.8f,1.0f,1.0f);
	}

	md3dImmediateContext->Unmap(mWavesVB,0);
}

void WavesDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		
		XMMATRIX world;
		XMMATRIX worldViewProj;

		//画出地表
		md3dImmediateContext->IASetVertexBuffers(0,1,&mLandVB,&stride,&offset);
		md3dImmediateContext->IASetIndexBuffer(mLandIB,DXGI_FORMAT_R32_UINT,0);

	    world = XMLoadFloat4x4(&mGridWorld);
		worldViewProj = world*view*proj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount,0,0);

		//画出波浪
		md3dImmediateContext->RSSetState(mWireFrameRS);

		md3dImmediateContext->IASetVertexBuffers(0,1,&mWavesVB,&stride,&offset);
		md3dImmediateContext->IASetIndexBuffer(mWavesIB,DXGI_FORMAT_R32_UINT,0);

		world = XMLoadFloat4x4(&mWavesWorld);
		worldViewProj = world*view*proj;
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(3*mWaves.TriagnleCount(),0,0);
		
		

		md3dImmediateContext->RSSetState(0);
	}
	HR(mSwapChain->Present(0,0));
}

void WavesDemo::OnMouseDown(WPARAM btnState,int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}
void WavesDemo::OnMouseUp(WPARAM btnState, int x, int y)
{

	ReleaseCapture();
}

void WavesDemo::OnMouseMove(WPARAM btnState, int x, int y )
{
	if((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mTheta += dx;
		mPhi += dy;

		mPhi = MathHelper::Clamp(mPhi,0.1f,MathHelper::Pi-0.1f);
	}else if((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.2f*static_cast<float>(x-mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y-mLastMousePos.y);

		mRadius += dx - dy;
		mRadius = MathHelper::Clamp(mRadius,50.0f,500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float WavesDemo::GetHeight(float x, float z)const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

void WavesDemo::BuildLandGeometryBuffers()
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
		//vertices[i].Color = XMFLOAT4(0.0f,0.0f,0.0f,1.0f);
		if(p.y < -10.0f)
		{
			vertices[i].Color = XMFLOAT4(1.0f,0.96f,0.62f,1.0f);
		}else if(p.y < 5.0f)
		{
			vertices[i].Color = XMFLOAT4(0.48f,0.77f,0.46f,1.0f);
		}else if(p.y < 12.0f)
		{
			vertices[i].Color = XMFLOAT4(0.1f,0.48f,0.19f,1.0f);
		}else if(p.y < 20.0f)
		{
			vertices[i].Color = XMFLOAT4(0.45f,0.39f,0.34f,1.0f);
		}else
		{
			vertices[i].Color = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
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
	HR(md3dDevice->CreateBuffer(&vbd,&vinitData,&mLandVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd,&iinitData,&mLandIB));
}

void WavesDemo::BuildWavesGeometryBuffers()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd,0,&mWavesVB));

	std::vector<UINT> indices(3*mWaves.TriagnleCount());

	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumCount();
	int k = 0;
	for (UINT i = 0; i < m-1; i++)
	{
		for (UINT j = 0; j  < n-1; j ++)
		{
			indices[k] = i*n +j;
			indices[k+1] = i*n + j+1;
			indices[k+2] = (i+1)*n + j;

			indices[k+3] = (i+1)*n +j;
			indices[k+4] = i*n + j + 1;
			indices[k+5] = (i+1)*n + j+1;

			k+=6;
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd,&iinitData,&mWavesIB));
}

void WavesDemo::BuildFX()
{	DWORD shaderFlags = 0;
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

void WavesDemo::BuildVertexLayout()
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