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

#ifndef _NII_UI_LAYOUTCELL_H_
#define _NII_UI_LAYOUTCELL_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 排版格
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LayoutCell : public Container
    {
    public:
        LayoutCell(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~LayoutCell();

        /// @copydetails Container::getChildInnerArea
        virtual const CalcRect & getChildInnerArea() const;

        /// @copydetails Container::notifyPosChange
        virtual void notifyPosChange();

        /// @copydetails Container::addImpl
        virtual void addImpl(Widget * sub);
    protected:
        /// @copydoc Widget::getInnerAreaImpl
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;

        /** 获取子成员区域实现
        @version NIIEngine 3.0.0
        */
        Rectf getChildAreaImpl(bool pixelAlign) const;
    protected:
        CalcRect mChildInnerArea;
    };
}
}
#endif