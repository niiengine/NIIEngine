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

#ifndef _NII_UI_VIEWCOM_H_
#define _NII_UI_VIEWCOM_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiVector2.h"
#include "NiiRect.h"
#include "NiiColour.h"

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
}
}
#endif