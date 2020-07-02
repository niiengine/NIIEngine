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

#ifndef _NII_UI_PixelUnitGrid_H_
#define _NII_UI_PixelUnitGrid_H_

#include "NiiUIPreInclude.h"
#include "NiiPlaneSize.h"
#include "NiiRect.h"

namespace NII
{
namespace UI
{
    /** ÏñËØµ¥Ôª
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUnitGrid : public UIAlloc
    {
    public:
        typedef vector<PixelUnit *>::type ComponentList;
    public:
        PixelUnitGrid();
        PixelUnitGrid(const PixelUnitGrid & o);
        virtual ~PixelUnitGrid();

        PixelUnitGrid & operator=(const PixelUnitGrid & o);

        /**
        @version NIIEngine 3.0.0
        */
        void draw(const Widget * widget, NCount line, GeometryPixel & buffer, const Vector2f & pos, 
            const ColourArea * colour, const Rectf * clip_rect, NIIf space_extra) const;

        /**
        @version NIIEngine 3.0.0
        */
        PlaneSizef getPixelSize(const Widget * widget, NCount line) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getHExtent(const Widget * widget) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getVExtent(const Widget * widget) const;

        /**
        @version NIIEngine 3.0.0
        */
        void appendComponent(const PixelUnit & com);

        /**
        @version NIIEngine 3.0.0
        */
        void appendLineBreak();
        
        /**
        @version NIIEngine 3.0.0
        */
        void clearComponents();

        /**
        @version NIIEngine 3.0.0
        */
        NCount getDataCount() const;

        /** 
        @version NIIEngine 3.0.0
        */
        void split(const Widget * widget, NCount line, NIIf split_point, PixelUnitGrid & left);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getSpaceCount(NCount line) const;

        /**
        @version NIIEngine 3.0.0
        */
        NCount getLineCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setSelection(const Widget * widget, NIIf start, NIIf end);
    protected:
        /**
        @version NIIEngine 3.0.0
        */
        void cloneComponentList(const ComponentList & list);

        /**
        @version NIIEngine 3.0.0
        */
        static void clearComponentList(ComponentList & list);
    protected:
        typedef std::pair<NCount, NCount> LineInfo;
        typedef vector<LineInfo>::type LineList;
    protected:
        ComponentList d_components;
        LineList mLineList;
    };

    class _EngineAPI TextView : public UIAlloc
    {
    public:
        virtual PixelUnitGrid parse(const String & text, const Font * font, const ColourArea * colour) = 0;
    };
}
}
#endif