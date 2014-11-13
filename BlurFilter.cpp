#include "BlurFilter.h"
#include "Effect.h"

BlurFilter::BlurFilter():mBlurredOutputTexSRV(0),mBlurredOutputTexUAV(0)
{
	
}

BlurFilter::~BlurFilter()
{
	ReleaseCOM(mBlurredOutputTexSRV);
	ReleaseCOM(mBlurredOutputTexUAV);
}

ID3D11ShaderResourceView* BlurFilter::GetBlurredOutput()
{
	return mBlurredOutputTexSRV;
}

void BlurFilter::SetGaussianWeight(float sigma)
{
	float d = 2.0f* sigma*sigma;

	float weight[9];
	float sum = 0.0f;

	for (int i = 0; i < 8; i++)
	{
		float x =(float)i;

		weight[i] = expf(-x*x/d);

		sum += weight[i];
	}

	for (int i = 0; i < 8; i++)
	{
		weight[i] /= sum;
	}

	Effects::BlurFX->SetWeight(weight);
}

void BlurFilter::SetWeights(float weights[9])
{
	Effects::BlurFX->SetWeight(weights);
}

void BlurFilter::Init(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	ReleaseCOM(mBlurredOutputTexSRV);
	ReleaseCOM(mBlurredOutputTexUAV);

	mWidth = width;
	mHeight = height;
	mFormat = format;

	D3D11_TEXTURE2D_DESC blurredTexDesc;
	blurredTexDesc.Width= width;
	blurredTexDesc.Height = height;
	blurredTexDesc.MipLevels = 1;
	blurredTexDesc.ArraySize = 1;
	blurredTexDesc.Format = format;
	blurredTexDesc.SampleDesc.Count = 1;
	blurredTexDesc.SampleDesc.Quality = 0;
	blurredTexDesc.Usage = D3D11_USAGE_DEFAULT;
	blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	blurredTexDesc.CPUAccessFlags = 0;
	blurredTexDesc.MiscFlags = 0;

	ID3D11Texture2D* blurredTex = 0;
	HR(device->CreateTexture2D(&blurredTexDesc,0,&blurredTex));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	HR(device->CreateShaderResourceView(blurredTex, & srvDesc,&mBlurredOutputTexSRV));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	HR(device->CreateUnorderedAccessView(blurredTex,&uavDesc,&mBlurredOutputTexUAV));

	ReleaseCOM(blurredTex);
}

void BlurFilter::BlurInPlace(ID3D11DeviceContext* dc,ID3D11ShaderResourceView* inputSRV,ID3D11UnorderedAccessView* inputUAV,int blurCount)
{
	for (int i = 0; i < blurCount; i++)
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		Effects::BlurFX->HorzBlurTech->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; i++)
		{
			Effects::BlurFX->SetInputMap(inputSRV);
			Effects::BlurFX->SetOutputMap(mBlurredOutputTexUAV);
			Effects::BlurFX->HorzBlurTech->GetPassByIndex(i)->Apply(0,dc);


			// How many groups do we need to dispatch to cover a row of pixels, where each
			// group covers 256 pixels (the 256 is defined in the ComputeShader).
			UINT numGroupX = (UINT)ceilf(mWidth / 256.0f);
			dc->Dispatch(numGroupX,mHeight,1);
		}

		ID3D11ShaderResourceView* nullSRV[1] = {0};
		dc->CSSetShaderResources(0,1,nullSRV);

		ID3D11UnorderedAccessView* nullUAV[1] = {0};
		dc->CSSetUnorderedAccessViews(0,1,nullUAV,0);

		Effects::BlurFX->VertBlurTech->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; i++)
		{
			Effects::BlurFX->SetInputMap(mBlurredOutputTexSRV);
			Effects::BlurFX->SetOutputMap(inputUAV);
			Effects::BlurFX->VertBlurTech->GetPassByIndex(i)->Apply(0,dc);

			UINT numGroupY = (UINT)ceilf(mHeight/256.0f);
			dc->Dispatch(mWidth,numGroupY,1);
		}

		//清理输入的srv
		// Unbind the input texture from the CS for good housekeeping.
		dc->CSSetShaderResources(0,1,nullSRV);
		//清理输出的uav
		// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
		// and a resource cannot be both an output and input at the same time.
		dc->CSSetUnorderedAccessViews(0,1,nullUAV,0);
	}

	dc->CSSetShader(0,0,0);
}