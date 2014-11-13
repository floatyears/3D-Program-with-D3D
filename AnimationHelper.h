#ifndef ANIMATION_HELPER_H
#define ANIMATION_HELPER_H

#include "D3DUtils.h"

/// <summary>
/// һ���ؼ�֡������һ���̶�ʱ���ڵĹ����ƶ���
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
struct Keyframe
{
	Keyframe();
	~Keyframe();

	float TimePos;
	XMFLOAT3 Translation;
	XMFLOAT3 Scale;
	XMFLOAT4 RotationQuat;
};

/// <summary>
/// BoneAnimation�ඨ����һ���ؼ�֡���б������������ؼ�֮֡���ʱ�䣬���Ǽ����������󶨵�ʱ�������������ؼ�֡�����Ǽٶ�һ���ƶ����������ؼ�֡
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, XMFLOAT4X4& M)const;

	std::vector<Keyframe> Keyframes;
};

#endif