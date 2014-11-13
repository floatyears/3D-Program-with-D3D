#ifndef CAMERA_H
#define CAMERA_H

#include "D3DUtils.h"

class Camera
{
public:
	Camera();
	~Camera();

	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;

	void SetPosition(float x,float y, float z);
	void SetPosition(const XMFLOAT3& v);

	XMVECTOR GetRightXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetLook()const;

	//获得视野锥的属性值
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	//获得近和远视野的尺寸
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	//设置视野锥
	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
	
	//获得视图/投影矩阵
	XMMATRIX View()const;
	XMMATRIX Proj()const;
	XMMATRIX ViewProj()const;

	//
	void Strafe(float d);
	void Walk(float d);

	//旋转镜头
	void Pitch(float angle);
	void RotateY(float angel);

	//在修改镜头的位置和方向之后，调用此方法重建视图矩阵
	void UpdateViewMatrix();

private:

	//镜头坐标系统相对于世界空间的坐标
	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;

	//缓存视野锥属性值
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	//缓存视图/投影矩阵
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};

#endif