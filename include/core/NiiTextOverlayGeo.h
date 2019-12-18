/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-8

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_TextOverlayGeo_H_
#define _NII_TextOverlayGeo_H_

#include "NiiPreInclude.h"
#include "NiiOverlayGeo.h"
#include "NiiFont.h"

namespace NII
{
    class TextCharGeo;

    /** 文本覆盖层几何
    @note 简易文本显示
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextOverlayGeo : public OverlayGeo
    {
    public:
        /** 对齐方式
        @version NIIEngine 3.0.0
        */
        enum AlignMode
        {
            AM_Left,
            AM_Right,
            AM_Center
        };
    public:
        TextOverlayGeo(OverlayGeoID id, LangID lid = N_PrimaryLang);
        virtual ~TextOverlayGeo();

        /** 设置字体的名字
        @version NIIEngine 3.0.0
        */
        void setFontID(FontID id);

        /** 获取字体的名字
        @version NIIEngine 3.0.0
        */
        FontID getFontID() const;

        /** 设置文本颜色
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & col);

        /** 获取文本颜色
        @version NIIEngine 3.0.0
        */
        const Colour & getColour() const;

        /** 设置对齐模式
        @version NIIEngine 3.0.0
        */
        void setAlignMode(AlignMode a);

        /** 获取对齐模式
        @version NIIEngine 3.0.0
        */
        AlignMode getAlignMode() const;

        /** 设置字符宽度
        @version NIIEngine 3.0.0
        */
        void setGlyphHeight(NIIf height);

        /** 获取字符宽度
        @version NIIEngine 3.0.0
        */
        NIIf getGlyphHeight() const;

        /** 设置空格宽度
        @version NIIEngine 3.0.0
        */
        void setSpaceWidth(NIIf width);

        /** 获取空格宽度
        @version NIIEngine 3.0.0
        */
        NIIf getSpaceWidth() const;

        /// @copydetails OverlayGeo::init
        void init();

        /// @copydetails OverlayGeo::_update
        void _update();

        /// @copydetails OverlayGeo::queue
        void queue(RenderQueue * queue);

        /// @copydetails OverlayGeo::setCaption
        virtual void setCaption(const ViewString & text);

        /// @copydetails OverlayGeo::getTypeID
        virtual OverlayGeoType getTypeID() const;

        /// @copydetails OverlayGeo::setSizeMode
        void setSizeMode(SizeMode sm);

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;
    protected:
        /// copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 更新文本实现
        @version NIIEngine 3.0.0 内部api
        */
        void updateText(NCount count);

        /** 释放字符串
        @version NIIEngine 3.0.0 内部api
        */
        void freeChar();

        /// @copydetails OverlayGeo::updateGeoPos
        virtual void updateGeoPos();

        /// @copydetails OverlayGeo::updateGeoTex
        virtual void updateGeoTex();

        /** 更新顶点颜色
        @version NIIEngine 3.0.0
        */
        virtual void updateColours();
    protected:
        typedef vector<TextCharGeo *>::type CharGeoList;
    protected:
        Font * mFont;
        Colour mColour;
        GeometryRaw mGeo;
        CharGeoList mCharGeoList;
        AlignMode mAlignment;
        VertexData * mVertexData;
        NIIf mCharHeight;
        NIIf mSpaceWidth;
        NCount mCharCount;
        Nui16 mPixelSpaceWidth;
        Nui16 mPixelCharHeight;
    };
}
#endif