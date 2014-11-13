#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include <xnamath.h>

/// <summary>
/// Ҫ��֤struct�Ķ����HLSL�ṹ��ƫ�ƹ���һ�¡�Ԫ�ر������4D������������������һ��Ԫ�ز��ܺ��4D�����ı߽�
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
struct DirectionalLight
{
	DirectionalLight(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; //���һ��float��Ϊռλ�á�
};

struct PointLight
{
	PointLight(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//�����4D������(Position,Range)
	XMFLOAT3 Position;
	float Range;

	//�����4D������(A0,A1,A2,Pad)
	XMFLOAT3 Att;
	float Pad;
};

struct SpotLight
{
	SpotLight(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//�����4D������(Position,Range)
	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	//
	XMFLOAT3 Att;
	float Pad;
};

struct Material
{
	Material(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};
#endif