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
#ifndef _NII_UI_BidiTEXT_H_
#define _NII_UI_BidiTEXT_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** 双向文字类型
    @version NIIEngine 3.0.0
    */
    enum BidiType
    {
        BT_RTL,
        BT_LTR,
        BT_Normal
    };

    /** 双向文字处理
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BidiText : public UIAlloc
    {
    public:
        typedef vector<NIIi> IndexList;
    public:
        virtual ~BidiText();

        /** 获取文字双向类型 
        @param chr 字符
        @note 默认的基础方向,如英语/汉语的基础方向为从左到右,阿拉伯语的基础方向为从右到左.
        @version NIIEngine 3.0.0
        */
        virtual BidiType getBidiType(Nui32 chr) const = 0;

        /** 设置来源字符串
        @version NIIEngine 3.0.0
        */
        bool setSrc(const String & text);
        
        /** 获取目标字符串
        @version NIIEngine 3.0.0
        */
        const String & getDst() const;
        
        /** 重新排部
        @version NIIEngine 3.0.0
        */
        virtual bool reorder(const String & in, String & out) const = 0;

        /** 逻辑到可见映射
        @version NIIEngine 3.0.0
        */
        const IndexList & getLogicalToVisual() const;
        
        /** 可见到逻辑映射
        @version NIIEngine 3.0.0
        */
        const IndexList & getVisualToLogical() const;
    protected:
        IndexList mLTV;
        IndexList mVTL;
        String mDst;
    };
}
}
#endif