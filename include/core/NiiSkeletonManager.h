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

#ifndef _NII_SKELETON_MANAGER_H_
#define _NII_SKELETON_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiSkeletonFusion.h"

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
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, 
            ResResultScheme * rs, const PropertyData * params);
    };
}
#endif