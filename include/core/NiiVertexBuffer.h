/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-23

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_VERTEXBUFFER_H_
#define _NII_VERTEXBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"

namespace NII
{

    /** GPU AGP / SYS 三角形顶点索引缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexBuffer : public Buffer
    {
    public:
        enum IndexRenderType
        {
            IRT_Normal = 1,         ///< 普通索引
            IRT_LeftView = 2,       ///< 模型左视图(相对概念/切线空间)
            IRT_RightView = 3,      ///< 模型右视图(相对概念/切线空间)
            IRT_TopView = 4,        ///< 模型俯视图(相对概念/切线空间)
            IRT_Plane = 5           ///< 单一平行面
        };
    public:
        IndexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);
        ~IndexBuffer();

        /** 获取这个缓存缓存类型
        @version NIIEngine 3.0.0
        */
        NCount getUnitCount() const;

        /** 获取每个索引的字节大小
        @version NIIEngine 3.0.0
        */
        NCount getUnitSize() const;

        /// @copydetails Buffer::clone()
        Buffer * clone(Nmark m = Buffer::M_WRITE | Buffer::M_BLOCK | Buffer::M_CPU) const;
    protected:
        NCount mUnitCount;
        NCount mUnitSize;
    };

    /** AGP/GPU/Sys 三角形顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);

        ~VertexBuffer();

        /** 获取这个缓存的单元项单位总大小
        @version NIIEngine 3.0.0
        */
        NCount getComSize() const;

        /** 获取这个缓存的顶点数量
        @version NIIEngine 3.0.0
        */
        NCount getComCount() const;

        /** 同一渲染通路中多次应用绘制
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        void setInstancing(bool s);

        /** 同一渲染通路中多次应用绘制
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        bool isInstancing() const;
        
        /** 同一渲染通路中多次应用绘制数据偏移
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        NCount getInstancingOffset() const;

        /** 同一渲染通路中多次应用绘制数据偏移
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */ 
        void setInstancingOffset(NCount val);
        
        /// @copydetails Buffer::clone
        Buffer * clone(Nmark m = Buffer::M_WRITE | Buffer::M_BLOCK | Buffer::M_CPU) const;
    protected:
        NCount mComCount;
        NCount mComSize;
        NCount mInstancingOffset;
        
    };
}
#endif