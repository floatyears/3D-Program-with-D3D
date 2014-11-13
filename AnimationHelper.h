#ifndef ANIMATION_HELPER_H
#define ANIMATION_HELPER_H

#include "D3DUtils.h"

/// <summary>
/// 一个关键帧定义了一个固定时间内的骨骼移动。
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
/// BoneAnimation类定义了一个关键帧的列表。对于在两个关键帧之间的时间，我们计算在两个绑定到时间的最近的两个关键帧，我们假定一个移动总有两个关键帧
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