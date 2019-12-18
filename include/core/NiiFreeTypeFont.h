/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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
#ifndef _NII_FreeTypeFont_H_
#define _NII_FreeTypeFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace NII
{
    /** ttf 字体
    @version NIIEngine 3.0.0
    */
    class FreeTypeFont : public Font
    {
    public:
        FreeTypeFont(FontID fid, NIIf size, NCount pdi, bool antialias,
            const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, ScaleMode sm = SM_Unknow,
			const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), NIIf lineSpace = 0.0f, 
				LangID lid = N_PrimaryLang);

        ~FreeTypeFont();

        /** 设置分辨率
        @version NIIEngine 3.0.0
        */
        void setDPI(NCount c);

        /** 获取分辨率
        @version NIIEngine 3.0.0
        */
        NCount getDPI() const;

        /** 设置点大小
        @version NIIEngine 3.0.0
        */
        void setPointSize(NIIf size);

        /** 获取点大小
        @version NIIEngine 3.0.0
        */
        NIIf getPointSize() const;

        /** 设置是否抗锯齿
        @version NIIEngine 3.0.0
        */
        void setAntialias(bool b);

        /** 获取是否抗锯齿
        @version NIIEngine 3.0.0
        */
        bool isAntialias() const;
        
        /** 设置行距离
        @version NIIEngine 3.0.0
        */
		void setLineSpace(NIIf space);

        /** 获取行距离
        @version NIIEngine 3.0.0
        */
        NIIf getLineSpace() const;
        
        /** 设置此抗锯齿字体的颜色.
        @version NIIEngine 3.0.0
        */
        void setAlphaAsColour(bool b);

        /** 获取此抗锯齿字体的颜色.
        @version NIIEngine 3.0.0
        */
        bool isAlphaAsColour() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Font::rasterCodePointImpl
        void rasterCodePointImpl(CodePoint start, CodePoint end);

        /// @copydetails Font::reflushCodePointImpl
        void reflushCodePointImpl();

        /// @copydetails Font::getCodePointWidth
        NIIf getCodePointWidth(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointHeight
        NIIf getCodePointHeight(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointExtentImpl
        NIIf getCodePointExtentImpl(const FontGlyph * value) const;

        /** 生成字体
        @param[in] 纹理缓存
        @param[in] w 主纹理的宽
        @param[in] h 主文理的高
        @version NIIEngine 3.0.0 内部api
        */
        void genCodePointGlyph(Nui16 * out, NCount w, NCount h) const;

        /** 删除生成字体
        @version NIIEngine 3.0.0 内部api
        */
        void destroyCodePointGlyph();

        /** 获取纹理大小
        @param out 需要的纹理大小
        @param start 开始
        @param end 结束
        @return 实际加载字符数量
        @version NIIEngine 3.0.0 内部api
        */
        NCount getTextureSize(Vector2i & out, CodePoint start, CodePoint end) const;
    protected:
        /// @copydetails Resource::planImpl
        void planImpl();

        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
    protected:
        NCount mDIP;
        NIIf mPointSize;
        NIIf mLineSpace;
        FT_Face mFontFace;
        DataStream * mFontStream;
        bool mAntialias;
        bool mAlphaAsColour;
    };
}
#endif