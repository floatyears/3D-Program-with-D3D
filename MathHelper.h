#ifndef MATHHELPER_H
#define MATHHELPER_H

#include<Windows.h>
#include<xnamath.h>

class MathHelper
{
public:
	static float RandF()
	{
		return (float)(rand())/(float)RAND_MAX;
	}

	static float RandF(float a,float b)
	{
		return a+RandF()*(b-a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a<b? a:b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a>b? a:b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b,float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	/// <summary>
	/// 如果x处于low和high之间，那么返回x，否则返回low或者high
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x<low?low:(x>high ? high :x);
	}

	static float AngleFromXY(float x, float y);

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f,0.0f,0.0f,1.0f);

		//获得矩阵的行列式，如果行列式不等于0，那么表明矩阵A有反矩阵。
		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det,A));
	}

	static XMVECTOR RandUnitVec3();
	static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	static const float Infinity;
	static const float Pi;
};
#endif

