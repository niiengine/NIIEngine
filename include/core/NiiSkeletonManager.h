/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-8

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

#ifndef _NII_SKELETON_MANAGER_H_
#define _NII_SKELETON_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiBone.h"

namespace NII
{
    /** 骨骼管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SkeletonManager: public ResourceManager, public Singleton<SkeletonManager>
    {
    public:
        SkeletonManager();
        ~SkeletonManager();

        /** 优化骨骼顶点
        @param[in] svl 骨骼顶点分配列表
        @param[in] count 顶点数量
        @return 每顶点被多少个骨头影响
        @version NIIEngine 3.0.0
        */
        static NCount opt(SkeletonVertexList & svl, NCount count);

        /** 混合骨骼数据到顶点数据中
        @param[out] dest 顶点数据
        @param[in] svl 骨骼顶点分配列表
        @param[out] mat 实际的距阵映射
        @version NIIEngine 3.0.0
        */
        static void combine(VertexData * dest, const SkeletonVertexList & svl,
            BoneIndexList & matmap);

        /// @copydetails Singleton::getOnly
        static SkeletonManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static SkeletonManager * getOnlyPtr();
    protected:
		/// @copydetails ResourceManager::init
		void init();

        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResourceLoadScheme * ls, 
			ResourceResultScheme * rs, const PropertyData * params);
    };
}
#endif