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

#ifndef _NII_BASE64_H_
#define _NII_BASE64_H_

#include "NiiPreInclude.h"

namespace NII
{
    class Base64
    {
    public:
        static void Encode(const char * in, NCount length, String & out);
        static String Encode(const char * in, NCount length);
        static void Encode(const String & in, String & out);
        static String Encode(const String & in);
        static void Decode(const char * in, NCount length, String & out);
        static String Decode(const char * in, NCount length);
        static void Decode(const String & in, String & out);
        static String Decode(const String & in);
    private:
        static const String mData;
    };
}
#endif