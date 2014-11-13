#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LigthHelper.h"
#include "Effect.h"
#include "Vertexs.h"
#include "Camera.h"
#include "Sky.h"
#include "RenderStates.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "TextureMgr.h"
#include "BasicModel.h"

struct BoudingSphere
{
	BoudingSphere() : Center(0.0f,0.0f,0.0f),Radius(0.0f){}
	XMFLOAT3 Center;
	float Radius;
};

class MeshViewApp : public D3DApp 
{
public:
	MeshViewApp(HINSTANCE hInstance);
	~MeshViewApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void DrawSceneToSsaoNormalDepthMap();
	void DrawSceneToShadowMap();
	void DrawSceneQuad(ID3D11ShaderResourceView* srv);
	void BuildShadowTransform();
	void BuildScreenQuadGeometryBuffers();

private:

	TextureMgr mTexMgr;

	Sky* mSky;
	BasicModel* mTreeModel;
	BasicModel* mBaseModel;
	BasicModel* mStairsModel;
	BasicModel* mPillar1Model;
	BasicModel* mPillar2Model;
	BasicModel* mPillar3Model;
	BasicModel* mPillar4Model;
	BasicModel* mRockModel;

	std::vector<BasicMdoelInstance> mModelInstance;
	std::vector<BasicMdoelInstance> mAlphaClippedModelInstance;

	ID3D11Buffer* mSkySphereVB;
	ID3D11Buffer* mSkySphereIB;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	BoudingSphere mSceneBouds;

	static const int SMapSize = 2048;

	ShadowMap* mSmap;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	Ssao* mSsao;

	float mLightRotationAngle;
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];

	Camera mCam;

	POINT mLastMousePos;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	MeshViewApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

MeshViewApp::MeshViewApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), mTreeModel(0), mBaseModel(0), mStairsModel(0), 
  mPillar1Model(0), mPillar2Model(0), mPillar3Model(0), mPillar4Model(0), mRockModel(0),
  mScreenQuadVB(0), mScreenQuadIB(0),
  mSmap(0), mSsao(0),
  mLightRotationAngle(0.0f)
{
	mMainWndCaption = L"MeshView Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, -15.0f);
 
	mDirLights[0].Ambient  = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;
}


MeshViewApp::~MeshViewApp()
{
	SafeDelete(mTreeModel);
	SafeDelete(mBaseModel);
	SafeDelete(mStairsModel);
	SafeDelete(mPillar1Model);
	SafeDelete(mPillar2Model);
	SafeDelete(mPillar3Model);
	SafeDelete(mPillar4Model);
	SafeDelete(mRockModel);

	SafeDelete(mSky);
	SafeDelete(mSmap);
	SafeDelete(mSsao);

	ReleaseCOM(mScreenQuadVB);
	ReleaseCOM(mScreenQuadIB);

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
	RenderStates::DestroyAll();
}

