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

#ifndef _NII_PixmapFont_H_
#define _NII_PixmapFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

namespace NII
{
    /** 像素级字体
    @version NIIEngine 3.0.0
    */
    class PixmapFont : public Font
    {
    public:
        PixmapFont(FontID fid, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        PixmapFont(FontID fid, const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), LangID lid = N_PrimaryLang);
        ~PixmapFont();

        /** 定义字符
        @param[in] texidx 纹理索引,对应着所设置的纹理数组
        @param[in] value 字符码值
        @param[in] coord 纹理坐标
        @param[in] oft 像素位置偏移
        @param[in] extent 宽度
        @version NIIEngine 3.0.0
        */
        void defineCodePoint(Nidx texidx, CodePoint value, const Rectf & coord,
            const Vector2f & oft, NIIf extent);

        /** 设置纹理ID
        @note 将会重新设置当前纹理
        @version NIIEngine 3.0.0 高级api
        */
        void setTexture(ResourceID * rid, NCount count);

        /** 获取纹理ID
        @version NIIEngine 3.0.0 高级api
        */
        void getTexture(TextureList & out) const;
    protected:
        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
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
    protected:
        NIIf mLastXScale;
        NIIf mLastYScale;
    };
}
#endif