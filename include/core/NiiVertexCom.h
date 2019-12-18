/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-24

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

#ifndef _NII_VERTEX_ELEMENT_H_
#define _NII_VERTEX_ELEMENT_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"
#include "NiiDataType.h"

namespace NII
{
    /** 顶点元素的语义,用于识别顶点缓存的内容意义
    @version NIIEngine 3.0.0
    */
    enum VertexType
    {
        VT_Pos = 1,             ///< 位置,3个NIIf
        VT_Matrix_Weight = 2,   ///< 混合权重
        VT_Matrix_Index = 3,    ///< 混合索引
        VT_Normals = 4,         ///< 法线,3个NIIf
        VT_Diffuse = 5,         ///< 漫反射颜色
        VT_Specular = 6,        ///< 镜面反射颜色
        VT_Tex_Coord = 7,       ///< 纹理坐标
        VT_Space = 8,           ///< 顶点辅助空间
        VT_Binormal = 9,        ///< 副法线(Y轴,当法线是Z时)
        VT_Tangent = 10,        ///< 切线 (X轴,当法线是Z时)
        VT_Count = 11
    };

    /**
    @remark
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexCom : public VertexDataAlloc
    {
        friend class VertexData;
    public:
        VertexCom();

        VertexCom(DataType dt, VertexType vt, Nui16 bidx, NCount oft);

        VertexCom(DataType dt, VertexType vt, Nui16 bidx, NCount oft, Nui16 eidx);

        bool operator== (const VertexCom & o) const;

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const void * in, Nui8 *& out) const
        {
            out = (Nui8 *)in + mOffset;
        }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, void *& out) const
        {
            out = (void *)(in + mOffset);
        }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui8 *& out) const
        {
            out = (Nui8 *)(in + mOffset);
        }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui16 *& out) const
        {
            out = (Nui16 *)(in + mOffset);
        }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui32 *& out) const
        {
            out = (Nui32 *)(in + mOffset);
        }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, NIIf *& out) const
        {
            out = (NIIf *)(in + mOffset);
        }

        /** 获取这个元素的字节大小
        @version NIIEngine 3.0.0
        */
        NCount getSize() const;

        /** 转换顶点数据颜色类型
        @param[in] src DT_Colour DT_Colour_ABGR 或 DT_Colour_ARGB.
        @param[in] dst DT_Colour_ABGR 或 DT_Colour_ARGB.
        */
        static void conv(VertexData * obj, DataType src, DataType dest);
    public:
        DataType mType;         ///< 元素类型
        VertexType mVType;      ///< 元素的含义
        NCount mOffset;         ///< 数据偏移
        Nui16 mSource;          ///< 所在的缓存
        Nui16 mIndex;           ///< 项的索引,仅适于复合元素,类纹理坐标元素
    };
}
#endif