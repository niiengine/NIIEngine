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
    /** 3D�ռ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Box
    {
    public:
        Box() : mLeft(0), mTop(0), mRight(1), mBottom(1), mFront(0), mBack(1){}
        
        /** �������
        @remark (mFront = 0 and mBack = 1)
        @param[in] l ���
        @param[in] t �ϱ�
        @param[in] r �ұ�
        @param[in] b �±�
        */
        Box(NCount l, NCount t, NCount r, NCount b):
            mLeft(l), mTop(t), mRight(r), mBottom(b), mFront(0), mBack(1){}
        
        /** �������
        @param[in] l ���
        @param[in] t �ϱ�
        @param[in] f ǰ��
        @param[in] r �ұ�
        @param[in] b �±�
        @param[in] bb ���
        */
        Box(NCount l, NCount t, NCount f, NCount r, NCount bm, NCount bk):
            mLeft(l), mTop(t), mRight(r), mBottom(bm), mFront(f), mBack(bk){}

        /** �Ƿ����
        @verison NIIEngine 3.0.0
        */
        inline bool contains(const Box & o) const
        {
            return (o.mLeft >= mLeft && o.mTop >= mTop && o.mFront >= mFront &&
                o.mRight <= mRight && o.mBottom <= mBottom && o.mBack <= mBack);
        }

        /** ��ȡ������ӵĿ�
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mRight - mLeft; }

        /** ��ȡ������ӵĸ�
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mBottom - mTop; }

        /** ��ȡ������ӵ���
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const { return mBack - mFront; }
    public:
        NCount mLeft, mTop, mRight, mBottom, mFront, mBack;
    };
}
#endif