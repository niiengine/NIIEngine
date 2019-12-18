/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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

#ifndef _NII_NET_UNIQUE_ADDRESS_H_
#define _NII_NET_UNIQUE_ADDRESS_H_

#include "NiiNetPreInclude.h"
#include "NiiNetUniqueID.h"

namespace NII
{
namespace NII_NET
{
    class _EngineAPI UniqueAddress : public NetAlloc
    {
    public:
        UniqueAddress();

        UniqueAddress(Message * msg);

        UniqueAddress(const UniqueID & o);

        UniqueAddress(const Address * o);

        UniqueAddress(const UniqueAddress & o);

        ~UniqueAddress();
        
        UniqueAddress & operator = (const UniqueID & o);

        UniqueAddress & operator = (const Address & o);

        UniqueAddress & operator = (const UniqueAddress & o);

        bool operator == (const UniqueAddress & o) const;

		/**
		*/
		void write(String & out, bool port = true) const;

		/**
		*/
		void read(const String & in, bool port = true);

		/**
		*/
		bool isValid() const;

		/**
		*/
        static Nul toNum(const UniqueAddress & obj);
	public:
        UniqueID mID;
        Address * mAddress;
    };
}
}
#endif