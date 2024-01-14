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
    /** ����ר�ò��ϸ�ڵȼ�����
    @remark ÿ������һ�����ֻ����ͬһ������
    @note ��������
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI MeshLodScheme : public LodScheme
    {
    public:
        /** �߼�api
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

        /** ���LOD��Ϣ
        @version NIIEngine 3.0.0 �߼�api
        */
        void add(const MeshLod & lod);

        /** ���LOD��Ϣ
        @param[in] idxbuf ʹ�õ��������漯,һ������LODʹ��ͬ����������
        @param[in] rid ��Դid
        @param[in] gid ��Դ��
        @param[in] rel ����LODʹ��ͬ��������̬,һ������LODʹ��ͬ����������
        @version NIIEngine 3.0.0 �߼�api
        */
        void add(NIIf src, Nidx idxbuf, ResourceID rid, GroupID gid = GroupUnknow, bool rel = true);

        /** ���LOD��Ϣ
        @param[in] rid ��Դid
        @param[in] gid ��Դ��
        @param[in] idxbuf ʹ�õ��������漯,һ������LODʹ��ͬ����������
        @param[in] rel ����LODʹ��ͬ��������̬,һ������LODʹ��ͬ����������
        @version NIIEngine 3.0.0 �߼�api
        */
        void add(NIIf src, Nui32 dest, Nidx idxbuf, ResourceID rid, GroupID gid = GroupUnknow, bool rel = true);

        /** �ֶ�����LOD��Ϣ
        @remark �ֶ�����
        @param[in] index lod��
        @param[in] rid ��Դid
        @param[in] idxbuf ʹ�õ��������漯,һ������LODʹ��ͬ����������
        @param[in] rel ����LODʹ��ͬ��������̬,һ������LODʹ��ͬ����������
        @note һ���0�㲻������
        @version NIIEngine 3.0.0 �߼�api
        */
        void set(Nidx index, ResourceID rid, Nidx idxbuf = 0, bool rel = true);

        /** �ֶ�����LOD��Ϣ
        @param[in] index lod��
        @note һ���0�㲻������
        @version NIIEngine 3.0.0 �߼�api
        */
        void set(Nidx index, NIIf src);

        using LodScheme::get;

        /** ��ȡLOD��Ϣ
        @param[in] index lod��
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, ResourceID & rid) const;

        /** ��ȡLOD��Ϣ
        @param[in] index lod��
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, Mesh *& mesh) const;

        /** ��ȡLOD��Ϣ
        @param[in] index lod��
        @version NIIEngine 3.0.0
        */
        void get(Nidx index, GeoEdget *& lod) const;

        /** ��ȡLOD��Ϣ
        @version NIIEngine 3.0.0
        */
        const MeshLod & getInfo(Nidx index) const;

        /** �Ƿ�ͬ��LOD
        @param[in] index lod��
        @note һ����ָ��������ԭ����ͬ����,ͬ������LOD
        @version NIIEngine 3.0.0
        */
        bool isRelation(Nidx index) const;

        /** ��ȡͬ��LOD����
        @version NIIEngine 3.0.0
        */
        Nidx getRelationIndex(Nidx index) const;

        /** ��ȡͬ��LOD����
        @note һ����ָ��������ԭ����ͬ����,ͬ������LOD
        @version NIIEngine 3.0.0
        */
        NCount getRelationCount() const;

        /** ����LOD����
        @note ���벻С��1,������һ��
        @version NIIEngine 3.0.0
        */
        void setCount(NCount count);

        /** ��ȡLOD����
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** ����Lod
        @remark ����������Ϣȷ��ʱ�ŵ���
        @version NIIEngine 3.0.0
        */
        void build();

        /** ɾ��Lod
        @remark ��ɾ������������Lod��Ϣ
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

        /** ����lod
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void load(Nidx index);
    protected:
        LodList mLods;
        bool mValidLod;
    };

    /** ������ͼLOD����
    @remark �������������� ResResultScheme һ��ʹ��
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

    /** ���������LOD����
    @remark �������������� ResLoadScheme һ��ʹ��
    @version NIIEngine 3.0.0
    */
    class _EngineInner MeshCameraLodScheme : public MeshLodScheme
    {
    public:
        MeshCameraLodScheme(SchemeID sid);

        /** ���öԱ�����
        @version NIIEngine 3.0.0
        */
        void setOriArea(NIIf w, NIIf h, Radian fovy);

        /** �����Ƿ�Ա�����
        @version NIIEngine 3.0.0
        */
        void setRefArea(bool b);

        /** ��ȡ�Ƿ�Ա�����
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