/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-5-8

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
#ifndef _NII_PIXELORBIT_FACTORY_H_
#define _NII_PIXELORBIT_FACTORY_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** PixelOrbitר�ù�����
	@version NIIEngine 3.0.0
    */
    class _EngineAPI PixelOrbitFactory : public SpaceObjFactory
    {
    public:
        PixelOrbitFactory();
        ~PixelOrbitFactory();

        ///@copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        ///@copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        ///@copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}

#endif