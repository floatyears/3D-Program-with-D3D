#ifndef EFFECTS_H
#define EFFECTS_H

#include "D3DUtils.h"
#include "d3dx11effect.h"
#include "LigthHelper.h"
#include "Effect.h"

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* devices, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);


protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect:public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}

	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetFogColor(const FXMVECTOR v) 
	{ 
		FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); 
	}
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }

	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex);}
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;

	ID3DX11EffectMatrixVariable* WorldViewProjTex;

	ID3DX11EffectMatrixVariable* ShadowTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
};
#pragma endregion

class TreeSpriteEffect:public Effect
{
public:
	TreeSpriteEffect(ID3D11Device* device,const std::wstring& filename);
	~TreeSpriteEffect();

	void SetViewProj(CXMMATRIX M){ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));};
	void SetEyePosW(const XMFLOAT3& v){EyePosW->SetRawValue(&v,0,sizeof(XMFLOAT3));};
	void SetFogColor(const FXMVECTOR v){FogColor->SetFloatVector(reinterpret_cast<const float*>(&v));};
	void SetFogStart(float f){FogStart->SetFloat(f);};
	void SetFogRange(float f){FogRange->SetFloat(f);};
	void SetDirLights(const DirectionalLight* lights){DirLights->SetRawValue(lights,0,3*sizeof(DirectionalLight));};
	void SetMaterial(const Material& mat){Mat->SetRawValue(&mat,0,sizeof(Material));};
	void SetTreeTextureMapArray(ID3D11ShaderResourceView* tex){TreeTextureMapArray->SetResource(tex);}

	ID3DX11EffectTechnique* Light3TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* TreeTextureMapArray;
};

class VecAddEffect:public Effect
{
public:
	VecAddEffect(ID3D11Device* device,const std::wstring& filename);
	~VecAddEffect();

	void SetInputA(ID3D11ShaderResourceView* srv){InputA->SetResource(srv);};
	void SetInputB(ID3D11ShaderResourceView* srv){InputB->SetResource(srv);}
	void SetOutput(ID3D11UnorderedAccessView* uav){Output->SetUnorderedAccessView(uav);}


	ID3DX11EffectTechnique* VecAddTech;
	
	ID3DX11EffectShaderResourceVariable* InputA;
	ID3DX11EffectShaderResourceVariable* InputB;
	ID3DX11EffectUnorderedAccessViewVariable* Output;
};

class BlurEffect : public Effect
{
public:
	BlurEffect(ID3D11Device* device,const std::wstring& filename);
	~BlurEffect();

	void SetWeight(float weights[9]){Weights->GetFloatArray(weights,0,9);}
	void SetInputMap(ID3D11ShaderResourceView* tex){InputMap->SetResource(tex);}
	void SetOutputMap(ID3D11UnorderedAccessView* tex){OutputMap->SetUnorderedAccessView(tex);}

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* Weights;
	ID3DX11EffectShaderResourceVariable* InputMap;
	ID3DX11EffectUnorderedAccessViewVariable* OutputMap;
};

#pragma region Effects

class TessellationEffect:public Effect
{
public:
	TessellationEffect(ID3D11Device* device,const std::wstring& filename);
	~TessellationEffect();

	void SetWorldViewProj(CXMMATRIX M){WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));};
	void SetWorld(CXMMATRIX M){World->SetMatrix(reinterpret_cast<const float*>(&M));};
	void SetTexTransform(CXMMATRIX M){TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));};
	void SetWorldInvTranspose(CXMMATRIX M){WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));};
	void SetEyePosW(const XMFLOAT3& v){EyePosW->SetRawValue(&v,0,sizeof(XMFLOAT3));};
	void SetFogColor(const FXMVECTOR v){FogColor->SetFloatVector(reinterpret_cast<const float*>(&v));};
	void SetFogStart(float f){FogStart->SetFloat(f);};
	void SetFogRange(float f){FogRange->SetFloat(f);};
	void SetDirLights(const DirectionalLight* light){DirLight->SetRawValue(light,0,3*sizeof(DirectionalLight));};
	void SetMaterial(const Material& mat){Mat->SetRawValue(&mat,0,sizeof(Material));};
	void SetDiffuseMap(ID3D11ShaderResourceView* tex){DiffuseMap->SetResource(tex);};

	ID3DX11EffectTechnique* TessTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLight;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};

