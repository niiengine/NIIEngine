/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-22

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_MATERIAL_LOD_SCHEME_H_
#define _NII_MATERIAL_LOD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"

namespace NII
{
    /** ����LOD
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MaterialLodScheme : public LodScheme
    {
    public:
        typedef vector<std::pair<NIIf, Nui32> >::type LodList;
    public:
        MaterialLodScheme(SchemeID sid);
        virtual ~MaterialLodScheme();

        /// @copydetails LodScheme::getLodList
        void getLodList(SrcList & list) const;

        /// @copydetails LodScheme::add
        void add(NIIf src);

        /// @copydetails LodScheme::removeAll
        void removeAll();
        
        /// @copydetails LodScheme::map
        void map();

        /// @copydetails LodScheme::setValid
        void setValid(bool s) {}

        /// @copydetails LodScheme::isValid
        bool isValid() const { return true; }

        /** ��ȡ�б�
        @version NIIEngine 3.0.0
        */
        const LodList & getList() const;
    protected:
        LodList mLods;
    };

    /** ������ͼLOD����
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialViewLodScheme : public MaterialLodScheme
    {
    public:
        MaterialViewLodScheme(SchemeID sid);

        /// @copydetails LodScheme::getOrigin
        virtual NIIf getOrigin() const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    };

    /** ������������LOD����
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialCameraLodScheme : public MaterialLodScheme
    {
    public:
        MaterialCameraLodScheme(SchemeID sid);

        /** ���öԱ�����
        @param[in] w �ӿڵĿ��
        @param[in] h �ӿڵĸ߶�
        @version NIIEngine 3.0.0
        */
        void setOriArea(NIIf w, NIIf h, Radian fovY);

        /** �����Ƿ�Ա�����
        @version NIIEngine 3.0.0
        */
        void setRefArea(bool b);

        /** ��ȡ�Ƿ�Ա�����
        @version NIIEngine 3.0.0
        */
        bool isRefArea() const;

        /// @copydetails LodScheme::getDest
        Nui32 getDest(NIIf src) const;

        /// @copydetails LodScheme::getBias
        NIIf getBias(NIIf f) const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    private:
        bool mRelation;
        NIIf mRefArea;
    };
}
#endif