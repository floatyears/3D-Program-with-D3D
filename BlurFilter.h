#ifndef BLURFILTER_H
#define BLURFILTER_H


#include "D3DUtils.h"

class BlurFilter
{
public:
	BlurFilter();
	~BlurFilter();
	
	ID3D11ShaderResourceView* GetBlurredOutput();

	void SetGaussianWeight(float sigma);

	void SetWeights(float weights[9]);

	void Init(ID3D11Device* device,UINT width, UINT height, DXGI_FORMAT format);

	void BlurInPlace(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV, int blurCount);

private:
	UINT mWidth;
	UINT mHeight;
	DXGI_FORMAT mFormat;

	ID3D11ShaderResourceView* mBlurredOutputTexSRV;
	ID3D11UnorderedAccessView* mBlurredOutputTexUAV;
};

#endif