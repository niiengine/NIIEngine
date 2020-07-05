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
#include "NiiUIWidget.h"
#include "NiiColourArea.h"
#include "NiiUIStyleArea.h"
#include "NiiUIPixelLayout.h"
#include "NiiPropertyType.h"
#include "NiiXMLSerializer.h"
#include "NiiGeometryPixel.h"

namespace NII
{
namespace UI
{
    /** 风格成分
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSectionUnit : public UIAlloc
    {
    public:
        StyleSectionUnit();
        virtual ~StyleSectionUnit();

        /** 相等
        @verison NIIEngine 3.0.0
        */
        virtual bool equal(const StyleSectionUnit & o) const;

        /** 渲染
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** 渲染
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** 设置区域
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area)     { mArea = area; }

        /** 获取区域
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const        { return mArea;}

        /** 设置颜色区域
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour){ mColour = colour;}

        /** 设置颜色区域来源
        @version NIIEngine 3.0.0
        */
        inline void setColour(PropertyID pid)           { mColourID = pid; }

        /** 获取颜色区域
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const     { return mColour; }
        
        /** 字体改变时回调
        @version NIEngine 3.0.0
        */
        virtual bool notifyFont(Widget * e, const Font * font) const;
        
        /** 写入成分
        @version NIIEngine 3.0.0
        */
        virtual void write(XmlSerializer * out) const;

        /** 写入颜色
        @version NIIEngine 3.0.0
        */
        virtual bool writeColour(XmlSerializer * out) const;
    protected:
        /** 渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(const Widget * dest, const Rectf & rect, const ColourArea * colour, const Rectf * clip) const = 0;

        /** 获取颜色
        @version NIIEngine 3.0.0 高级api
        */
        virtual void getColourImpl(const Widget * w, const ColourArea * colour, ColourArea & out) const;
    protected:
        StyleArea mArea;
        ColourArea mColour;
        PropertyID mColourID;
    };
    
    /** 风格区域
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
        inline void setID(Nid id)          { mID = id; }

        /**
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const           { return mID; }

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
        inline void setColour(const ColourArea & cols)      { mColour = colour; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const         { return mColour; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setColourFetch(PropertyID property)     { mColourID = pid; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline PropertyID getColoursFetch() const           { return mColourID; }

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
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSectionTextUnit : public StyleSectionUnit
    {
    public:
        StyleSectionTextUnit();

        ~StyleSectionTextUnit();

        StyleSectionTextUnit(const StyleSectionTextUnit & o);

        StyleSectionTextUnit & operator=(const StyleSectionTextUnit & o);

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setText(const String & text)    { mText = text; mValidBidi = false; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const       { return mText; }

        /**
        @version NIIEngine 3.0.0
        */
        String getDestText(const Widget & e) const;

        /**
        @version NIIEngine 3.0.0
        */
        const String & getVisualText() const;

