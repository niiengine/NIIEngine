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
#include "NiiMultiTextureFrame.h"
#include "NiiTextureFrame.h"
#include "NiiException.h"
#include "NiiGpuBuffer.h"
#include "NiiImage.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TextureFrame
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    TextureFrame::TextureFrame(FrameBuffer * buffer, NCount zoffset):
        FrameObj(0),
        mBuffer(buffer),
        mZOft(zoffset)
    {
        mFlushLevel = NII_REND_TO_TEX_RT_GROUP;
        mWidth = (Nui32)mBuffer->getWidth();
        mHeight = (Nui32)mBuffer->getHeight();
        mColourDepth = (Nui32)PixelUtil::getUnitSize(mBuffer->getFormat()) * 8;
    }
    //-----------------------------------------------------------------------------
    TextureFrame::~TextureFrame()
    {
        mBuffer->destroyImpl(this, 0);
    }
    //-----------------------------------------------------------------------------
    void TextureFrame::fill(FaceType buffer, const Box & src, PixelBlock & dst)
    {
        if(buffer == FT_Unknow)
        {
            buffer = FT_Front;
        }
        if(buffer != FT_Front)
        {
            N_EXCEPT(InvalidParam, _I18n("Invalid buffer."));
        }

        mBuffer->read(src, dst);
    }
    //---------------------------------------------------------------------
    PixelFormat TextureFrame::getFormat() const
    {
        return mBuffer->getFormat();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MultiTextureFrame
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MultiTextureFrame::MultiTextureFrame(const String & name):
        FrameObj(0, name)
    {
        mFlushLevel = NII_REND_TO_TEX_RT_GROUP;
        // Width and height is unknown with no targets attached
        mWidth = mHeight = 0;
    }
    //------------------------------------------------------------------------
    void MultiTextureFrame::fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst)
    {
        N_EXCEPT(InvalidParam, _I18n("Cannot get MultiTextureFrames pixels"));
    }
    //------------------------------------------------------------------------
    void MultiTextureFrame::attach(Nidx index, TextureFrame * obj)
    {
        for(NCount i = mAttachList.size(); i <= index; ++i)
        {
            mAttachList.push_back(0);
        }
        mAttachList[index] = obj;

        attachImpl(index, obj);
    }
    //------------------------------------------------------------------------
    void MultiTextureFrame::detach(Nidx index)
    {
        if(index < mAttachList.size())
            mAttachList[index] = 0;
        detachImpl(index);
    }
    //------------------------------------------------------------------------
    TextureFrame * MultiTextureFrame::getAttach(NCount index)
    {
        N_assert (index < mAttachList.size(), "error");
        return mAttachList[index];
    }
    //------------------------------------------------------------------------
    const MultiTextureFrame::AttachList & MultiTextureFrame::getAttachList() const
    {
        return mAttachList;
    }
    //------------------------------------------------------------------------
    PixelFormat MultiTextureFrame::getFormat() const
    {
        return PF_UNKNOWN;
    }
    //------------------------------------------------------------------------
}