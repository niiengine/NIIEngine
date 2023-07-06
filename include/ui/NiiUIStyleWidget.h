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

#ifndef _NII_UI_StyleWidget_H_
#define _NII_UI_StyleWidget_H_

#include "NiiUIPreInclude.h"
#include "NiiAssignProperty.h"
#include "NiiUIStyleSpecial.h"

namespace NII
{
namespace UI
{
    /** 风格附属单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleWidget : public UIAlloc
    {
    public:
        typedef vector<AssignProperty>::type PropertyList;
    public:
        StyleWidget(StyleViewID sid, WidgetID wid, bool autowin);

        /** 附加到单元中
        @version NIIEngine 3.0.0
        */
        void attach(Widget * obj) const;

        /** 从单元中解除
        @version NIIEngine 3.0.0
        */
        void detach(Widget * obj) const;

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        inline void setID(WidgetID id)              { mID = id; }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getID() const               { return mID; }

        /** 排版
        @version NIIEngine 3.0.0
        */
        void layout(Widget * dst) const;

        /** 设置区域
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area) { mArea = area; }

        /** 获取区域
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const    { return mArea; }

        /** 设置单元工厂类
        @version NIIEngine 3.0.0
        */
        inline void setViewID(StyleViewID id)       { mViewID = id; }

        /** 获取单元工厂类
        @version NIIEngine 3.0.0
        */
        inline StyleViewID getViewID() const        { return mViewID; }

        /** 设置x方向对齐
        @version NIIEngine 3.0.0
        */
        inline void setHAlign(HAlign align)         { mHAlign = align; }

        /** 设置x方向对齐
        @version NIIEngine 3.0.0
        */
        inline HAlign getHAlign() const             { return mHAlign; }

        /** 设置y方向对齐
        @version NIIEngine 3.0.0
        */
        inline void setVAlign(VAlign align)         { mVAlign = align; }

        /** 获取y方向对齐
        @version NIIEngine 3.0.0
        */
        inline VAlign getVAlign() const             { return mVAlign; }
        
        /** 添加预赋值属性
        @version NIIEngine 3.0.0
        */
        void add(const AssignProperty & property);

        /** 获取预赋值属性
        @version NIIEngine 3.0.0
        */
        const AssignProperty * getAssignProperty(PropertyID pid) const;
        
        /** 移去预赋值属性
        @version NIIEngine 3.0.0
        */
        void removeAssignProperty(PropertyID pid);

        /** 清理所有预赋值属性
        @version NIIEngine 3.0.0
        */
        void removeAllAssignProperty();

        /** 获取预赋值属性列表
        @version NIIEngine 3.0.0
        */
        const PropertyList & getPropertyList() const{ return mPropertyList; }
        
        /** 设置是否自动窗体
        @version NIIEngine 3.0.0
        */
        inline void setAutoWindow(bool b)           { mAutoGen = b; }

        /** 获取是否自动窗体
        @version NIIEngine 3.0.0
        */
        inline bool isAutoWindow() const            { return mAutoGen; }

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * window, const Font * font) const { return false; }

        /** 写入到 XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        StyleWidget();
    private:
        WidgetID mID;
        StyleViewID mViewID;
        VAlign mVAlign;
        HAlign mHAlign;
        StyleArea mArea;
        PropertyList mPropertyList;
        bool mAutoGen;
    };
}
}
#endif