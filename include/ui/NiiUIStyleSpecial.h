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

#ifndef _NII_UI_StyleSpecial_H_
#define _NII_UI_StyleSpecial_H_

#include "NiiUIPreInclude.h"
#include "NiiSizeDefine.h"

namespace NII
{
namespace UI
{
    /** 描述UI单元或图象成分
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleArea : public UIAlloc
    {
    public:
        StyleArea();

        /** 获取区域的像素区域
        @param w
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w) const;

        /** 获取区域的像素区域
        @param w
        @param container
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w, const Rectf & container) const;

        /**设置
        @version NIIEngine 3.0.0
        */
        void setSrcFetch(StyleID sid, Nid style_special);

        /**
        @version NIIEngine 3.0.0
        */
        void setPropertyFetch(PropertyID pid);

        /**
        @version NIIEngine 3.0.0
        */
        inline PropertyID getPropertyFetch() const      { return mPropertyID; }

        /** 通过属性获取
        @version NIIEngine 3.0.0
        */
        inline bool isPropertyFetch() const             { return mPropertyID != 0 && mStyleID == 0; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSpecialFetch() const              { return mStyleID != 0 && mPropertyID != 0; }

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * widget, const Font * font) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    public:
        SizeDefine mLeft;
        SizeDefine mTop;
        SizeDefine mXDim;
        SizeDefine mYDim;
    private:
        PropertyID mPropertyID;
        StyleID mStyleID;
    };

    /** 风格特定
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSpecial : public UIAlloc
    {
    public:
        StyleSpecial();
        StyleSpecial(Nid id);

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id)                  {  mID = id; }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                   { return mID; }

        /** 设置区域
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area){ mArea = area; }

        /** 获取区域
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const   { return mArea; }

        /** 写入到XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        /** 字体改变时触发
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * obj, const Font * font) const;
    private:
        Nid mID;
        StyleArea mArea;
    };
}
}
#endif