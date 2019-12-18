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

#ifndef _NII_NET_MESSAGE_H_
#define _NII_NET_MESSAGE_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
	/**
	@note Ӧ�ò����
	*/
    class _EngineAPI Message : public NetAlloc
    {
    public:
        Message(NCount size);
        Message(Nui8 * data, NCount size);
        Message(Nui8 * data, NCount size, bool autoDestroy, bool inner);
        ~Message();
	public:
        UniqueID mID;
        Address * mAddress;

        Nui8 * mData;
        Nui32 mLength;
        NBitCount mBitCount;

        Nmark mMark;

        bool mAutoDestroy;
        bool mLocal;
    };
}
}