#include "Sky.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Vertexs.h"
#include "Effect.h"


Sky::Sky(ID3D11Device* device,const std::wstring& cubemapFilename, float skysphereRadius)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device,cubemapFilename.c_str(),0,0,&mCubeMapSRV,0));

	GeometryGenerator::MeshData sphere;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(skysphereRadius,30,30,sphere);

	std::vector<XMFLOAT3> vertices(sphere.Vertices.size());

	for (size_t i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i] = sphere.Vertices[i].Position;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3)*vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags= 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&vbd,&vinitData,&mVB));

	mIndexCount = sphere.Indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT)*mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags= 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(),sphere.Indices.end());

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices16[0];

	HR(device->CreateBuffer(&ibd,&iinitData,&mIB));
}


Sky::~Sky(void)
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mCubeMapSRV);
}

ID3D11ShaderResourceView* Sky::CubeMapSRV()
{
	return mCubeMapSRV;
}

void Sky::Draw(ID3D11DeviceContext* dc, const Camera& camera)
{
	//����������н���յ����ķ������ԭ��
	XMFLOAT3 eyePos = camera.GetPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x,eyePos.y,eyePos.z);


	XMMATRIX WVP = XMMatrixMultiply(T,camera.ViewProj());
	Effects::SkyFX->SetWorldViewProj(WVP);
	Effects::SkyFX->SetCubeMap(mCubeMapSRV);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0,1,&mVB,&stride,&offset);
	dc->IASetIndexBuffer(mIB,DXGI_FORMAT_R16_UINT,0);
	dc->IASetInputLayout(InputLayouts::Pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::SkyFX->SkyTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		ID3DX11EffectPass* pass = Effects::SkyFX->SkyTech->GetPassByIndex(i);
		pass->Apply(0,dc);
		dc->DrawIndexed(mIndexCount,0,0);
	}
}