/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#include <d3dx9.h>
#include "SQLiteLoggerCommon.h"

class DX9_BackbufferGrabber
{
public:
	// Width and height are size of the surface to copy to
	DX9_BackbufferGrabber();
	~DX9_BackbufferGrabber();

	// Call before using LockBackbufferCopy or ReleaseBackbufferCopy
	void InitBackbufferGrabber(LPDIRECT3DDEVICE9 pd3dDevice, int _width, int _height);

	// blob is an output parameter. Unchanged if the copy fails.
	// blob->data is only valid until you call ReleaseBackbufferCopy
	// If pd3dDevice is 0, it will use whatever was passed to InitBackbufferGrabber
	void LockBackbufferCopy(RGBImageBlob *blob);

	// Call ReleaseBackbufferCopy after calling LockBackbufferCopy, when you are done with blob.
	void ReleaseBackbufferCopy(void);
	
protected:
	IDirect3DSurface9 * pDestSurface, *pRenderTargetSurface;
	int width;
	int height;
	bool needsUnlock;
	LPDIRECT3DDEVICE9 deviceUsedToInit;
};
