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

#ifndef _NII_UI_PIXEL_H_
#define _NII_UI_PIXEL_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiVector2.h"
#include "NiiRect.h"
#include "NiiColour.h"
#include "NiiScriptProperty.h"

namespace NII
{
namespace UI
{
    /** 像素元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUnit : public UIAlloc
    {
    public:
        virtual ~PixelUnit();

        /** 设置x方向排版
        @version NIIEngine 3.0.0
        */
        inline void setHLayout(HLayout hl){ mHLayout = hl;}

        /** 获取x方向排版
        @version NIIEngine 3.0.0
        */
        inline HLayout getHLayout() const{ return mHLayout;}

        /** 设置y方向排版
        @version NIIEngine 3.0.0
        */
        inline void setVLayout(VLayout vl){ mVLayout = vl; }

        /** 获取y方向排版
        @version NIIEngine 3.0.0
        */
        inline VLayout getVLayout() const{ return mVLayout;}

        /** 设置四边填充
        @version NIIEngine 3.0.0
        */
        inline void setPadding(const Rectf & pad){ mPadding = pad; }

        /** 获取四边填充
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getPadding() const{ return mPadding;}

        /** 设置是否固定宽高比
        @version NIIEngine 3.0.0
        */
        inline void setFixedAspect(bool set){ mFixedAspect = set;}

        /** 获取是否固定宽高比
        @version NIIEngine 3.0.0
        */
        inline bool getFixedAspect() const{ return mFixedAspect; }

        /** 渲染几何单元到几何像素中
        @param[in] target 实际应用到的UI单元
        @version NIIEngine 3.0.0
        */
        virtual void render(GeometryPixel * gp, const Widget * target, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * colour) const = 0;

        /** 获取单元应用时实际大小
        @param[in] target 实际应用到的UI单元
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getPixelSize(const Widget * dest) const = 0;

        /** 复合单元
        @version NIIEngine 3.0.0
        */
        virtual bool isMultiUnit() const = 0;

        /** 选中后的图案
        @note 一般是半透明状
        @version NIIEngine 3.0.0
        */
        inline void setSelectView(PixelBuffer * view){ mSelectPixel = view;}
        
        /** 选中后的图案
        @note 一般是半透明状
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getSelectView() const { return mSelectPixel;}

        /** 设置复合选区
        @param[in] start 开始宽度
        @param[in] end 结束宽度
        @version NIIEngine 3.0.0
        */
        virtual void setSelect(const Widget * target, NIIf start, NIIf end) = 0;

        /** 分离复合单元
        @param[in] start 分割开始宽度
        @return 分离出的左侧
        @version NIIEngine 3.0.0
        */
        virtual PixelUnit * split(const Widget * target, NIIf pixelsplitpoint, Nidx startindex);

        /** 空格数量
        @note MultiUnit,空白符号数量,Space/Tab(Space4X)均是空白符号
        @version NIIEngine 3.0.0
        */
        virtual NCount getBlankCount() const;

