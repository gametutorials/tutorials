#ifndef __PCDX9RENDERTARGET_H__
#define __PCDX9RENDERTARGET_H__

#include "d3d_obj.h"
#include "d3d_texture.h"
		
class CD3DRenderTarget	
{
public:

	CD3DRenderTarget() { zeroVars(); }
	~CD3DRenderTarget() { deinit(); }

	bool init(int wid, int hgt, D3DFORMAT format, D3DFORMAT zStencilFormat);
	bool deinit();
	
	void begin(CD3DTexture *tex);
	void end();
	
	// Getters ***
	
	// *** End Getters
	
	void clear(int color);
	
private:

	LPD3DXRENDERTOSURFACE mRenderSurface;
	DWORD mClearFlags;
	
	void zeroVars()
	{
		mRenderSurface = NULL;
		mClearFlags = 0;
	}

};

#endif