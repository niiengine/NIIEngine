/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-22

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
#ifndef _NII_PIXELBUFFER_H_
#define _NII_PIXELBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSize.h"
#include "NiiRect.h"

namespace NII
{
    /** 像素缓存
    @remark
        像素概念比面/片复杂点.像素的实际渲染效果是依赖缩放方式,实际渲染大小等参数
        确定的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBuffer : public ResourceAlloc
    {
        friend class GeometryPixel;
    public:
        PixelBuffer(PixelBufferID pxid, ResourceID gid);
		PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area, const Vector2f & oft);
        PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area,
            const Vector2f & oft, ScaleMode mode, const PlaneSizef & srcRes);

        ~PixelBuffer();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        PixelBufferID getID() const;

        /** 获取群组
        @version NIIEngine 3.0.0
        */
        ResourceID getGroupID() const;

        /** 设置像素来源
        @param[in] obj 纹理
        @version NIIEngine 3.0.0
        */
        void setTexture(Texture * obj);

        /** 设置像素区域
        @param[in] area 区域定义(像素级)
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** 设置缩放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setMode(ScaleMode mode);

        /** 获取资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & area);

        /** 设置渲染偏移
        @param[in] oft 像素位置偏移(像素级)
        @version NIIEngine 3.0.0
        */
        void setPixelOffset(const Vector2f & oft);

        /** 获取实际尺寸
        @return 实际渲染尺寸(屏幕级)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getSize() const;

        /** 获取实际偏移
        @return 实际渲染偏移(屏幕级)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getOffset() const;

        /** 确定视图的实际大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);

        /** 计算缩放比例
        @param[in] out 缩放比例
        @version NIIEngine 3.0.0
        */
        static void calc(ScaleMode mode, const PlaneSizef & dest, const PlaneSizef & src, Vector2f & out);
    protected:
        PixelBufferID mID;
        Texture * mTexture;     // 合并 统一
        ScaleMode mMode;        // 合并 统一
        Rectf mArea;
        Vector2f mOffset;
        Vector2f mPixelOffset;  // 临时
        PlaneSizef mNativeRes;  // 合并 临时
        PlaneSizef mSize;		// 临时
    };
}
#endif