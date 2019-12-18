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

#ifndef _NII_MeshManager_H_
#define _NII_MeshManager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiResourceLoadScheme.h"
#include "NiiGeometryRaw.h"
#include "NiiMesh.h"

namespace NII
{
    struct MeshPatchDefine;

    /** 网格资源管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MeshManager : public ResourceManager, public Singleton<MeshManager>,
        public ResourceLoadScheme
    {
        friend class Engine;
    public:
        /** 面向类型
        @version NIIEngine 3.0.0
        */
        enum SideType
        {
            ST_Front,
            ST_Back,
            ST_Both
        };
    public:
        MeshManager();
        ~MeshManager();

        /** 尝试获取/创建网格
        @version NIIEngine 3.0.0
        */
        TouchResult touch(ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0,
            const PropertyData * params = 0, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 计划资源加载网格
        @version NIIEngine 3.0.0
        */
        Mesh * plan(ResourceID rid, GroupID gid, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 加载资源网格
        @version NIIEngine 3.0.0
        */
        Mesh * load(ResourceID rid, GroupID gid, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 创建自定义网格
        @version NIIEngine 3.0.0
        */
        Mesh * createManual(ResourceID rid, GroupID gid, ResourceLoadScheme * ls = 0,
            ResourceResultScheme * rs = 0);

        /** 创建贝塞尔面
        @param width 宽点数
        @param height 高点数
        @param vd 控制点数据
        @param uMaxSubLevel u方向细等级
        @param vMaxSubLevel v方向细等级
        */
        Mesh * createBezierPatch(ResourceID rid, GroupID gid, NCount width, NCount height,
            VertexData * vd, NCount uMaxSubLevel = -1, NCount vMaxSubLevel = -1, SideType type = ST_Front,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_WRITE | Buffer::M_CPU);

        /** 创建一个基本面,默认主要在 x/y 轴 面向位置 Z
        @param plane 面的基本定义
        @param width 面宽度
        @param height 面长度
        @param xdetails x方向片数
        @param ydetails y方向片数
        @param utile 纹理u方向重复次数
        @param vtile 纹理v方向重复次数
        @param up 面向上的方向
        */
        Mesh * createPlane(ResourceID rid, GroupID gid, const Plane & plane,
            NIIf width, NIIf height, NIIi xdetails = 1, NIIi ydetails = 1,
            bool normals = true, NCount texcoordCount = 1, NIIf utile = 1.0f, NIIf vtile = 1.0f,
            const Vector3f & up = Vector3f::Y,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 创建纹理坐标弯曲的面
        @param plane 面的基本定义
        @param width 面宽度
        @param height 面长度
        @param curvature 曲率
        @param xdetails x方向片数
        @param ydetails y方向片数
        @param ykeep y方向实际片数
        @param utile 纹理u方向重复次数
        @param vtile 纹理v方向重复次数
        @param up 面向上的方向
        @param orientation 面的曲向
        */
        Mesh * createCurvedIllusionPlane(ResourceID rid, GroupID gid, const Plane & plane,
            NIIf width, NIIf height, NIIf curvature, NIIi xdetails = 1, NIIi ydetails = 1,
            NIIi ykeep = 0, bool normals = true, NCount texcoordCount = 1,
            NIIf utile = 1.0f, NIIf vtile = 1.0f, const Vector3f & up = Vector3f::Y,
            const Quaternion & orientation = Quaternion::IDENTITY,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 创建弯曲面
        @param plane 面的基本定义
        @param width 面宽度
        @param height 面长度
        @param curvature 曲率
        @param xdetails x方向片数
        @param ydetails y方向片数
        @param utile 纹理u方向重复次数
        @param vtile 纹理v方向重复次数
        @param up 面向上的方向
        */
        Mesh * createCurvedPlane(ResourceID rid, GroupID gid, const Plane & plane,
            NIIf width, NIIf height, NIIf curvature = 0.5f, NIIi xdetails = 1, NIIi ydetails = 1,
            bool normals = false, NCount texcoordCount = 1, NIIf xtile = 1.0f, NIIf ytile = 1.0f,
            const Vector3f & up = Vector3f::Y,
            Nmark vbmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_CPU | Buffer::Buffer::M_GPU | Buffer::M_WRITE);

        /** 构建预定几何
        @version NIIEngine 3.0.0
        */
        bool buildPrefab(Mesh * mesh, GeometryPrefab type);

        /// @copydetails Singleton::getOnly
        static MeshManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static MeshManager * getOnlyPtr();

        /// @copydetails ResourceLoadScheme::load
        void load(Resource * res);
    protected:
        /// @copydetails ResourceManager::init
        void init();

        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls, ResourceResultScheme * rs,
                const PropertyData * params);
    protected:
        typedef map<Resource *, MeshPatchDefine *>::type PatchDefineList;
    protected:
        PatchDefineList mPatchDefineList;
    };
}
#endif