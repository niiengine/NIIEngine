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

#ifndef _NII_MultiTextureFrame_H_
#define _NII_MultiTextureFrame_H_

#include "NiiPreInclude.h"
#include "NiiFrameObj.h"

namespace NII
{
    /** 帧输出到多个纹理
    @note 纹理基础格式必须相同
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiTextureFrame : public FrameObj
    {
    public:
        typedef vector<TextureFrame *>::type AttachList;
    public:
        MultiTextureFrame(const String & name);

        /** 附加
        @param[in] index 索引
        @param[in] obj 对象
        @version NIIEngine 3.0.0
        */
        virtual void attach(Nidx index, TextureFrame * obj);

        /** 解除
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        virtual void detach(Nidx index);

        /** 获取
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        TextureFrame * getAttach(NCount index);

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const AttachList & getAttachList() const;

        /// @copydetails FrameObj::fill
        virtual void fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst);

        /// @copydetails FrameObj::getFormat
        PixelFormat getFormat() const;
    protected:
        /** 实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void attachImpl(Nidx index, TextureFrame * obj) = 0;

        /** 实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void detachImpl(Nidx index) = 0;
    protected:
        AttachList mAttachList;
    };
}

#endif