        /**
        @version NIIEngine 3.0.0
        */
        String getEffectText(const Widget & e) const;

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setFont(FontID fid){  mFont = fid;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline FontID getFont() const{ return mFont;}

        /**
        @version NIIEngine 3.0.0
        */
        FontID getDestFont(const Widget & e) const;

        /** 
        @version NIIEngine 3.0.0
        */
        inline VTextLayout getVLayout(const Widget & e) const   { return mVLayout.get(&e); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setVLayout(VTextLayout fmt)     { mVLayout.set(fmt); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline HTextLayout getHLayout(const Widget & e) const   { return mHLayout.get(&e); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setHLayout(HTextLayout fmt)     { mHLayout.set(fmt); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setHLayoutID(PropertyID pid)    { mHLayout.setSrcFetch(pid); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setVLayoutID(PropertyID pid)    { mVLayout.setSrcFetch(pid); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setTextID(PropertyID pid)       { mTextID = pid; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline PropertyID getTextID() const         { return mTextID;} 
        
        /**
        @param id
        @version NIIEngine 3.0.0
        */
        inline void setFontID(PropertyID pid)       { mFontID = pid; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline PropertyID getFontID() const         { return mFontID;}

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getHTextExtent(const Widget & e) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getVTextExtent(const Widget & e) const;
        
        /// @copydetails StyleSectionUnit::write
        void write(XmlSerializer * out) const;

        /// @copydetails StyleSectionUnit::notifyFont
        bool notifyFont(Widget * dest, const Font * font) const;
    protected:
        /// @copydetails StyleSectionUnit::renderImpl
        void renderImpl(const Widget * dest, const Rectf & rect, const ColourArea * colour, const Rectf * clip) const;

        void setupLayout(const Widget * dest, const PixelUnitGrid * text) const;

        /// @copydetailsStyleSectionUnit::equal
        bool equal(const StyleSectionUnit & o) const;

        /// helper to get the font object to use
        const Font * getFont(const Widget * e) const;
    private:
        String mText;
        BidiText * mBidiText;
        FontID mFont;    
        PropertyID mTextID; 
        PropertyID mFontID;
        VTextLayoutPropertyType mVLayout;
        HTextLayoutPropertyType mHLayout;
        mutable PixelUnitGrid mRenderText;
        mutable SharedPtr<PixelLayout> mLayoutText;
        mutable HTextLayout mLastHLayout;
        mutable bool mValidBidi;        
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSectionImageUnit : public StyleSectionUnit
    {
    public:
        StyleSectionImageUnit();

        /**
        @version NIIEngine 3.0.0
        */
        inline void setImage(PixelBuffer * pb)      { mImage = pb; }

        /**
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * getImage() const       { return mImage; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setVLayout(VLayout vl)          { mVLayout.set(vl); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline VLayout getVLayout(const Widget & widget) const{ return mVLayout.get(&widget); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setHLayout(HLayout hl)          { mHLayout.set(hl); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline HLayout getHLayout(const Widget & widget) const{ return mHLayout.get(&widget); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setHLayoutID(PropertyID pid)    { mHLayout.setSrcFetch(pid); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setVLayoutID(PropertyID pid)    { mVLayout.setSrcFetch(pid); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setImageSrc(PropertyID pid)     { mImageID = pid; }

        /**
        @version NIIEngine 3.0.0
        */
        inline PropertyID getImageSrc() const       { return mImageID; }
        
        /// @copydetails StyleSectionUnit::write
        void write(XmlSerializer * out) const;
    protected:
        /// @copydetails StyleSectionUnit::renderImpl
        void renderImpl(const Widget * dest, const Rectf & rect, const ColourArea * colour, const Rectf * clip) const;

        /// @copydetails StyleSectionUnit::equal
        bool equal(const StyleSectionUnit & o) const;
    protected:
        PropertyID mImageID;
        PixelBuffer * mImage;
        VLayoutPropertyType mVLayout;
        HLayoutPropertyType mHLayout;
    };
    
    /** 边框成分类型
    @version NIIEngine 3.0.0
    */
    enum FrameComType
    {
        FCT_TL,
        FCT_TR,
        FCT_BL,
        FCT_BR,
        FCT_Left,
        FCT_Right,
        FCT_Top,
        FCT_Bottom,
        FCT_Bg,
        FCT_Count
    };

    struct FrameComSrc;

    /** 描述窗体级边框(共9个图片,包含一个背景)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSectionFrameUnit : public StyleSectionUnit
    {
    public:
        StyleSectionFrameUnit();
        virtual ~StyleSectionFrameUnit();

        /** 设置左边排版模式
        @version NIIEngine 3.0.0
        */
        inline void setLeftLayout(VLayout vl)       { mLeftLayout.set(vl); }

        /** 设置右边排版模式
        @version NIIEngine 3.0.0
        */
        inline void setRightLayout(VLayout vl)      { mRightLayout.set(vl); }

        /** 设置上边排版模式
        @version NIIEngine 3.0.0
        */
        inline void setTopLayout(HLayout hl)        { mTopLayout.set(hl);}

        /** 设置下边排版模式
        @version NIIEngine 3.0.0
        */
        inline void setBottomLayout(HLayout hl)     { mBottomLayout.set(hl);}

        /** 设置背景垂直模式
        @version NIIEngine 3.0.0
        */
        inline void setBgVLayout(VLayout vl)        { mBgVLayout.set(vl);}

        /** 设置背景水平模式
        @version NIIEngine 3.0.0
        */
        inline void setBgHLayout(HLayout hl)        { mBgHLayout.set(hl); }

        /** 设置左边排版模式来源
        @version NIIEngine 3.0.0
        */
        inline void setLeftLayoutSrc(PropertyID id) { mLeftLayout.setSrcFetch(id);}

        /** 设置右边排版模式来源
        @version NIIEngine 3.0.0
        */
        inline void setRightLayoutSrc(PropertyID id){ mRightLayout.setSrcFetch(id); }

        /** 设置上边排版模式来源
        @version NIIEngine 3.0.0
        */
        inline void setTopLayoutSrc(PropertyID id)  { mTopLayout.setSrcFetch(id); }

        /** 设置下边排版模式来源
        @version NIIEngine 3.0.0
        */
        inline void setBottomLayoutSrc(PropertyID id){ mBottomLayout.setSrcFetch(id); }

        /** 设置背景垂直模式来源
        @version NIIEngine 3.0.0
        */
        inline void setBgVLayoutSrc(PropertyID id)  { mBgVLayout.setSrcFetch(id); }

        /** 设置背景水平模式来源
        @version NIIEngine 3.0.0
        */
        inline void setBgHLayoutSrc(PropertyID id)  { mBgHLayout.setSrcFetch(id); }

        /** 获取左边排版模式
        @version NIIEngine 3.0.0
        */
        inline VLayout getLeftLayout(const Widget * o) const{ return mLeftLayout.get(o); }

        /** 获取右边排版模式
        @version NIIEngine 3.0.0
        */
        inline VLayout getRightLayout(const Widget * o) const{ return mRightLayout.get(o); }

        /** 获取上边排版模式
        @version NIIEngine 3.0.0
        */
        inline HLayout getTopLayout(const Widget * o) const{ return mTopLayout.get(o); }

        /** 获取下边排版模式
        @version NIIEngine 3.0.0
        */
        inline HLayout getBottomLayout(const Widget * o) const{ return mBottomLayout.get(o);}

        /** 获取背景垂直模式
        @version NIIEngine 3.0.0
        */
        inline VLayout getBgVLayout(const Widget * o) const{ return mBgVLayout.get(o); }

        /** 获取背景水平模式
        @version NIIEngine 3.0.0
        */
        inline HLayout getBgHLayout(const Widget * o) const{ return mBgHLayout.get(o);}

        /** 设置图片
        @version NIIEngine 3.0.0
        */
        void setImage(FrameComType part, const PixelBuffer * pb);

        /** 设置图片
        @version NIIEngine 3.0.0
        */
        void setImage(FrameComType part, ResourceID rid, PixelBufferID pbid);

        /** 设置图片属性来源
        @version NIIEngine 3.0.0
        */
        void setImageSrc(FrameComType part, PropertyID pid);

        /** 获取图片成分
        @version NIIEngine 3.0.0
        */
        const PixelBuffer * getImage(FrameComType part, const Widget * o) const;

        /** 是否存在边框图片
        @version NIIEngine 3.0.0
        */
        bool isFrameExist(FrameComType part) const;

        /** 是否从属性中获取图片
        @version NIIEngine 3.0.0
        */
        bool isFetchSrc(FrameComType part) const;

        /** 获取获取图片属性
        @version NIIEngine 3.0.0
        */
        PropertyID getImageSrc(FrameComType part) const;

        /** 写入到 XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        /// @copydetails StyleSectionUnit::renderImpl
        void renderImpl(const Widget * dest, const Rectf & rect, const ColourArea * colour, const Rectf * clip) const;

        void renderImage(GeometryPixel & buffer, const PixelBuffer * pb, VLayout vlayout,
            HLayout hlayout, Rectf & rect, const ColourArea & colours, const Rectf * clipper) const;

        /// @copydetails StyleSectionUnit::equal
        bool equal(const StyleSectionUnit & o) const;
    protected:
        VLayoutPropertyType mLeftLayout;
        VLayoutPropertyType mRightLayout;
        HLayoutPropertyType mTopLayout;
        HLayoutPropertyType mBottomLayout;
        VLayoutPropertyType mBgVLayout;
        HLayoutPropertyType mBgHLayout;
        FrameComSrc * mFrames;
    };
}
}
#endif