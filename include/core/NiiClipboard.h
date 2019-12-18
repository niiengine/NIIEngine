/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-5

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

#ifndef _NII_CLIPBOARD_H_
#define _NII_CLIPBOARD_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ClipboardData : public DataAlloc
    {
    public:
        virtual ~ClipboardData();

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual void set(const String & mime, void * data, NCount size) = 0;

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        virtual void get(String & mime, void *& data, NCount & size) = 0;
    };

    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Clipboard : public DataAlloc
    {
    public:
        Clipboard();
        ~Clipboard();

        /** ��������
        @param[in] size �ֽڴ�С
        @version NIIEngine 3.0.0
        */
        void setData(const String & mime, const Nui8 * buffer, NCount size);

        /** ��ȡ����
        @param[out] size �ֽڴ�С
        @version NIIEngine 3.0.0
        */
        void getData(String & mime, const Nui8 *& buffer, NCount & size);

        /** ���ü�������
        @version NIIEngine 3.0.0
        */
        void set(ClipboardData * src);

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        void get(ClipboardData *& src) const;

        /** �����ı�
        @remark ���׷�ʽ text/plain ����
        @version NIIEngine 3.0.0
        */
        void setText(const String & text);

        /** ��ȡ�ı�
        @remark ���׷�ʽ text/plain ����
        @version NIIEngine 3.0.0
        */
        String getText();
    private:
        Nui8 * mBuffer; 
        String mMimeType;
        NCount mCount;
        ClipboardData * mData;
    };
}
#endif