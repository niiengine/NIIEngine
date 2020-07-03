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

#ifndef _NII_UI_STYLESECTION_H_
#define _NII_UI_STYLESECTION_H_

#include "NiiUIPreInclude.h"
#include "NiiUIStyleSectionUnit.h"

namespace NII
{
namespace UI
{
    /** ∑Á∏Ò«¯”Ú
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSection : public UIAlloc
    {
    public:
        StyleSection();
        StyleSection(Nid id);
        ~StyleSection();

        /**
        @version NIIEngine 3.0.0
        */
        void setID(Nid id);

        /**
        @version NIIEngine 3.0.0
        */
        Nid getID() const;

        /**
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /**
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /**
        @version NIIEngine 3.0.0
        */
        void add(StyleSectionUnit * unit);

        /**
        @version NIIEngine 3.0.0
        */
        void remove(StyleSectionUnit * unit);

        /**
        @version NIIEngine 3.0.0
        */
        void clearAll();

        /**
        @version NIIEngine 3.0.0
        */
        void setColour(const ColourArea & cols);

        /** 
        @version NIIEngine 3.0.0
        */
        const ColourArea & getColour() const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setColourFetch(PropertyID property);

        /** 
        @version NIIEngine 3.0.0
        */
        PropertyID getColoursFetch() const;

        /** 
		@version NIIEngine 3.0.0
        */
        Rectf getBoundingRect(const Widget & widget) const;

        /** 
		@version NIIEngine 3.0.0
        */
        Rectf getBoundingRect(const Widget & widget, const Rectf & rect) const;

        /**
        @param out
        */
        void write(XmlSerializer * out) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * widget, const Font * font) const;
    protected:
        typedef vector<StyleSectionUnit *>::type UnitList;
    protected:
        Nid mID;
        PropertyID mColourID;
        ColourArea mColour;
        UnitList mUnitList;
    };
}
}
#endif