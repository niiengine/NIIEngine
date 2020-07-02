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

#ifndef _NII_UI_ListboxTextItem_H_
#define _NII_UI_ListboxTextItem_H_

#include "NiiUIPreInclude.h"
#include "NiiUIListboxItem.h"
#include "NiiUIPixelUnitGrid.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListboxTextItem : public ListboxItem
    {
    public:
        ListboxTextItem(const String & text, Nui32 id = 0, void * data = 0,
            bool disable = false, bool autoDestroy = true);
        virtual ~ListboxTextItem() {}

        /**
        @version NIIEngine 3.0.0
        */
        void setFont(Font * font);
        
        /**
        @version NIIEngine 3.0.0
        */
        Font * getFont() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setColour(const ColourArea & cols);

        /**
        @version NIIEngine 3.0.0
        */
        inline ColourArea getColour() const {return mColour;}

        /** 
        @version NIIEngine 3.0.0
        */
        void setTextLayout(bool enable);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isTextLayout() const{ return mTextLayout; }

        /// @copydetails Widget::setText
        void setText(const String & text);

        /// @copydetails
        bool notifyFont(const Font * font);

        /// @copydetails
        PlaneSizef getPixelSize(void) const;

        /// @copydetails
        void draw(GeometryPixel & buffer, const Rectf & rect, NIIf alpha, const Rectf * clip) const;
    protected:
        Font * mFont;
        ColourArea mColour;
        mutable PixelUnitGrid mPixelGrid;
        mutable bool mGridValid;
        bool mTextLayout;
    };
}
}
#endif