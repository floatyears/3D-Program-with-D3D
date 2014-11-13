#include "SkinnedData.h"
#include "MeshGeometry.h"


float AnimationClip::GetClipStartTime()const 
{
	//找到所有骨骼中最小的开始时间
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// 找到所有骨骼中最大的结束时间
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		t = MathHelper::Max(t,BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const
{
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		BoneAnimations[i].Interpolate(t,boneTransforms[i]);
	}
}

float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount()const
{
	return mBoneHierachy.size();
}

void SkinnedData::Set(std::vector<int>& boneHierachy,
					  std::vector<XMFLOAT4X4>& boneOffsets,
					  std::map<std::string, AnimationClip>& animations)
{
	mBoneHierachy = boneHierachy;
	mBoneOffsets = boneOffsets;
	mAnimations = animations;
}

void SkinnedData::GetFinalTransform(const std::string& clipName,float timePos, std::vector<XMFLOAT4X4>& finalTransform)const
{
	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	//计算clip中给定时间的所有骨骼的transform。
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos,toParentTransforms);

	//遍历整个层级关系，将所有的骨骼移到root空间
	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	//根骨骼索引为0，根骨骼没有父级，所以它的toParentTransform就是它本身的变换
	toRootTransforms[0] = toParentTransforms[0];

	//找到子级的toRootTransform
	for (UINT i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		//这里要保证mBoneHierachy中的索引值从小到大，祖先元素比子元素先计算
		int parentIndex = mBoneHierachy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent,parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i],toRoot);
	}

	//左乘bone的offset得到finalTransform。
	for (UINT i = 0; i < numBones; i++)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransform[i],XMMatrixMultiply(offset,toRoot));
	}
}
