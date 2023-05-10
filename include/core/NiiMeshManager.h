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
        public ResLoadScheme
    {
        friend class Engine;
    public:
        MeshManager();
        ~MeshManager();

        /** 尝试获取/创建网格
        @version NIIEngine 3.0.0
        */
        TouchResult ref(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0,
            const PropertyData * params = 0, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);

        /** 计划资源加载网格
        @version NIIEngine 3.0.0
        */
        Mesh * plan(ResourceID rid, GroupID gid, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);

        /** 加载资源网格
        @version NIIEngine 3.0.0
        */
        Mesh * load(ResourceID rid, GroupID gid, GeometryPrefab type = GSP_None,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);

        /** 创建自定义网格
        @version NIIEngine 3.0.0
        */
        Mesh * createManual(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        /** 创建贝塞尔面
        @param width 宽点数
        @param height 高点数
        @param vd 控制点数据
        @param uMaxSubLevel u方向细等级
        @param vMaxSubLevel v方向细等级
        */
        Mesh * createBezierPatch(ResourceID rid, GroupID gid, NCount width, NCount height,
            VertexData * vd, NCount uMaxSubLevel = -1, NCount vMaxSubLevel = -1, FaceSideType type = FST_Front,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_WRITE | Buffer::M_HOST);

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
            bool normals = true, Nui16 texcoordCnt = 1, NIIf utile = 1.0f, NIIf vtile = 1.0f,
            const Vector3f & up = Vector3f::Y,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);

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
        Mesh * createCurved(ResourceID rid, GroupID gid, const Plane & plane,
            NIIf width, NIIf height, NIIf curvature = 0.5f, NIIi xdetails = 1, NIIi ydetails = 1,
            bool normals = false, Nui16 texcoordCount = 1, NIIf xtile = 1.0f, NIIf ytile = 1.0f,
            const Vector3f & up = Vector3f::Y,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);
            
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
        Mesh * createCurvedIllusion(ResourceID rid, GroupID gid, const Plane & plane,
            NIIf width, NIIf height, NIIf curvature, NIIi xdetails = 1, NIIi ydetails = 1,
            NIIi ykeep = 0, bool normals = true, Nui16 texcoordCount = 1,
            NIIf utile = 1.0f, NIIf vtile = 1.0f, const Vector3f & up = Vector3f::Y,
            const Quaternion & orientation = Quaternion::IDENTITY,
            Nmark vbmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE,
            Nmark ibmode = Buffer::M_HOST | Buffer::M_DEV | Buffer::M_WRITE);

        /** 构建预定几何
        @version NIIEngine 3.0.0
        */
        bool buildPrefab(Mesh * mesh, GeometryPrefab type);

        /// @copydetails ResLoadScheme::load
        void load(Resource * res);
    protected:
        /// @copydetails ResourceManager::init
        void init();

        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid,
            ResLoadScheme * ls, ResResultScheme * rs,
                const PropertyData * params);
    protected:
        typedef map<Resource *, MeshPatchDefine *>::type PatchDefineList;
    protected:
        PatchDefineList mPatchDefineList;
    };
}
#endif