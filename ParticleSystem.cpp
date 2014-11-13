#include "ParticleSystem.h"
#include "Vertexs.h"
#include "RenderStates.h"


ParticleSystem::ParticleSystem(void): mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f,0.0f,0.0f);
	mEmitPosW = XMFLOAT3(0.0f,0.0f,0.0f);
	mEmitDirW = XMFLOAT3(0.0f,1.0f,0.0f);
}


ParticleSystem::~ParticleSystem(void)
{
	ReleaseCOM(mInitVB);
	ReleaseCOM(mDrawVB);
	ReleaseCOM(mStreamOutVB);
}

float ParticleSystem::GetAge()const
{
	return mAge;
}

void ParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void ParticleSystem::Init(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV,ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	mMaxParticle = maxParticles;

	mFX = fx;

	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;

	BuildVB(device);
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void ParticleSystem::Update(float dt,float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	mAge += dt;
}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera& cam)
{
	XMMATRIX VP = cam.ViewProj();
	
	//���ó���
	mFX->SetViewProj(VP);
	mFX->SetGameTime(mGameTime);
	mFX->SetTimeStep(mTimeStep);
	mFX->SetEmitPosW(mEmitPosW);
	mFX->SetEyePosW(mEyePosW);
	mFX->SetEmitDirW(mEmitDirW);
	mFX->SetTexArray(mTexArraySRV);
	mFX->SetRandomTex(mRandomTexSRV);

	//���õ�IA�׶�
	dc->IASetInputLayout(InputLayouts::Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex::Particle);
	UINT offset = 0;

	//��һ���ó�ʼ����VB��֮�󣬾��ð�����ǰ�����б��VB��
	if(mFirstRun)
		dc->IASetVertexBuffers(0,1,&mInitVB,&stride,&offset);
	else
		dc->IASetVertexBuffers(0,1,&mDrawVB,&stride,&offset);

	//���Ƶ�ǰ�������б�ʹ�ý���������������������ǡ�
	//���µĶ����������Ŀ��VB��
	dc->SOSetTargets(1,&mStreamOutVB,&offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	mFX->StreamOutTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		mFX->StreamOutTech->GetPassByIndex(i)->Apply(0,dc);

		if (mFirstRun)
		{
			dc->Draw(1,0);
			mFirstRun = false;
		}else
		{
			dc->DrawAuto();
		}
		
	}

	//�������ɣ���󶥵㻺��
	ID3D11Buffer* buffArray[1] = {0};
	dc->SOSetTargets(1,buffArray,&offset);

	//�������㻺��
	std::swap(mDrawVB,mStreamOutVB);

	dc->IASetVertexBuffers(0,1,&mDrawVB,&stride,&offset);
		
	mFX->DrawTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		//dc->RSSetState(RenderStates::WireFrameRS);
		mFX->DrawTech->GetPassByIndex(i)->Apply(0,dc);

		dc->DrawAuto();
	}
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{

	//�������壬�߿�����ϵͳ
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex::Particle)*1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//��ʼ�����ӷ�����������0������0��ʣ�µ��������Բ�Ӧ�õ��������ϡ�
	Vertex::Particle p;
	ZeroMemory(&p,sizeof(Vertex::Particle));
	p.Age = 0.0f;
	p.Type = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd,&vinitData,&mInitVB));

	//Ϊ������ͻ��ƴ���ƹ�һ���
	vbd.ByteWidth = sizeof(Vertex::Particle)*mMaxParticle;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(device->CreateBuffer(&vbd,0,&mDrawVB));
	HR(device->CreateBuffer(&vbd,0,&mStreamOutVB));
}

