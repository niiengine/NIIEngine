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

#ifndef _NII_MD5_H_
#define _NII_MD5_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    struct MD5Context 
    {
        Nui32 buf[4];
        Nui32 bytes[2];
        Nui32 in[16];
    };

    class Md5
    {
    public:
        Md5(void);
        ~Md5(void);
        void append(const Nui8 * inBuf, size_t inLen);
        void append(const String & str);
        void getDigest(Nui8 digest[16]);
        void getDigest(String & digest);
    
        /*! \brief Get the MD5 digest of the given text
        \param text text to compute the MD5 for
        \return MD5 digest
        */
        static String GetMD5(const String &text);
	private:
        MD5Context m_ctx;
    };
}

#endif
