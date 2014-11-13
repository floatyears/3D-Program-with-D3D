#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

#include "D3DUtils.h"
#include <map>

/// <summary>
/// 简单的纹理管理器，避免加载重复的纹理。当多个网格引用同一个纹理文件时可能发生。
/// </summary>
/// <param name="Name of the parameter">Description</param>
/// <returns>Description of return value</returns>
class TextureMgr
{
public:
	TextureMgr();
	~TextureMgr();

	void Init(ID3D11Device* device);
	
	ID3D11ShaderResourceView* CreateTexture(std::wstring filename);

private:
	TextureMgr(const TextureMgr& rhs);
	TextureMgr& operator=(const TextureMgr& rhs);

private:
	ID3D11Device* md3dDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif