#ifndef WAVES_H
#define WAVES_H

#include <Windows.h>
#include <xnamath.h>

class Waves
{
public:
	Waves();
	~Waves();

	UINT RowCount()const;
	UINT ColumCount()const;
	UINT VertexCount()const;
	UINT TriagnleCount()const;

	float Width()const;
	float Depth()const;

	//
	const XMFLOAT3& operator[](int i)const{return mCurrSolution[i]; }

	const XMFLOAT3& Normal(int i){return mNormals[i];}

	const XMFLOAT3& TangentX(int i){return mTangentX[i];}

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);

private:
	/// <summary>
	/// 行数
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	UINT mNumRows;
	/// <summary>
	/// 列数
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	UINT mNumCols;

	/// <summary>
	/// 顶点数
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	UINT mVertexCount;
	/// <summary>
	/// 三角形数量
	/// </summary>
	/// <param name="Name of the parameter">Description</param>
	/// <returns>Description of return value</returns>
	UINT mTriangleCount;

	//模拟能够预计算的常量
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	//前一个解
	XMFLOAT3* mPrevSolution;
	
	//当前的解，把它复制到顶点缓冲中用来渲染
	XMFLOAT3* mCurrSolution;

	XMFLOAT3* mNormals;
	XMFLOAT3* mTangentX;
};
#endif
