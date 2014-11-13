#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include <xnamath.h>

/// <summary>
/// 要保证struct的对齐和HLSL结构的偏移规则一致。元素被打包成4D的向量，限制条件是一个元素不能横跨4D向量的边界
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
	float Pad; //最后一个float作为占位用。
};

struct PointLight
{
	PointLight(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//打包成4D向量：(Position,Range)
	XMFLOAT3 Position;
	float Range;

	//打包成4D向量：(A0,A1,A2,Pad)
	XMFLOAT3 Att;
	float Pad;
};

struct SpotLight
{
	SpotLight(){ZeroMemory(this,sizeof(this));}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//打包成4D向量：(Position,Range)
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