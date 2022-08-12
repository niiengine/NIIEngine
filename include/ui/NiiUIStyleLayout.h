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

#ifndef _NII_UI_STYLESTATE_H_
#define _NII_UI_STYLESTATE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiColour.h"

namespace NII
{
namespace UI
{
    /** 区域
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI Section : public UIAlloc
    {
    public:
        Section();
        Section(StyleID style, Nid section, PropertyID vctrlpid, WidgetID vctrlwidget);
        Section(StyleID style, Nid section, PropertyID vctrlpid, WidgetID vctrlwidget, const ColourArea & cols);

        /** 设置使用的风格
        @version NIIEngine 3.0.0
        */
        inline void setStyle(StyleID id)                    { mStyle = id; }

        /** 获取使用的风格
        @version NIIEngine 3.0.0
        */
        inline StyleID getStyle() const                     { return mStyle;}

        /** 设置使用的风格区域
        @version NIIEngine 3.0.0
        */
        inline void setStyleSection(Nid id)                 { mStyleSection = id;}

        /** 获取使用的风格区域
        @version NIIEngine 3.0.0
        */
        inline Nid getStyleSection() const                  { return mStyleSection;}

        /** 设置区域颜色
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour)    { mColour = colour;}

        /** 获取区域颜色
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const         { return mColour;}

        /** 设置区域颜色(来源属性)
        @version NIIEngine 3.0.0
        */
        inline void setColourID(PropertyID pid)             { mColourID = pid;}

        /** 获取区域颜色(来源属性)
        @version NIIEngine 3.0.0
        */
        inline PropertyID getColourID() const               { return mColourID; }

        /** 设置可见控制属性
        @version NIIEngine 3.0.0
        */
        inline void setVisibleCtrlProperty(PropertyID pid)  { mCtlProperty = pid;}

        /** 获取可见控制属性
        @version NIIEngine 3.0.0
        */
        inline PropertyID getVisibleCtrlProperty() const    { return mCtlProperty; }

        /** 获取可见控制主体
        @version NIIEngine 3.0.0
        */
        inline void setVisibleCtrlWidget(WidgetID id)       { mCtlWidget = id; }
        
        /** 获取可见控制主体
        @version NIIEngine 3.0.0
        */
        inline WidgetID getVisibleCtrlWidget() const        { return mCtlWidget;}
        
        /** 渲染区域
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * mc = 0, const Rectf * clip = 0) const;

        /** 渲染区域
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * mc = 0, const Rectf * clip = 0) const;

        /** 写入到Xml
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    private:
        StyleID mStyle;
        Nid mStyleSection;
        ColourArea mColour;
        PropertyID mColourID;
        WidgetID mCtlWidget;
        PropertyID mCtlProperty;
    };
    
    /** UI区域层
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Layer : public UIAlloc
    {
    public:
        Layer(NCount level = 0);

        /** 比较函数
        @version NIIEngine 3.0.0
        */
        inline bool operator<(const Layer & o) const{ return mLevel < o.mLevel;}

        /** 设置层
        @version NIIEngine 3.0.0
        */
        inline void setLevel(NCount level)      { mLevel = level;}

        /** 获取层
        @version NIIEngine 3.0.0
        */
        inline NCount getLevel() const          { return mLevel;}

        /** 添加UI绘制区域
        @version NIIEngine 3.0.0
        */
        inline void add(const Section & obj)    { mSections.push_back(obj); }

        /** 移去所有层
        @version NIIEngine 3.0.0
        */
        inline void removeAll()                 { mSections.clear(); }

        /** 渲染层
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** 渲染层
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** 写入到Xml序列
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        typedef vector<Section>::type Sections;
    protected:
        NCount mLevel;
        Sections mSections;
    };
    
    /** UI绘制风格布局
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleLayout : public UIAlloc
    {
    public:
        StyleLayout();
        StyleLayout(Nid id);
        StyleLayout(const StyleLayout & o);

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id){ mID = id;}

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const{ return mID;}

        /** 设置是否裁剪区域
        @version NIIEngine 3.0.0
        */
        inline void setClip(bool set){ mClip = set;}

        /** 获取是否裁剪区域
        @version NIIEngine 3.0.0
        */
        inline bool isClip() const{ return mClip;}

        /** 添加
        @version NIIEngine 3.0.0
        */
        inline void add(const Layer & layer){ mLayers.insert(layer);}

        /** 移动
        @version NIIEngine 3.0.0
        */
        inline void removeAll(){ mLayers.clear();}

        /** 渲染状态
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * cv = 0, const Rectf * clip = 0) const;

        /** 渲染状态
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * cv = 0, const Rectf * clip = 0) const;

        /** 写入
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        typedef multiset<Layer>::type Layers;
    protected:
        Nid mID;
        Layers mLayers;
        Nmark mMark;
        bool mClip;
    };
}
}
#endif