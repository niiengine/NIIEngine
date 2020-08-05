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

#ifndef _NII_TEXTUREFRAME_H_
#define _NII_TEXTUREFRAME_H_

#include "NiiPreInclude.h"
#include "NiiFrameObj.h"

namespace NII
{
    /** 帧输出到纹理帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureFrame : public FrameObj
    {
    public:
        TextureFrame(FrameBuffer * dst, NCount z);
        virtual ~TextureFrame();

        /** 填充到像素块中
        @version NIIEngine 3.0.0
        */
        virtual void fill(FaceType type, const Box & src, PixelBlock & dst);

        /** 获取像素格式
        @version NIIEngine 3.0.0
        */
        PixelFormat getFormat() const;
    protected:
        FrameBuffer * mBuffer;
        NCount mZOft;
    };
}
#endif