#include "SkinnedData.h"
#include "MeshGeometry.h"


float AnimationClip::GetClipStartTime()const 
{
	//�ҵ����й�������С�Ŀ�ʼʱ��
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// �ҵ����й��������Ľ���ʱ��
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

	//����clip�и���ʱ������й�����transform��
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos,toParentTransforms);

	//���������㼶��ϵ�������еĹ����Ƶ�root�ռ�
	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	//����������Ϊ0��������û�и�������������toParentTransform����������ı任
	toRootTransforms[0] = toParentTransforms[0];

	//�ҵ��Ӽ���toRootTransform
	for (UINT i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		//����Ҫ��֤mBoneHierachy�е�����ֵ��С��������Ԫ�ر���Ԫ���ȼ���
		int parentIndex = mBoneHierachy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent,parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i],toRoot);
	}

	//���bone��offset�õ�finalTransform��
	for (UINT i = 0; i < numBones; i++)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransform[i],XMMatrixMultiply(offset,toRoot));
	}
}
