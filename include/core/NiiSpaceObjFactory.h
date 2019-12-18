/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-10-28

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

#ifndef _NII_SPACEOBJ_FACTORY_H_
#define _NII_SPACEOBJ_FACTORY_H_

#include "NiiPreInclude.h"
#include "NiiFactoryObj.h"
#include "NiiSpaceObj.h"

namespace NII
{
    /** �ռ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObjFactory : public FactoryObj
    {
    public:
        SpaceObjFactory() : mTypeMark(0xFFFFFFFF) {}
        virtual ~SpaceObjFactory() {}

        /** ����ʵ��
        @param[in] sid
        @param[in] mag
        @param[in] params
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0) = 0;

        /** ���ù��������������
        @version NIIEngine 3.0.0
        */
        void setTypeMark(Nmark mark) { mTypeMark = mark; }

        /** ��ȡ���������������
        @version NIIEngine 3.0.0
        */
        Nmark getTypeMark() const { return mTypeMark; }
    protected:
        Nmark mTypeMark;
    };
}

#endif