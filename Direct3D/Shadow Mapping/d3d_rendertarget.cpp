#include "d3d_rendertarget.h"


bool CD3DRenderTarget::init(int wid, int hgt, D3DFORMAT format, D3DFORMAT zStencil)
{
	deinit(); // First clear out anything already there

	assert(CD3DObj::mDevice);
	
	bool hasZStencil = (zStencil != 0); // If the format is anything but zero, we are 
										// requesting a Z-Stencil buffer also
										
	HRESULT r = D3DXCreateRenderToSurface(CD3DObj::mDevice, wid, hgt, format, 
										  hasZStencil, zStencil, &rendersurface);
										  
	mClearFlags |= D3DCLEAR_TARGET;
	
	if(hasZStencil) 
	{
		mClearFlags |= D3DCLEAR_ZBUFFER;
		mClearFlags |= D3DCLEAR_STENCIL;
	}
	
	return (r == D3D_OK);
}

bool CD3DRenderTarget::deinit()
{
	SAFE_RELEASE(rendersurface);
	zeroVars();
		return true;
}

void CD3DRenderTarget::begin(CD3DTexture *tex)
{
	assert(tex != NULL);
	assert(tex->getSurface() != NULL);
	
	HRESULT r;
	D3DVIEWPORT9 viewPort = {0};
	
	viewPort.MaxZ = 1.0f;
	viewPort.Width = tex->getWidth();
	viewPort.Height = tex->getHeight();
	
	r = rendersurface->BeginScene(tex->getSurface(), &viewPort);
	assert(r == D3D_OK);	
}

void CD3DRenderTarget::end()
{
	HRESULT r = rendersurface->EndScene(D3DX_FILTER_LINEAR);
	assert(r == D3D_OK);
}

void CD3DRenderTarget::clear(int color)
{
	CD3DObj::mDevice->Clear(0,0,mClearFlags,color,1.0f,0);
}