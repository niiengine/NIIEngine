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
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_StringSerialiser_H_
#define _NII_StringSerialiser_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
	/** �ı���
	@version NIIEngine 3.0.0
	*/
	class StringSerialiser
	{
	public:
		StringSerialiser(NCount size = 0);
		~StringSerialiser();

		String getStr() const;

		StringSerialiser & operator << (const Ntchar * str);
		StringSerialiser & operator << (const String & str);
		StringSerialiser & operator << (Ni8 val);
		StringSerialiser & operator << (Ni16 val);
		StringSerialiser & operator << (Ni32 val);
		StringSerialiser & operator << (Nui8 val);
		StringSerialiser & operator << (Nui16 val);
		StringSerialiser & operator << (Nui32 val);
		StringSerialiser & operator << (NIIf val);
		StringSerialiser & operator << (NIId val);
	private:
		void enlarge(NCount n);
	private:
		Ntchar * mBuffer;
		NCount mBufferOft;
		NCount mTotalSize;
	};
}
#endif