class InstancedBasicEffect:public Effect
{
public:
	InstancedBasicEffect(ID3D11Device* device,const std::wstring& filename);
	~InstancedBasicEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};

#pragma region SkyEffect
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M){ 
		WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); 
	}
	void SetCubeMap(ID3D11ShaderResourceView* cubemap)  { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region NormalMapEffect
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex);}
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }


	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }

	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt) {BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M),0,cnt);}

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectTechnique* Light1SkinnedTech;
	ID3DX11EffectTechnique* Light2SkinnedTech;
	ID3DX11EffectTechnique* Light3SkinnedTech;

	ID3DX11EffectTechnique* Light0TexSkinnedTech;
	ID3DX11EffectTechnique* Light1TexSkinnedTech;
	ID3DX11EffectTechnique* Light2TexSkinnedTech;
	ID3DX11EffectTechnique* Light3TexSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipSkinnedTech;

	ID3DX11EffectTechnique* Light1FogSkinnedTech;
	ID3DX11EffectTechnique* Light2FogSkinnedTech;
	ID3DX11EffectTechnique* Light3FogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogSkinnedTech;

	ID3DX11EffectTechnique* Light1ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3ReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectSkinnedTech;

	ID3DX11EffectTechnique* Light1FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3FogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectMatrixVariable* BoneTransforms;

	ID3DX11EffectMatrixVariable* WorldViewProjTex;

	ID3DX11EffectMatrixVariable* ShadowTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
};
#pragma endregion

class DisplacementMapEffect:public Effect 
{
public:
	DisplacementMapEffect(ID3D11Device* device, const std::wstring& filename);
	~DisplacementMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransfom->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransfom;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;

	ID3DX11EffectMatrixVariable* ShadowTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
};


#pragma region TerrianEffect
class TerrainEffect :public Effect
{
public:
	TerrainEffect(ID3D11Device* device,const std::wstring& filenmae);
	~TerrainEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinDist(float f)                            { MinDist->SetFloat(f); }
	void SetMaxDist(float f)                            { MaxDist->SetFloat(f); }
	void SetMinTess(float f)                            { MinTess->SetFloat(f); }
	void SetMaxTess(float f)                            { MaxTess->SetFloat(f); }
	void SetTexelCellSpaceU(float f)                    { TexelCellSpaceU->SetFloat(f); }
	void SetTexelCellSpaceV(float f)                    { TexelCellSpaceV->SetFloat(f); }
	void SetWorldCellSpace(float f)                     { WorldCellSpace->SetFloat(f); }
	void SetWorldFrustumPlanes(XMFLOAT4 planes[6])      { WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6); }

	void SetLayerMapArray(ID3D11ShaderResourceView* tex)   { LayerMapArray->SetResource(tex); }
	void SetBlendMap(ID3D11ShaderResourceView* tex)        { BlendMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex)       { HeightMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
	ID3DX11EffectScalarVariable* TexelCellSpaceU;
	ID3DX11EffectScalarVariable* TexelCellSpaceV;
	ID3DX11EffectScalarVariable* WorldCellSpace;
	ID3DX11EffectVectorVariable* WorldFrustumPlanes;

	ID3DX11EffectShaderResourceVariable* LayerMapArray;
	ID3DX11EffectShaderResourceVariable* BlendMap;
	ID3DX11EffectShaderResourceVariable* HeightMap;
};
#pragma endregion

