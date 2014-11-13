#ifndef SKINNEDDATA_H
#define SKINNEDDATA_H

#include "D3DUtils.h"
#include <map>
#include "AnimationHelper.h"

/// <summary>
/// AnimationClips�����У�"Walk","Run","Attack","Defend"
//  һ��AnimationClip��ÿ��bone��Ҫһ��BoneAnimation
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

	void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const;

	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:
	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	void Set(std::vector<int> & boneHierachy,
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<std::string, AnimationClip>& animation);

	//��ʵ�ʵ���Ŀ�У��������Ҫ�����������������л����������ͬ��clipName��timePos���ö�Ρ�
	void GetFinalTransform(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)const;

private:

	std::vector<int> mBoneHierachy;
	std::vector<XMFLOAT4X4> mBoneOffsets;
	std::map<std::string,AnimationClip> mAnimations;
};

#endif