#ifndef VERTEX_H
#define VERTEX_H

#include "D3DUtils.h"


class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC TreePointSprite[2];
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasic32[8];
	static const D3D11_INPUT_ELEMENT_DESC PosNomalTexTan[4];
	static const D3D11_INPUT_ELEMENT_DESC Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static void InitOther(ID3D11Device* device);

	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* TreePointSprite;
	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* InstancedBasic32;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* Terrain;
	static ID3D11InputLayout* Particle;
	static ID3D11InputLayout* PosNormalTexTanSkinned;
};

namespace Vertex
{
	struct Basic32
	{
		Basic32() : Pos(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 0.0f), Tex(0.0f, 0.0f) {}
		Basic32(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv)
			: Pos(p), Normal(n), Tex(uv) {}
		Basic32(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
			: Pos(px, py, pz), Normal(nx, ny, nz), Tex(u,v) {}
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct TreePointSprite
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};
	
	struct Pos
	{
		float x;
		float y;
		float z;
	};

	struct PosNormalTexTan
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
	//	XMFLOAT3 TangentU;
	};

	struct Terrain
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
		XMFLOAT2 BoundsY;
	};

	struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

	struct PosNormalTexTanSkinned
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
		XMFLOAT3 Weights;
		BYTE BoneIndices[4];
	};
};


#endif

