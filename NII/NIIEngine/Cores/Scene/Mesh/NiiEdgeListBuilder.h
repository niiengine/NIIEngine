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

#ifndef _NII_EDGET_LIST_BUILDER_H_
#define _NII_EDGET_LIST_BUILDER_H_

#include "NiiPreProcess.h"
#include "NiiGeoEdget.h"

namespace NII
{
    /** �����Ե����
    @remark
        �������Ӷ�綥����������ݼ�ȥ������Ե���,�����ڸ��ּ����Ѿ����ڵļ��н���
        ��Ե; �⽫������ʹ��һ���ɶ������(ʹ�ö��������Ϳ�ѡ�Ķ�������)��Ȼ��ͬ����
        ������Ϣ��ɵ�ģ��.һ���ص��Ǳ�Ե����������һ����һ���㻺����Լ��(����Ⱦ��Ե
        ʱ,���Ǳ����)
    */
    class _EngineAPI EdgeListBuilder
    {
    public:
        /** һ������ʵ����������ģ������Դ�����������,��Ϊ�ض��������ϵȽ��ظ�.
        Ϊ����ȷ������������,һ����ͬ�Ķ�����������Щ�ظ�,�����汣����Щ�ظ�
        �������ϸ���.
        @version NIIEngine 3.0.0
        */
        struct CommonVertex
        {
            NCount index;
            Vector3f position;
            NCount vertexSet;
            NCount indexSet;
            NCount originalIndex;
        };

        /// �����������ݼ�
        struct Geometry
        {
            NCount indexSet;                    ///< ��������������õ��������ݼ�
            NCount vertexSet;                   ///< ��������������õĶ������ݼ�
            const IndexData * mIndex;           ///< ������Ϣ,����������
            GeometryRaw::OperationType opType;  ///< ������Ⱦ������εĲ�������
        };
    public:
        EdgeListBuilder();
        ~EdgeListBuilder();

        /** ���һϵ�ж��㼸�����ݵ���Ե������.
        @remark �ڵ��ý�������ǰ,�������Ӳ�����һ���������ݼ�����������
        @version NIIEngine 3.0.0
        */
        void addVertex(const VertexData * vd);

        /** ��һϵ�������������ݵ���Ե������.
        @remark �ڵ��ý�������ǰ,�������Ӳ�����һ���������ݼ�����������
        @param[in] index ������Ϣ,���������ε�
        @param[in] vdidx ��������������õĶ�������,����Ҫ�ı���,���������˶�����㼯
        @param[in] opType ������Ⱦ��Щ�����Ĳ�������. ������������֧��(�ǵ����֮���)
        @version NIIEngine 3.0.0
        */
        void addIndex(const IndexData * index, NCount vdidx = 0,
            GeometryRaw::OperationType type = GeometryRaw::OT_TRIANGLE_LIST);

        /** ����ĿǰΪֹ��������Ϣ,���ɵı�Ե��Ϣ.
        @note ���ض�����Ҫ�ֶ�ɾ��
        @version NIIEngine 3.0.0
        */
        GeoEdget * build();

        /** ���Է���
        @versioin NIIEngine 3.0.0
        */
        void log(Log * l);
    protected:
        void buildTrianglesEdges(const Geometry & geometry);

        /** �ҵ�һ�����ڵĹ�ͬ����,���Ų���һ���µ�
        @version NIIEngine 3.0.0
        */
        NCount CreateVertex(const Vector3f & vec, NCount vdidx, NCount indexSet, NCount originalIndex);

        /** ����һ�����ڵı�Ե�򴴽�һ���±�Ե - �ڽ���ʱʵ�õķ���
        @version NIIEngine 3.0.0
        */
        void CreateEdge(NCount vdidx, NCount triangleIndex, NCount vertIndex0,
            NCount vertIndex1, NCount common0, NCount common1);
    protected:
        struct vectorLess
        {
            bool operator()(const Vector3f & a, const Vector3f & b) const;
        };
        typedef vector<Geometry>::type GeometryList;
        typedef vector<CommonVertex>::type CommonVertexList;
        typedef vector<const VertexData *>::type VertexDataList;
        typedef map<Vector3f, NCount, vectorLess>::type CommonVertexMap;
        typedef multimap<std::pair<NCount, NCount>, std::pair<NCount, NCount> >::type EdgeMap;

        GeometryList mGeometryList;
        VertexDataList mVertexDataList;
        CommonVertexList mVertices;
        GeoEdget * mEdgeData;
        CommonVertexMap mCommonVertexMap;
        EdgeMap mEdgeMap;
    };
}

#endif