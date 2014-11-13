#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "D3DUtils.h"

class GeometryGenerator
{
public:
	/// <summary>
	/// 顶点数据，包含坐标、法线、切线、纹理坐标
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	struct Vertex
	{
		Vertex(){}
		Vertex(const XMFLOAT3& p,XMFLOAT3& n,const XMFLOAT3& t,const XMFLOAT2& uv)
			:Position(p),Normal(n),TangentU(t),TexC(uv){}
		Vertex(float px,float py,float pz,
			float nx,float ny,float nz,
			float tx, float ty, float tz,
			float u, float v):Position(px,py,pz),Normal(nx,ny,nz),TangentU(tx,ty,tz),TexC(u,v){}

		/// <summary>
		/// 坐标
		/// </summary>
		/// <param name="Name of the parameter">Description</param>
		/// <returns>Description of return value</returns>
		XMFLOAT3 Position;

		/// <summary>
		/// 法线
		/// </summary>
		/// <param name="Name of the parameter">Description</param>
		/// <returns>Description of return value</returns>
		XMFLOAT3 Normal;

		/// <summary>
		/// 切线
		/// </summary>
		/// <param name="Name of the parameter">Description</param>
		/// <returns>Description of return value</returns>
		XMFLOAT3 TangentU;

		/// <summary>
		/// 纹理坐标
		/// </summary>
		/// <param name="Name of the parameter">Description</param>
		/// <returns>Description of return value</returns>
		XMFLOAT2 TexC;
	};

	/// <summary>
	/// 网格数据，包含顶点和索引
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	void CreateBox(float width, float height, float depth, MeshData& meshData);

	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	void CreateCylinder(float bottomRaduis,float topRadius,float height, UINT sliceCount, UINT stackCount, MeshData& meshData);

	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);

	void CreateFullScreenQuad(MeshData& meshData);

private:
	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,UINT sliceCount,UINT stackCout,MeshData& MeshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius,float height,UINT sliceCount,UINT stackCount,MeshData& meshData);
};
#endif

