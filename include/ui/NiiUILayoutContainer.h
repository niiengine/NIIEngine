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

#ifndef _NII_UI_LAYOUTCONTAINER_H_
#define _NII_UI_LAYOUTCONTAINER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 排版容器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LayoutContainer : public Container
    {
    public:
        LayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~LayoutContainer();

        /// @copydetails Widget::update
        virtual void update(TimeDurMS cost);

        /// @copydetails Widget::getChildInnerArea
        virtual const CalcRect & getChildInnerArea() const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();

        /// @copydoc Container::addImpl
        virtual void addImpl(Widget * sub);

        /// @copydoc Container::removeImpl
        virtual void removeImpl(Widget * sub);
    protected:
        /// @copydetails Widget::getInnerAreaImpl
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;
        
		/** 执行排版
		@version NIIEngine 3.0.0
		*/
		virtual void layoutImpl() = 0;

        /** 获取子成员区域实现
        @version NIIEngine 3.0.0
        */
        Rectf getChildAreaImpl(bool pixelAlign) const;

        /** 排版函数
        @version NIIEngine 3.0.0 高级api
        */
        virtual void LayoutPrc(const EventArgs * e);
    protected:
        typedef multimap<Widget *, SignalPtr>::type SignalList;
        SignalList mSignalList;
        CalcRect mChildInnerArea;
        bool mLayout;
    };
}
}
#endif