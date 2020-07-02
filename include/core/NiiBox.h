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

#ifndef _NII_BOX_H_
#define _NII_BOX_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 3D空间盒
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Box
    {
    public:
        Box() : mLeft(0), mTop(0), mRight(1), mBottom(1), mFront(0), mBack(1){}
        
        /** 定义盒子
        @remark (mFront = 0 and mBack = 1)
        @param[in] l 左边
        @param[in] t 上边
        @param[in] r 右边
        @param[in] b 下边
        */
        Box(NCount l, NCount t, NCount r, NCount b):
            mLeft(l), mTop(t), mRight(r), mBottom(b), mFront(0), mBack(1){}
        
        /** 定义盒子
        @param[in] l 左边
        @param[in] t 上边
        @param[in] f 前边
        @param[in] r 右边
        @param[in] b 下边
        @param[in] bb 后边
        */
        Box(NCount l, NCount t, NCount f, NCount r, NCount bm, NCount bk):
            mLeft(l), mTop(t), mRight(r), mBottom(bm), mFront(f), mBack(bk){}

        /** 是否包含
        @verison NIIEngine 3.0.0
        */
        inline bool contains(const Box & o) const
        {
            return (o.mLeft >= mLeft && o.mTop >= mTop && o.mFront >= mFront &&
                o.mRight <= mRight && o.mBottom <= mBottom && o.mBack <= mBack);
        }

        /** 获取这个盒子的宽
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mRight - mLeft; }

        /** 获取这个盒子的高
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mBottom - mTop; }

        /** 获取这个盒子的深
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const { return mBack - mFront; }
    public:
        NCount mLeft, mTop, mRight, mBottom, mFront, mBack;
    };
}
#endif