bool MeshViewApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mTexMgr.Init(md3dDevice);

	mSky = new Sky(md3dDevice,L"Textures/desertcube1024.dds",5000.0f);
	mSmap = new ShadowMap(md3dDevice,SMapSize, SMapSize);

	mCam.SetLens(0.25f*MathHelper::Pi,AspectRatio(),1.0f,1000.0f);
	mSsao = new Ssao(md3dDevice,md3dImmediateContext,mClientWidth,mClientHeight,mCam.GetFovY(),mCam.GetFarZ());

	BuildScreenQuadGeometryBuffers();

	mTreeModel = new BasicModel(md3dDevice, mTexMgr, "Models\\tree.m3d", L"Textures\\");
	mBaseModel = new BasicModel(md3dDevice, mTexMgr, "Models\\base.m3d", L"Textures\\");
	mStairsModel = new BasicModel(md3dDevice, mTexMgr, "Models\\stairs.m3d", L"Textures\\");
	mPillar1Model = new BasicModel(md3dDevice, mTexMgr, "Models\\pillar1.m3d", L"Textures\\");
	mPillar2Model = new BasicModel(md3dDevice, mTexMgr, "Models\\pillar2.m3d", L"Textures\\");
	mPillar3Model = new BasicModel(md3dDevice, mTexMgr, "Models\\pillar5.m3d", L"Textures\\");
	mPillar4Model = new BasicModel(md3dDevice, mTexMgr, "Models\\pillar6.m3d", L"Textures\\");
	mRockModel = new BasicModel(md3dDevice, mTexMgr, "Models\\rock.m3d", L"Textures\\");

	BasicMdoelInstance treeInstance;
	BasicMdoelInstance baseInstance;
	BasicMdoelInstance stairsInstance;
	BasicMdoelInstance pillar1Instance;
	BasicMdoelInstance pillar2Instance;
	BasicMdoelInstance pillar3Instance;
	BasicMdoelInstance pillar4Instance;
	BasicMdoelInstance rockInstance1;
	BasicMdoelInstance rockInstance2;
	BasicMdoelInstance rockInstance3;

	treeInstance.Model = mTreeModel;
	baseInstance.Model = mBaseModel;
	stairsInstance.Model = mStairsModel;
	pillar1Instance.Model = mPillar1Model;
	pillar2Instance.Model = mPillar2Model;
	pillar3Instance.Model = mPillar3Model;
	pillar4Instance.Model = mPillar4Model;
	rockInstance1.Model = mRockModel;
	rockInstance2.Model = mRockModel;
	rockInstance3.Model = mRockModel;

	XMMATRIX modelScale = XMMatrixScaling(1.0f,1.0f,1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(0.0f,0.0f,0.0f);
	XMStoreFloat4x4(&treeInstance.World,modelScale*modelRot*modelOffset);
	XMStoreFloat4x4(&baseInstance.World,modelScale*modelRot*modelOffset);

	modelRot = XMMatrixRotationY(0.5f*XM_PI);
	modelOffset = XMMatrixTranslation(-0.5f,1.5f,5.0f);
	XMStoreFloat4x4(&stairsInstance.World,modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(-5.0f, 1.5f, 5.0f);
	XMStoreFloat4x4(&pillar1Instance.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(5.0f, 1.5f, 5.0f);
	XMStoreFloat4x4(&pillar2Instance.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(5.0f, 1.5f, -5.0f);
	XMStoreFloat4x4(&pillar3Instance.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelOffset = XMMatrixTranslation(-5.0f, 1.0f, -5.0f);
	XMStoreFloat4x4(&pillar4Instance.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(-1.0f, 1.4f, -7.0f);
	XMStoreFloat4x4(&rockInstance1.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(5.0f, 1.2f, -2.0f);
	XMStoreFloat4x4(&rockInstance2.World, modelScale*modelRot*modelOffset);

	modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = XMMatrixTranslation(-4.0f, 1.3f, 3.0f);
	XMStoreFloat4x4(&rockInstance3.World, modelScale*modelRot*modelOffset);

	mAlphaClippedModelInstance.push_back(treeInstance);

	mModelInstance.push_back(baseInstance);
	mModelInstance.push_back(stairsInstance);
	mModelInstance.push_back(pillar1Instance);
	mModelInstance.push_back(pillar2Instance);
	mModelInstance.push_back(pillar3Instance);
	mModelInstance.push_back(pillar4Instance);
	mModelInstance.push_back(rockInstance1);
	mModelInstance.push_back(rockInstance2);
	mModelInstance.push_back(rockInstance3);

	//计算场景边界盒子
	XMFLOAT3 minPt(MathHelper::Infinity,MathHelper::Infinity,MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity,-MathHelper::Infinity,-MathHelper::Infinity);
	for (UINT i = 0; i < mModelInstance.size(); i++)
	{
		for (int j = 0; j < mModelInstance[i].Model->Vertices.size(); j++)
		{
			XMFLOAT3 P = mModelInstance[i].Model->Vertices[j].Pos;

			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.x, P.x);
			minPt.z = MathHelper::Min(minPt.x, P.x);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.x, P.x);
			maxPt.z = MathHelper::Max(maxPt.x, P.x);
		}
	}

	mSceneBouds.Center = XMFLOAT3(
		0.5f*(minPt.x + maxPt.x),
		0.5f*(minPt.y + maxPt.y),
		0.5f*(minPt.z + maxPt.z));

	XMFLOAT3 extent(
		0.5f*(maxPt.x - minPt.x),
		0.5f*(maxPt.y - minPt.y),
		0.5f*(maxPt.z - minPt.z));

	mSceneBouds.Radius = sqrtf(extent.x*extent.x + extent.y * extent.y + extent.z* extent.z);

	return true;
}

void MeshViewApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	if( mSsao )
	{
		mSsao->OnSize(mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());
	}
}

void MeshViewApp::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(10.0f*dt);

	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-10.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-10.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(10.0f*dt);

	//
	// Animate the lights (and hence shadows).
	//

	BuildShadowTransform();

	mCam.UpdateViewMatrix();
}

void MeshViewApp::DrawScene()
{
	mSmap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);

	DrawSceneToShadowMap();

	md3dImmediateContext->RSSetState(0);

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
	md3dImmediateContext->RSSetViewports(1,&mScreenViewPort);
	mSsao->SetNormalDepthRenderTarget(mDepthStencilView);

	DrawSceneToSsaoNormalDepthMap();

	//计算环境光遮蔽
	mSsao->ComputeSsao(mCam);
	mSsao->BlurAmientMap(2);

	ID3D11RenderTargetView* renderTargets[1] = {mRenderTargetView};
	md3dImmediateContext->OMSetRenderTargets(1,renderTargets,mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1,&mScreenViewPort);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::Silver));

	md3dImmediateContext->OMSetDepthStencilState(RenderStates::EqualsDSS,0);

	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	float blendFactor[] = {0.0f,0.0f,0.0f,0.0f};

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mCam.GetPosition());
	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::NormalMapFX->SetShadowMap(mSmap->DeptchMapSRV());
	Effects::NormalMapFX->SetSsaoMap(mSsao->AmbientSRV());

	ID3DX11EffectTechnique* tech = Effects::NormalMapFX->Light3TexTech;
	ID3DX11EffectTechnique* alphaClippedTech = Effects::NormalMapFX->Light3TexAlphaClipTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireFrameRS);

	//绘制不透明的物体
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		for (UINT modelIndex = 0; modelIndex < mModelInstance.size(); modelIndex++)
		{
			world = XMLoadFloat4x4(&mModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f,1.0f,1.0f));

			for (UINT subset = 0; subset < mModelInstance[modelIndex].Model->SubsetCount; subset++)
			{
				Effects::NormalMapFX->SetMaterial(mModelInstance[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(mModelInstance[modelIndex].Model->DiffuseMapSRV[subset]);
				Effects::NormalMapFX->SetNormalMap(mModelInstance[modelIndex].Model->NormalMapSRV[subset]);

				tech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);
				mModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext,subset);
			}
		}
	}

	//经过alpha测试的三角形是叶子，两面地渲染它。
	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireFrameNoCullRS);
	else
	{
		md3dImmediateContext->RSSetState(RenderStates::NoCullRS);
	}
		
	alphaClippedTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		for (UINT modelIndex = 0; modelIndex < mAlphaClippedModelInstance.size(); modelIndex++)
		{
			world = XMLoadFloat4x4(&mAlphaClippedModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f,1.0f,1.0f));

			for (UINT subset = 0; subset < mAlphaClippedModelInstance[modelIndex].Model->SubsetCount; subset++)
			{
				Effects::NormalMapFX->SetMaterial(mAlphaClippedModelInstance[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(mAlphaClippedModelInstance[modelIndex].Model->DiffuseMapSRV[subset]);
				Effects::NormalMapFX->SetNormalMap(mAlphaClippedModelInstance[modelIndex].Model->NormalMapSRV[subset]);

				alphaClippedTech->GetPassByIndex(i)->Apply(0,md3dImmediateContext);
				mAlphaClippedModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext,subset);
			}
		}
	}

	md3dImmediateContext->RSSetState(0);

	md3dImmediateContext->OMSetDepthStencilState(0,0);

	// Debug view SSAO map.
	//DrawSceneQuad(mSsao->AmbientSRV());

	mSky->Draw(md3dImmediateContext,mCam);

	md3dImmediateContext->RSSetState(0);

	md3dImmediateContext->OMSetDepthStencilState(0,0);

	ID3D11ShaderResourceView* nullSRV[16] = {0};
	md3dImmediateContext->PSSetShaderResources(0,16,nullSRV);

	HR(mSwapChain->Present(0,0));
}

void MeshViewApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MeshViewApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MeshViewApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MeshViewApp::DrawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormaldDepthFX->NormalDepthTech;
	ID3DX11EffectTechnique* alphaClippedTech = Effects::SsaoNormaldDepthFX->NormalDepthAlphaClipTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;


	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireFrameRS);
     
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mModelInstance.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormaldDepthFX->SetWorldView(worldView);
			Effects::SsaoNormaldDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormaldDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormaldDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			for(UINT subset = 0; subset < mModelInstance[modelIndex].Model->SubsetCount; ++subset)
			{
				mModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }

	// The alpha tested triangles are leaves, so render them double sided.
	md3dImmediateContext->RSSetState(RenderStates::NoCullRS);
	alphaClippedTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mAlphaClippedModelInstance.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mAlphaClippedModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormaldDepthFX->SetWorldView(worldView);
			Effects::SsaoNormaldDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormaldDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormaldDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			
			for(UINT subset = 0; subset < mAlphaClippedModelInstance[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::SsaoNormaldDepthFX->SetDiffuseMap(mAlphaClippedModelInstance[modelIndex].Model->DiffuseMapSRV[subset]);
				alphaClippedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				mAlphaClippedModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }
 
	md3dImmediateContext->RSSetState(0);
}