        /** 副本对象
        @note 使用 N_delete 释放内存
        @version NIIEngine 3.0.0
        */
        virtual PixelUnit * clone() const = 0;
    protected:
        PixelUnit();
    protected:
        PixelBuffer * mSelectPixel;
        Rectf mPadding;
        VLayout mVLayout;
        HLayout mHLayout;
        bool mFixedAspect;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextPixelUnit : public PixelUnit
    {
    public:
        TextPixelUnit();
        TextPixelUnit(const String & text);
        TextPixelUnit(const String & text, const Font * font);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setText(const String & text){ mText = text;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const{return mText;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setFont(const Font * font){mFont = font;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const Font * getFont() const{ return mFont; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour){mColour = colour;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const{return mColour;}

        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf hegiht, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);
        
        /// @copydetails PixelUnit::split
        PixelUnit * split(const Widget * dest, NIIf pixelsplitpoint, Nidx startindex);

        /// @copydetails PixelUnit::getBlankCount
        NCount getBlankCount() const;
        
        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        const Font * getDestFont(const Widget * dest) const;
    protected:
        String mText;
        const Font * mFont;
        ColourArea mColour;
        NCount mSelectBegin;
        NCount mSelectLength;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImagePixelUnit : public PixelUnit
    {
    public:
        ImagePixelUnit();
        ImagePixelUnit(const PixelBuffer * pb);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setImage(const PixelBuffer * pb) { mImage = pb; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getImage() const { return mImage; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & cr) { mColour = cr;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const{return mColour;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSize(const PlaneSizef & sz) { mDestSize = sz;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const{ return mDestSize; }

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;
        
        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);
        
        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;
        
        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        const PixelBuffer * mImage;
        ColourArea mColour;
        PlaneSizef mDestSize;
        bool mSelect;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetPixelUnit : public PixelUnit
    {
    public:
        WidgetPixelUnit();
        WidgetPixelUnit(WidgetID wid);
        WidgetPixelUnit(Widget * widget);

        /**
        @version NIIEngine 3.0.0
        */
        void setWindow(WidgetID wid);

        /**
        @version NIIEngine 3.0.0
        */
        void setWindow(Widget * widget);

        /**
        @version NIIEngine 3.0.0
        */
        const Widget * getWindow() const;

        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);

        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        Widget * getTarget(const Widget * dest) const;
    protected:
        WidgetID mWidgetID;
        mutable Widget * mTarget;
        mutable bool mTargetValid;
        bool mSelect;
    };
    
    /** 像素单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUnitGrid : public UIAlloc
    {
    public:
        typedef vector<PixelUnit *>::type UnitList;
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
        void add(const PixelUnit & com);

        /**
        @version NIIEngine 3.0.0
        */
        void addEnter();
        
        /**
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /**
        @version NIIEngine 3.0.0
        */
        inline NCount getDataCount() const          { return mUnitList.size(); }

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
        inline NCount getLineCount() const          { return mLineList.size(); }

        /**
        @version NIIEngine 3.0.0
        */
        void setSelection(const Widget * widget, NIIf start, NIIf end);
    protected:
        typedef std::pair<NCount, NCount> LineInfo;
        typedef vector<LineInfo>::type LineList;
    protected:
        UnitList mUnitList;
        LineList mLineList;
    };

    class _EngineAPI TextView : public UIAlloc
    {
    public:
        virtual ~TextView();
        /** 
        @version NIIEngine 3.0.0
        */
        inline void setFont(FontID fid)                 {  mFont = fid; }

        /**
        @version NIIEngine 3.0.0
        */
        inline FontID getFont() const                   { return mFont; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour){  mColour = colour; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const     { return mColour; }
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual PixelUnitGrid parse(const String & text, const Font * font, const ColourArea * colour);
    protected:
        FontID mFont;    
        ColourArea mColour;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EffectTextView : public ScriptProperty, public TextView
    {
    public:
        EffectTextView();
        EffectTextView(FontID font, const ColourArea & colour);

        ///@copydetails TextView::parse
        PixelUnitGrid parse(const String & text, const Font * font, const ColourArea * colour);
    protected:
        VLayout mVLayout;
        PlaneSizef mSize;
        Rectf mPadding;
        FontID mRenderFont;
        ColourArea mRenderColour;
        bool mAspect;
    };
    
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
        inline void setGrid(const PixelUnitGrid * grid)     { mPixelGrid = grid; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const PixelUnitGrid * getGrid() const        { return mPixelGrid; }

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
        virtual NCount getRowCount() const                  { return mPixelGrid->getLineCount(); }

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

        ///@copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;

        ///@copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        vector<NIIf> mSpaceCache;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    template <typename T> class ViewWrap : public PixelLayout
    {
    public:
        ViewWrap(const PixelUnitGrid * str) : PixelLayout(str){}
        ~ViewWrap();

        ///@copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area_size);
        
        ///@copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & buffer, const Vector2f & pos,
            const ColourArea * mc, const Rectf * clip_rect) const;
            
        ///@copydetails PixelLayout::getRowCount
        NCount getRowCount() const      { return mLineList.size();}
        
        ///@copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;
        
        ///@copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        typedef vector<PixelLayout *>::type LineList;
        LineList mLineList;
    };
    //------------------------------------------------------------------------
    template<> _EngineAPI void ViewWrap<JustifyLayout>::layout(const Widget * dest, const PlaneSizef & area_size);
    //------------------------------------------------------------------------
    template <typename T> ViewWrap<T>::~ViewWrap()
    {
        NCount i, iend = mLineList.size();
        for(i = 0; i < iend; ++i)
        {
            const PixelUnitGrid * rs = mLineList[i]->getGrid();
            N_delete rs;
            N_delete mLineList[i];
        }
        mLineList.clear();
    }
    //------------------------------------------------------------------------
    template <typename T>
    void ViewWrap<T>::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        NCount i, iend = mLineList.size();
        for(i = 0; i < iend; ++i)
        {
            const PixelUnitGrid * rs = mLineList[i]->getGrid();
            N_delete rs;
            N_delete mLineList[i];
        }
        mLineList.clear();

        PixelUnitGrid rstring, lstring;
        rstring = *mPixelGrid;

        T * frs;
        NCount l, lend = rstring.getLineCount();
        for(l = 0; l < lend; ++l)
        {
            NIIf rs_width;
            while((rs_width = rstring.getPixelSize(dest, l).mWidth) > 0)
            {
                if (rs_width <= area_size.mWidth)
                    break;
                rstring.split(dest, l, area_size.mWidth, lstring);
                frs = N_new T(N_new PixelUnitGrid(lstring));
                frs->layout(dest, area_size);
                mLineList.push_back(frs);
                l = 0;
            }
        }

        frs = N_new T(N_new PixelUnitGrid(rstring));
        frs->layout(dest, area_size);
        mLineList.push_back(frs);
    }
    //------------------------------------------------------------------------
    template <typename T> void ViewWrap<T>::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & pos, const ColourArea * colour, const Rectf * clip) const
    {
        Vector2f line_pos(pos);
        LineList::const_iterator i, iend = mLineList.begin();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            (*i)->draw(dest, buffer, line_pos, colour, clip);
            line_pos.y += (*i)->getVExtent(dest);
        }
    }
    //------------------------------------------------------------------------
    template <typename T> NIIf ViewWrap<T>::getHExtent(const Widget * dest) const
    {
        NIIf re = 0;
        LineList::const_iterator i, iend = mLineList.end();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            NIIf w = (*i)->getHExtent(dest);
            if (w > re)
                re = w;
        }

        return re;
    }
    //------------------------------------------------------------------------
    template <typename T> NIIf ViewWrap<T>::getVExtent(const Widget * dest) const
    {
        NIIf re = 0;
        LineList::const_iterator i, iend = mLineList.end();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            re += (*i)->getVExtent(dest);
        }
        return re;
    }
    //------------------------------------------------------------------------
}
}
#endif