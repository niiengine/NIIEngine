/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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

#ifndef _NII_SpaceIDManager_h_
#define _NII_SpaceIDManager_h_

#include "NiiPreInclude.h"
#include "NiiIDManager.h"

namespace NII
{
    /** ���ƶ������
    @remark 
	@version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceIDManager : public IDManager
    {
    public:
        SpaceIDManager(const String & autoname = N_StrBlank);
        virtual ~SpaceIDManager();

        /** ͨ��ָ�����ʹ���һ���ռ����
        @param[in] classid
		@version NIIEngine 3.0.0
        */
        virtual void * create(Nid classid);
    protected:
        /// @copydetails IDManager::create
        virtual void * create(const String & name);

        /// @copydetails IDManager::create
        virtual void * create();

        /// @copydetails IDManager::destroy
        virtual void destroy(void * target);

        /// @copydetails IDManager::reset
        virtual void reset(void * target, Nid id);
    };
}
#endif