class ParticleEffect:public Effect
{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	void SetViewProj(CXMMATRIX M){ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void SetGameTime(float f){GameTime->SetFloat(f);}
	void SetTimeStep(float f){TimeStep->SetFloat(f);}

	void SetEyePosW(const XMFLOAT3& v){EyePosW->SetRawValue(&v,0,sizeof(XMFLOAT3));}
	void SetEmitPosW(const XMFLOAT3& v){EmitPosW->SetRawValue(&v,0,sizeof(XMFLOAT3));}
	void SetEmitDirW(const XMFLOAT3& v){EmitDirW->SetRawValue(&v,0,sizeof(XMFLOAT3));}

	void SetTexArray(ID3D11ShaderResourceView* tex){TexArray->SetResource(tex);}
	void SetRandomTex(ID3D11ShaderResourceView* tex){RandomTex->SetResource(tex);}

	ID3DX11EffectTechnique* StreamOutTech;
	ID3DX11EffectTechnique* DrawTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectScalarVariable* GameTime;
	ID3DX11EffectScalarVariable* TimeStep;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* EmitPosW;
	ID3DX11EffectVectorVariable* EmitDirW;
	ID3DX11EffectShaderResourceVariable* TexArray;
	ID3DX11EffectShaderResourceVariable* RandomTex;
};

class BuildShadowMapEffect:public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectTechnique* BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipSkinnedTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;
 
	ID3DX11EffectMatrixVariable* BoneTransforms;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};

class DebugTexEffect:public Effect
{
public:
	DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
	~DebugTexEffect();

	void SetWorldViewProj(CXMMATRIX M){WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void SetTexture(ID3D11ShaderResourceView* tex){Texture->SetResource(tex);}

	ID3DX11EffectTechnique* ViewArgbTech;
	ID3DX11EffectTechnique* ViewRedTech;
	ID3DX11EffectTechnique* ViewGreenTech;
	ID3DX11EffectTechnique* ViewBlueTech;
	ID3DX11EffectTechnique* ViewAlphaTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* Texture;
};

class SsaoNormalDepthEffect:public Effect
{
public:
	SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoNormalDepthEffect();

	void SetWorldView(CXMMATRIX M)                      { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTransposeView(CXMMATRIX M)          { WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }

	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipTech;

	ID3DX11EffectTechnique* NormalDepthSkinnedTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldInvTransposeView;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectMatrixVariable* BoneTransforms;



	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};

class SsaoEffect : public Effect
{
public:
	SsaoEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoEffect();

	void SetViewToTexSpace(CXMMATRIX M)                    { ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetOffsetVectors(const XMFLOAT4 v[14])            { OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14); }
	void SetFrustumCorners(const XMFLOAT4 v[4])            { FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4); }
	void SetOcclusionRadius(float f)                       { OcclusionRadius->SetFloat(f); }
	void SetOcclusionFadeStart(float f)                    { OcclusionFadeStart->SetFloat(f); }
	void SetOcclusionFadeEnd(float f)                      { OcclusionFadeEnd->SetFloat(f); }
	void SetSurfaceEpsilon(float f)                        { SurfaceEpsilon->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetRandomVecMap(ID3D11ShaderResourceView* srv)    { RandomVecMap->SetResource(srv); }

	ID3DX11EffectTechnique* SsaoTech;

	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectScalarVariable* OcclusionRadius;
	ID3DX11EffectScalarVariable* OcclusionFadeStart;
	ID3DX11EffectScalarVariable* OcclusionFadeEnd;
	ID3DX11EffectScalarVariable* SurfaceEpsilon;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
};

class SsaoBlurEffect : public Effect
{
public:
	SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoBlurEffect();

	void SetTexelWidth(float f)                            { TexelWidth->SetFloat(f); }
	void SetTexelHeight(float f)                           { TexelHeight->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetInputImage(ID3D11ShaderResourceView* srv)      { InputImage->SetResource(srv); }

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
};

class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();
	
	static void initOther(ID3D11Device* device);

	static TreeSpriteEffect* TreeSpriteFX;
	static BasicEffect* BasicFX;
	static VecAddEffect* VecAddFX;
	static BlurEffect* BlurFX;
	static TessellationEffect* TessellationFX;
	static InstancedBasicEffect* InstancedBasicFX;
	static SkyEffect* SkyFX;
	static NormalMapEffect* NormalMapFX;
	static DisplacementMapEffect* DisplacementMapFX;
	static TerrainEffect* TerrainFX;

	static ParticleEffect* FireFX;
	static ParticleEffect* RainFX;

	static BuildShadowMapEffect* BuildShadowMapFX;
	static DebugTexEffect* DebugTexFX;

	static SsaoNormalDepthEffect* SsaoNormaldDepthFX;
	static SsaoEffect* SsaoFX;
	static SsaoBlurEffect* SsaoBlurFX;
};

#endif


