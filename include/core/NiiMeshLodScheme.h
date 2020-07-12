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

#ifndef _NII_MESH_LOD_SCHEME_H_
#define _NII_MESH_LOD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"

namespace NII
{
    /** 网格专用层次细节等级策略
    @remark 每个对象一般情况只用于同一个网格
    @note 被动策略
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI MeshLodScheme : public LodScheme
    {
    public:
        /** 高级api
        */
        class _EngineAPI MeshLod : public LodAlloc
        {
        public:
            MeshLod();
            MeshLod(NIIf src, Nui32 dst, bool rel = true);
            MeshLod(NIIf src, Nui32 dst, ResourceID rid, GroupID gid, Nidx bufidx = 0, bool rel = true);

            MeshLod & operator =(const MeshLod & o);

            NIIf mSrc;
            Nui32 mDest;
            GroupID mGid;
            ResourceID mRid;
            Nidx mIdxBuf;
            Mesh * mMesh;
            bool mRelLod;
        };
        typedef vector<MeshLod>::type LodList;
    public:
        MeshLodScheme(SchemeID sid);
        virtual ~MeshLodScheme();

        /// @copydetails LodScheme::add
        void add(NIIf src);

        /** 添加LOD信息
        @version NIIEngine 3.0.0 高级api
        */
        void add(const MeshLod & lod);

        /** 添加LOD信息
        @param[in] idxbuf 使用的索引缓存集,一般与主LOD使用同个顶点数据
        @param[in] rid 资源id
        @param[in] gid 资源组
        @param[in] rel 与主LOD使用同个动画形态,一般与主LOD使用同个顶点数据
        @version NIIEngine 3.0.0 高级api
        */
        void add(NIIf src, Nidx idxbuf, ResourceID rid, GroupID gid = GroupUnknow, bool rel = true);

        /** 添加LOD信息
        @param[in] rid 资源id
        @param[in] gid 资源组
        @param[in] idxbuf 使用的索引缓存集,一般与主LOD使用同个顶点数据
        @param[in] rel 与主LOD使用同个动画形态,一般与主LOD使用同个顶点数据
        @version NIIEngine 3.0.0 高级api
        */
        void add(NIIf src, Nui32 dest, Nidx idxbuf, ResourceID rid, GroupID gid = GroupUnknow, bool rel = true);

        /** 手动设置LOD信息
        @remark 手动设置
        @param[in] index lod层
        @param[in] rid 资源id
        @param[in] idxbuf 使用的索引缓存集,一般与主LOD使用同个顶点数据
        @param[in] rel 与主LOD使用同个动画形态,一般与主LOD使用同个顶点数据
        @note 一般第0层不做操作
        @version NIIEngine 3.0.0 高级api
        */
        void set(Nidx index, ResourceID rid, Nidx idxbuf = 0, bool rel = true);

        /** 手动设置LOD信息
        @param[in] index lod层
        @note 一般第0层不做操作
        @version NIIEngine 3.0.0 高级api
        */
        void set(Nidx index, NIIf src);

        using LodScheme::get;

        /** 获取LOD信息
        @param[in] index lod层
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, ResourceID & rid) const;

        /** 获取LOD信息
        @param[in] index lod层
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, Mesh *& mesh) const;

        /** 获取LOD信息
        @param[in] index lod层
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, GeoEdget *& lod) const;

        /** 获取LOD信息
        @version NIIEngine 3.0.0
        */
        const MeshLod & getInfo(Nidx index) const;

        /** 是否同型LOD
        @param[in] index lod层
        @note 一般是指适用于与原网格同骨骼,同动画的LOD
        @version NIIEngine 3.0.0
        */
        bool isRelation(Nidx index) const;

        /** 获取同型LOD索引
        @version NIIEngine 3.0.0
        */
        Nidx getRelationIndex(Nidx index) const;

        /** 获取同型LOD数量
        @note 一般是指适用于与原网格同骨骼,同动画的LOD
        @version NIIEngine 3.0.0
        */
        NCount getRelationCount() const;

        /** 设置LOD数量
        @note 必须不小于1,至少有一层
        @version NIIEngine 3.0.0
        */
        void setCount(NCount count);

        /** 获取LOD数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 构建Lod
        @remark 仅当所有信息确立时才调用
        @version NIIEngine 3.0.0
        */
        void build();

        /** 删除Lod
        @remark 仅删除构建出来的Lod信息
        @version NIIEngine 3.0.0
        */
        void destroy();

        /// @copydetails LodScheme::getLodList
        void getLodList(SrcList & list) const;

        /// @copydetails LodScheme::setValid
        void setValid(bool s);

        /// @copydetails LodScheme::isValid
        bool isValid() const;

        /// @copydetails LodScheme::removeAll
        void removeAll();

        /// @copydetails LodScheme::map
        void map();
    protected:
        MeshLodScheme();

        /** 加载lod
        @version NIIEngine 3.0.0 内部api
        */
        void load(Nidx index);
    protected:
        LodList mLods;
        bool mValidLod;
    };

    /** 网格视图LOD策略
    @remark 这个方案可以配合 ResResultScheme 一起使用
    @version NIIEngine 3.0.0
    */
    class _EngineInner MeshViewLodScheme : public MeshLodScheme
    {
    public:
        MeshViewLodScheme(SchemeID sid);

        /// @copydetails LodScheme::getOrigin
        virtual NIIf getOrigin() const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 dest) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        virtual NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    };

    /** 网格摄象机LOD策略
    @remark 这个方案可以配合 ResLoadScheme 一起使用
    @version NIIEngine 3.0.0
    */
    class _EngineInner MeshCameraLodScheme : public MeshLodScheme
    {
    public:
        MeshCameraLodScheme(SchemeID sid);

        /** 设置对比区域
        @version NIIEngine 3.0.0
        */
        void setOriArea(NIIf w, NIIf h, Radian fovy);

        /** 设置是否对比区域
        @version NIIEngine 3.0.0
        */
        void setRefArea(bool b);

        /** 获取是否对比区域
        @version NIIEngine 3.0.0
        */
        bool isRefArea() const;

        /// @copydetails LodScheme::getDest
        virtual Nui32 getDest(NIIf src) const;

        /// @copydetails LodScheme::getBias
        virtual NIIf getBias(NIIf f) const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 dest) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        virtual NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    private:
        bool mRelation;
        NIIf mRefArea;
    };
}
#endif