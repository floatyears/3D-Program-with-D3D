#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "d3dApp.h"
#include <string>
#include <vector>
#include "Camera.h"
#include "Effect.h"

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	float GetAge()const;
	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Init(ID3D11Device* device, ParticleEffect* fx,ID3D11ShaderResourceView* texArraySRV, ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam);

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

private:

	UINT mMaxParticle;
	bool mFirstRun;
	
	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	ParticleEffect* mFX;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};

#endif