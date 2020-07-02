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

#ifndef _NII_UI_VIEWLAYOUT_H_
#define _NII_UI_VIEWLAYOUT_H_

#include "NiiUIPreInclude.h"
#include "NiiUIPixelUnitGrid.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelLayout : public UIAlloc
    {
    public:
        PixelLayout(const PixelUnitGrid * grid);
        virtual ~PixelLayout();
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setGrid(const PixelUnitGrid * grid){ mPixelGrid = grid; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const PixelUnitGrid * getGrid() const{ return mPixelGrid; }

        /** 排版
        @version NIIEngine 3.0.0
        */
        virtual void layout(const Widget * dest, const PlaneSizef & area) = 0;

        /** 绘制
        @version NIIEngine 3.0.0
        */
        virtual void draw(const Widget * dest, GeometryPixel & out, const Vector2f & pos, 
            const ColourArea * colour, const Rectf * clip) const = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual NCount getRowCount() const = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual NIIf getHExtent(const Widget * dest) const = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual NIIf getVExtent(const Widget * dest) const = 0;
    protected:
        const PixelUnitGrid * mPixelGrid;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LeftLayout : public PixelLayout
    {
    public:
        LeftLayout(const PixelUnitGrid * grid);

        /// @copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area);

        /// @copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & out, const Vector2f & pos, 
            const ColourArea * colour, const Rectf * clip) const;

        /// @copydetails PixelLayout::getRowCount
        NCount getRowCount() const;

        /// @copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;

        /// @copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RightLayout : public PixelLayout
    {
    public:
        RightLayout(const PixelUnitGrid * grid);

        /// @copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area);

        /// @copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & out, const Vector2f & pos, 
            const ColourArea * colour, const Rectf * clip) const;

        /// @copydetails PixelLayout::getRowCount
        NCount getRowCount() const;

        /// @copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;

        /// @copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        vector<NIIf>::type mSpaceCache;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CenterLayout : public PixelLayout
    {
    public:
        CenterLayout(const PixelUnitGrid * grid);

        ///@copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area);

        ///@copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & out, const Vector2f & pos,
            const ColourArea * colour, const Rectf * clip) const;

        ///@copydetails PixelLayout::getRowCount
        NCount getRowCount() const;

        ///@copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;

        ///@copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        vector<NIIf>::type mSpaceCache;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JustifyLayout : public PixelLayout
    {
    public:
        JustifyLayout(const PixelUnitGrid * grid);

        ///@copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area);
        
        ///@copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & out, const Vector2f & pos,
            const ColourArea * colour, const Rectf * clip) const;

        ///@copydetails PixelLayout::getRowCount
        NCount getRowCount() const;

        ///@copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;

        ///@copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        std::vector<NIIf> mSpaceCache;
    };
}
}
#endif