#ifndef SKINNEDDATA_H
#define SKINNEDDATA_H

#include "D3DUtils.h"
#include <map>
#include "AnimationHelper.h"

/// <summary>
/// AnimationClips例子有："Walk","Run","Attack","Defend"
//  一个AnimationClip中每个bone需要一个BoneAnimation
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

	//在实际的项目中，你可能需要缓存这个结果，可能有机会你会用相同的clipName和timePos调用多次。
	void GetFinalTransform(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)const;

private:

	std::vector<int> mBoneHierachy;
	std::vector<XMFLOAT4X4> mBoneOffsets;
	std::map<std::string,AnimationClip> mAnimations;
};

#endif