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

	//�����Ұ׶������ֵ
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	//��ý���Զ��Ұ�ĳߴ�
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	//������Ұ׶
	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
	
	//�����ͼ/ͶӰ����
	XMMATRIX View()const;
	XMMATRIX Proj()const;
	XMMATRIX ViewProj()const;

	//
	void Strafe(float d);
	void Walk(float d);

	//��ת��ͷ
	void Pitch(float angle);
	void RotateY(float angel);

	//���޸ľ�ͷ��λ�úͷ���֮�󣬵��ô˷����ؽ���ͼ����
	void UpdateViewMatrix();

private:

	//��ͷ����ϵͳ���������ռ������
	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;

	//������Ұ׶����ֵ
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	//������ͼ/ͶӰ����
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};

#endif