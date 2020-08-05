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

#ifndef _NII_EXT_FRAMEBUFFER_H_
#define _NII_EXT_FRAMEBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 帧的扩展缓存
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI ExtFrameBuffer : public BufferAlloc
    {
    public:
        ExtFrameBuffer(GroupID gid, NCount w, NCount h, NCount bit, Nui32 fsaa, 
            const String & hint, bool manual);

        virtual ~ExtFrameBuffer();

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mWidth; }

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mHeight; }

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getBitDepth() const { return mBitDepth; }

        /** 获取各异向因子
        @version NIIEngine 3.0.0
        */
        inline NCount getFsaa() const { return mFsaa; }

        /** 获取各异向因子提示
        @version NIIEngine 3.0.0
        */
        inline const String & getFsaaHint() const { return mFsaaHint; }

        /** 是否合适这个帧缓存对象
        @version NIIEngine 3.0.0
        */
        virtual bool isSuitable(FrameObj * fo) const;

        /** 附加到帧缓存对象中
        @version NIIEngine 3.0.0
        */
        virtual void attach(FrameObj * fo);

        /** 冲帧缓存对象中解除
        @version NIIEngine 3.0.0
        */
        virtual void detach(FrameObj * fo);
        
        /** 设置组
        @version NIIEngine 3.0.0 高级api
        */
        void setGroup(GroupID gid);

        /** 获取组
        @version NIIEngine 3.0.0 高级api
        */
        inline GroupID getGroup() const { return mPoolId; }

        /** 设置属于
        @version NIIEngine 3.0.0 高级api
        */
        inline void setManual(bool b) { mManual = b; }
        
        /** 设置属于
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isManual() const { return mManual; }
    protected:
        typedef set<FrameObj *>::type UserList;
    protected:
        GroupID mPoolId;
        NCount mWidth;
        NCount mHeight;
        NCount mBitDepth;
        NCount mFsaa;
        String mFsaaHint;
        UserList mUserList;
        bool mManual;
    };
    typedef vector<ExtFrameBuffer *>::type ExtFrameBufferList;
}
#endif
