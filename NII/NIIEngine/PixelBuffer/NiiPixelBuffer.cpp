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

#include "NiiPreProcess.h"
#include "NiiPixelBuffer.h"
#include "NiiPixelManager.h"
#include "NiiGeometryPixel.h"
#include "NiiTextureManager.h"
#include "NiiVertex.h"
#include "NiiUIManager.h"
#include "NiiScriptProperty.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PixelBufferGroup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PixelBufferGroup::PixelBufferGroup(GroupID gid) :
        mGroupID(gid),
        mTexture(0),
        mMode(SM_Unknow),
        mOffset(0, 0),
        mPixelOffset(0.0f, 0.0f),
        mNativeRes(640.0f, 480.0f)
    {
    }
    //------------------------------------------------------------------------
    PixelBufferGroup::PixelBufferGroup(GroupID gid, Texture * obj, ScaleMode mode, const Vector2f & oft,
        const Vector2f & pixeloft, const PlaneSizef & srcRes) :
        mGroupID(gid),
        mTexture(obj),
        mMode(mode),
        mOffset(oft),
        mPixelOffset(pixeloft),
        mNativeRes(srcRes)
    {
    }
    //------------------------------------------------------------------------
    PixelBufferGroup::PixelBufferGroup() :
        mPixelOffset(0.0f, 0.0f),
        mOffset(0, 0),
        mMode(SM_Unknow),
        mNativeRes(640.0f, 480.0f),
        mTexture(0)
    {
    }
    //------------------------------------------------------------------------
    PixelBufferGroup::~PixelBufferGroup()
    {
        PBList::iterator i, iend = mPBList.end();
        for (i = mPBList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
    }
    //------------------------------------------------------------------------
    bool PixelBufferGroup::operator == (PixelBufferGroup & o)
    {
        if (mTexture == o.mTexture && mMode == o.mMode && mPixelOffset == o.mPixelOffset
            && mOffset == o.mOffset && mNativeRes == o.mNativeRes)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    PixelBuffer * PixelBufferGroup::remove(PixelBufferID pid) 
    { 
        PBList::const_iterator i = mPBList.find(pid); 
        if (i != mPBList.end())
        {
            PixelBuffer * re = i->second;
            mPBList.erase(i);
            return re;
        }
        return 0; 
    }
    //------------------------------------------------------------------------
    void PixelBufferGroup::notifyView(const PlaneSizei & size)
    {
        Vector2f out;
        PixelBuffer::calcAspectRatio(getMode(), size, getNativeRes(), out);
        //mSize.mWidth = mArea.getSize().mWidth * out.x;
        //mSize.mHeight = mArea.getSize().mHeight * out.y;
        mOffset.x = mPixelOffset.x * out.x;
        mOffset.y = mPixelOffset.y * out.y;
        PBList::iterator i, iend = mPBList.end();
        for (i = mPBList.begin(); i != iend; ++i)
        {
            i->second->notifyRatio(out);
        }
    }
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// PixelBuffer
	//------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PixelBuffer::PixelBuffer(PixelBufferID pid, PixelBufferGroup * pbg, const Rectf & area) :
        mID(pid),
        mGroup(pbg),
        mArea(area)
    {
        mSize = mArea.getSize();
    }
    //------------------------------------------------------------------------
    PixelBuffer::PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area, ScaleMode mode,
        const Vector2f & oft, const Vector2f & pixeloft, const PlaneSizef & srcRes) :
        mID(pxid),
        mArea(area)
    {
        mGroup = N_Only(Pixel).create(obj, mode, oft, pixeloft, srcRes);
        mSize = mArea.getSize();

        Vector2f out;
        calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
        mSize.mWidth = mArea.getSize().mWidth * out.x;
        mSize.mHeight = mArea.getSize().mHeight * out.y;
    }
    //------------------------------------------------------------------------
    PixelBuffer::~PixelBuffer()
    {
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setArea(const Rectf & area)
    {
        mArea = area;

        if(getMode() != SM_Unknow)
        {
            Vector2f out;
            calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
            mSize.mWidth = mArea.getSize().mWidth * out.x;
            mSize.mHeight = mArea.getSize().mHeight * out.y;
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setOffset(const Vector2f & oft)
    {
        if (getOffset() != oft)
        {
            mGroup = N_Only(Pixel).create(getTexture(), getMode(), oft, getPixelOffset(), getNativeRes());
            if (getMode() != SM_Unknow)
            {
                Vector2f out;
                calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
                mSize.mWidth = mArea.getSize().mWidth * out.x;
                mSize.mHeight = mArea.getSize().mHeight * out.y;
            }
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setPixelOffset(const Vector2f & oft)
    {
        if (getPixelOffset() != oft)
        {
            mGroup = N_Only(Pixel).create(getTexture(), getMode(), getOffset(), oft, getNativeRes());
            if (getMode() != SM_Unknow)
            {
                Vector2f out;
                calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
                mSize.mWidth = mArea.getSize().mWidth * out.x;
                mSize.mHeight = mArea.getSize().mHeight * out.y;
            }
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setMode(ScaleMode mode)
    {
        if (getMode() != mode)
        {
            mGroup = N_Only(Pixel).create(getTexture(), mode, getOffset(), getPixelOffset(), getNativeRes());
            if (getMode() != SM_Unknow)
            {
                Vector2f out;
                calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
                mSize.mWidth = mArea.getSize().mWidth * out.x;
                mSize.mHeight = mArea.getSize().mHeight * out.y;
            }
            else
            {
                mSize = mArea.getSize();
            }
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setNativeRes(const PlaneSizef & area)
    {
        if (getNativeRes() != area)
        {
            mGroup = N_Only(Pixel).create(getTexture(), getMode(), getOffset(), getPixelOffset(), area);
            if (getMode() != SM_Unknow)
            {
                Vector2f out;
                calcAspectRatio(getMode(), N_Only(UI).getDisplaySize(N_UI_Viewport), getNativeRes(), out);
                mSize.mWidth = mArea.getSize().mWidth * out.x;
                mSize.mHeight = mArea.getSize().mHeight * out.y;
            }
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::setTexture(Texture * tex)
    { 
        if (getTexture() != tex)
        {
            mGroup = N_Only(Pixel).create(tex, getMode(), getOffset(), getPixelOffset(), getNativeRes());
        }
    }
    //------------------------------------------------------------------------
    void PixelBuffer::notifyRatio(const Vector2f & aspect)
    {
        //Vector2f out;
        //calcAspectRatio(getMode(), size, getNativeRes(), out);
        mSize.mWidth = mArea.getSize().mWidth * aspect.x;
        mSize.mHeight = mArea.getSize().mHeight * aspect.y;
        //mOffset.x = mPixelOffset.x * out.x;
        //mOffset.y = mPixelOffset.y * out.y;
    }
    //------------------------------------------------------------------------
    void PixelBuffer::calcAspectRatio(ScaleMode mode, const PlaneSizei & dest, const PlaneSizef & src, Vector2f & out)
    {
		switch (mode)
		{
			case SM_Unknow:
			{
				out.x = 1.0f;
				out.y = 1.0f;
			}
			break;
			case SM_Y:
			{
				out.x = dest.mHeight / src.mHeight;
				out.y = out.x;
			}
			break;
			case SM_X:
			{
				out.x = dest.mWidth / src.mWidth;
				out.y = out.x;
			}
			break;
			case SM_Min:
			{
				out.x = std::min(dest.mWidth / src.mWidth, dest.mHeight / src.mHeight);
				out.y = out.x;
			}
			break;
			case SM_Max:
			{
				out.x = std::max(dest.mWidth / src.mWidth, dest.mHeight / src.mHeight);
				out.y = out.x;
			}
			break;
			case SM_ALL:
			{
				out.x = dest.mWidth / src.mWidth;
				out.y = dest.mHeight / src.mHeight;
			}
			default:
				//N_assert(false, "Invalid ScaleMode");
				break;
		}
    }
    //------------------------------------------------------------------------
}