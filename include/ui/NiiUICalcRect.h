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

#ifndef _NII_UI_CALCRECT_H_
#define _NII_UI_CALCRECT_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiRect.h"

namespace NII
{
namespace UI
{
    typedef Rectf(Widget::*CalcMethod)(bool) const;
    /** �����첽�������
    @version NIIEngine 3.0.0
    */
    class CalcRect
    {
    public:
        enum CacheType
        {
            CT_None     = 0,
            CT_Int      = 0x01,
            CT_Float    = 0x02
        };
    public:
        CalcRect(Widget * obj, CalcMethod m):
            mObj(obj),
            mCalcMethod(m),
            mValidCache(CT_None)
        {
        }

        /** ��ȡ����汾����
        @version NIIEngine 3.0.0
        */
        const Rectf & getFloatCache() const
        {
            if(mValidCache != CT_Float)
            {
                mCacheData = (mObj->*mCalcMethod)(false);
                mValidCache = CT_Float;
            }
            return mCacheData;
        }
        /** ��ȡ���ΰ汾����
        @version NIIEngine 3.0.0
        */
        const Rectf & getIntCache() const
        {
            if(mValidCache != CT_Int)
            {
                mCacheData = (mObj->*mCalcMethod)(true);
                mValidCache = CT_Int;
            }
            return mCacheData;
        }

        /** ��������
        @version NIIEngine UI 3.0.0
        */
        inline void resetCached()
        {
            mValidCache = CT_None;
        }

        /** ����洢�Ƿ�����Ч
        @version NIIEngine 3.0.0
        */
        inline bool isFloatCache()
        {
            return (mValidCache == CT_Float);
        }

        /** ���ʹ洢�Ƿ�����Ч
        @version NIIEngine 3.0.0
        */
        inline bool isIntCache()
        {
            return (mValidCache == CT_Int);
        }
    private:
        Widget * mObj;
        CalcMethod mCalcMethod;
        mutable Rectf mCacheData;
        mutable Nmark mValidCache;
    };
}
}
#endif