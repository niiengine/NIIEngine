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

#ifndef _NII_ADDRESS_ID_H_
#define _NII_ADDRESS_ID_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
    /** ��Ϸ�����ڲ��������ַ
    @remark ���IDֻ���ڳɹ�������Ϸ�����Ż������,������Ψһ����ı�ʶ
	@version NIIEngine 3.0.0
    */
    class _EngineAPI AddressID : public NetWorkAlloc
    {
    public:
        AddressID();
        AddressID(Nui64 id);

		/** ��������
		@version NIIEngine 3.0.0
		*/
		void setIndex(Nui16 idx);

		/** ��ȡ����
		@version NIIEngine 3.0.0
		*/
		Nui16 getIndex() const;

        /// ��ָ���ַ�����ת��������ID
        bool read(const String & in);

        /// ת������ID��һ��ָ���ַ���
        void write(String & out) const;

        /// ���ڲ�����
        AddressID & operator =(const AddressID & org);

        /// ��Ȳ�����
        bool operator == (const AddressID & org);

        /// ���Ȳ�����
        bool operator != (const AddressID & org);

        /// ���ڲ�����
        bool operator > (const AddressID & org);

        /// С�ڲ�����
        bool operator < (const AddressID & org);

        /// ��λ������
        operator Nui32() const;

        /// ����ID�ṹ��С
        static NCount getSize() const;

        /// ��Ч������ID
        static const AddressID INVALID;
    protected:
        Nui64 mID;                      ///< Ӧ����ϵ�е�ΨһID
        Nui16 mIdex;                    ///< ����ΨһID
    };
}
}