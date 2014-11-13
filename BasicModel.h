#ifndef BASICMODEL_H
#define BASICMODEL_H

#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "LigthHelper.h"
#include "Vertexs.h"

class BasicModel
{
public:
	BasicModel(ID3D11Device* device, TextureMgr& texMgr, const std::string& modeFilename, const std::wstring& texturePath);
	~BasicModel();

	UINT SubsetCount;

	std::vector<Material> Mat;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	std::vector<Vertex::PosNormalTexTan> Vertices;
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
};

struct BasicMdoelInstance
{
	BasicModel* Model;
	XMFLOAT4X4 World;
};

#endif