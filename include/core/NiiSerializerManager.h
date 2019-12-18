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

#ifndef _NII_SERIALIZER_MANAGER_H_
#define _NII_SERIALIZER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSerializerFactoryObj.h"

namespace NII
{
    /** �������н�����
	@version NIIEngine 3.0.0
    */
    class _EngineAPI SerializerManager : public Singleton<SerializerManager>,
        public SerializerAlloc
    {
    public:
        SerializerManager();
        ~SerializerManager();

        /** ����ļ����н�����
        @remark ������ͷŵ�ʱ�򲻻�ɾ��ʵ��������,�����ǰ�ɾ����¼
        @param[in] serial
		@version NIIEngine 3.0.0
        */
        void add(SerializerFactoryObj * serial);

        /** ��ȥ�ļ����н�����
        @remark ������ͷŵ�ʱ�򲻻�ɾ��ʵ��������,�����ǰ�ɾ����¼
        @param[in] serial
		@version NIIEngine 3.0.0
        */
        void remove(SerializerFactoryObj * serial);

        /** ��ȡ�ļ����н�����
        @param[in] id �ڲ���ı�ʶID,�������е��඼�б�ʶID��
		@version NIIEngine 3.0.0
        */
        SerializerFactoryObj * getFactory(Nui32 id);

        /** ��ȥ���е��ļ����н�����
        @version NIIEngine 3.0.0
		*/
		void removeAll();

        /// @copydetails Singleton::getOnly
        static SerializerManager & getOnly();
        
        /// @copydetails Singleton::getOnlyPtr
        static SerializerManager * getOnlyPtr();
    private:
        typedef map<FactoryID, SerializerFactoryObj *>::type SerializerList;
        SerializerList mSerializers;
    };
}

#endif