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

#ifndef _NII_FontGlyph_H_
#define _NII_FontGlyph_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 字体字型
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontGlyph : public UIAlloc
    {
    public:
        FontGlyph(){}
        FontGlyph(const Rectf & coord, const Vector2f & oft, NIIf extent) :
            mTexCoord(coord),
            mPixelOffset(oft),
            mHoriAdvance(extent){}

        /** 设置纹理坐标
        @version NIIEngine 3.0.0 高级 api
        */
        void setTextureCoord(const Rectf & coord)
        {
            mTexCoord = coord;
        }

        /** 获取纹理坐标
        @version NIIEngine 3.0.0 高级 api
        */
        const Rectf & getTextureCoord() const
        {
            return mTexCoord;
        }

        /** 设置像素显示位置偏移
        @version NIIEngine 3.0.0 高级 api
        */
        void setOffset(const Vector2f & oft)
        {
            mPixelOffset = oft;
        }

        /** 获取像素显示位置偏移
        @version NIIEngine 3.0.0 高级 api
        */
        const Vector2f & getOffset() const
        {
            return mPixelOffset;
        }

        /** 设置水平偏移
        @version NIIEngine 3.0.0 高级 api
        */
        void setHoriAdvance(NIIf adv)
        {
            mHoriAdvance = adv;
        }

        /** 获取水平偏移
        @version NIIEngine 3.0.0 高级 api
        */
        NIIf getHoriAdvance() const
        {
            return mHoriAdvance;
        }
    protected:
        Rectf mTexCoord;
        Vector2f mPixelOffset;
        NIIf mHoriAdvance;
    };
    typedef vector<FontGlyph *>::type FontGlyphList;
}
#endif