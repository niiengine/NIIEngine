/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

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

#ifndef _NII_ShadowVolumeOBJ_H_
#define _NII_ShadowVolumeOBJ_H_

#include "NiiPreInclude.h"
#include "NiiShadowVolume.h"
#include "NiiSpaceObj.h"

namespace NII
{
    /** 阴影容积生成,
    @remark 模版级的阴影技术才需要
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowVolumeObj : public SpaceObj
    {
    public:
        /** 创建特性
        @version NIIEngine 3.0.0
        */
        enum VolumeType
        {
            VT_Infinity  = 0x01,    ///< 无限挤压
            VT_FrontCap  = 0x02,    ///< 亮罩
            VT_BackCap   = 0x04,    ///< 暗罩
            VT_Prefab    = 0x08,    ///< 预定制
            VT_Software  = 0x10     ///< 软件处理
        };
    public:
        ShadowVolumeObj();
        ShadowVolumeObj(SpaceID sid);
        virtual ~ShadowVolumeObj();

        /** 获取边缘列表
        @remark 确定侧面影像
        @version NIIEngine 3.0.0
        */
        virtual GeoEdget * getEdgeList();

        /** 边缘列表是否存在
        @version NIIEngine 3.0.0
        */
        virtual bool isEdgeExist();

        /** 获取灯亮罩的世界空间边界盒
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getFrontCapAABB() const = 0;

        /** 获取灯暗罩的世界空间边界盒
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getBackCapAABB(const Light & light, NIIf extent) const;

        /** 获取阴影体积列表
        @param[in] light 用于生成阴影的灯光
        @param[in] extent 挤压阴影体积的距离
        @param[in] mark VolumeType 的多个组合
        */
        virtual const ShadowVolumeList & getVolumeList(const Light * light, NIIf extent,
            Nmark mark, IndexBuffer * index);

        /** 获取挤压距离
        @version NIIEngine 3.0.0
        */
        virtual NIIf getExtrudeDistance(const Light * l) const = 0;

        /** 积压顶点
        @version NIIEngine 3.0.0
        */
        static void extrude(VertexBuffer * data, NCount count, const Vector4f & pos, NIIf extent);
    protected:
        /** 生成容积列表
        @param[in] mark VolumeType 的多个组合
        @version NIIEngine 3.0.0
        */
        void generate(const GeoEdget * src, IndexBuffer * out, const Light * light,  
            Nmark mark, const ShadowVolumeList & srcobj);
    protected:
        mutable AABox mDarkCapAABB;
    };
}
#endif