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
        bool initCmd(PropertyCmdSet * dest);

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