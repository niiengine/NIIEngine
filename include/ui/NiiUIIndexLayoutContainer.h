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

#ifndef _NII_UI_INDEX_LayoutContainer_H_
#define _NII_UI_INDEX_LayoutContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUILayoutContainer.h"

namespace NII
{
namespace UI
{
    /** 索引排版容器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexLayoutContainer : public LayoutContainer
    {
    public:
        IndexLayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~IndexLayoutContainer();

        using Container::add;

        using Container::remove;


        /** 交换元素
        @version NIIEngine 3.0.0
        */
        void swap(Nidx sub1, Nidx sub2);

        /** 交换元素
        @version NIIEngine 3.0.0
        */
        void swap(Widget * sub1, Widget * sub2);

        /** 交换元素
        @version NIIEngine 3.0.0
        */
        void move(Widget * sub, Nidx index);

        /** 推前元素
        @version NIIEngine 3.0.0
        */
        void forward(Widget * sub, NCount count = 1);

        /** 添加
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub, Nidx index);

        /** 移去
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);
    public:
        /** 子元素排序变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID ChildOrderChangeEvent;

        static const EventID EventCount;
    protected:
        /** 子元素排序变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onChildOrderChange(const WidgetEventArgs * arg);
    };
}
}
#endif