void MeshViewApp::DrawSceneToShadowMap()
{
	XMMATRIX view     = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj     = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mCam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	ID3DX11EffectTechnique* tech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
     
	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireFrameRS);
	
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mModelInstance.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			for(UINT subset = 0; subset < mModelInstance[modelIndex].Model->SubsetCount; ++subset)
			{
				mModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }

	alphaClippedTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mAlphaClippedModelInstance.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mAlphaClippedModelInstance[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for(UINT subset = 0; subset < mAlphaClippedModelInstance[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::BuildShadowMapFX->SetDiffuseMap(mAlphaClippedModelInstance[modelIndex].Model->DiffuseMapSRV[subset]);
				alphaClippedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				mAlphaClippedModelInstance[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }

	md3dImmediateContext->RSSetState(0);
}

void MeshViewApp::DrawSceneQuad(ID3D11ShaderResourceView* srv)
{
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
 
	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ID3DX11EffectTechnique* tech = Effects::DebugTexFX->ViewRedTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::DebugTexFX->SetWorldViewProj(world);
		Effects::DebugTexFX->SetTexture(srv);

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
    }
}

void MeshViewApp::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBouds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBouds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBouds.Radius;
	float b = sphereCenterLS.y - mSceneBouds.Radius;
	float n = sphereCenterLS.z - mSceneBouds.Radius;
	float r = sphereCenterLS.x + mSceneBouds.Radius;
	float t = sphereCenterLS.y + mSceneBouds.Radius;
	float f = sphereCenterLS.z + mSceneBouds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void MeshViewApp::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullScreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}
