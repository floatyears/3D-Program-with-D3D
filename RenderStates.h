#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "D3DUtils.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	
	static ID3D11DepthStencilState* MarkMirrorDSS;
	static ID3D11DepthStencilState* DrawReflectionDSS;
	static ID3D11DepthStencilState* NoDoubleBlendDSS;

	static ID3D11RasterizerState* WireFrameNoCullRS;
	static ID3D11RasterizerState* WireFrameRS;
	static ID3D11RasterizerState* NoCullRS;
	static ID3D11RasterizerState* CullClockWiseRS;


	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
	static ID3D11BlendState* NoRenderTargetWritesBS;

	static ID3D11DepthStencilState* LessEqualDSS;
	static ID3D11DepthStencilState* EqualsDSS;
};

#endif