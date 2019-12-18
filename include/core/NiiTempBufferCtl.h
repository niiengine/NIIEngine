/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-10

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
#ifndef _NII_TEMP_BUFFER_CTL_H_
#define _NII_TEMP_BUFFER_CTL_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��ʱ����
    @remark һ�����ڷ�CPU�洢����ڴ�,��CPU�洢�ڴ���/�ͷ�ռ�õ�ϵͳʱ���
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI TempBufferCtl
    {
    public:
        /** ����Ƶ������
        @version NIIEngine 3.0.0 �߼�api
        */
        enum OpType
        {
            T_ReadLess,     ///< ��������ȡ
            T_ReadMore,     ///< ������ȡ
            T_WriteLess,    ///< ������д��
            T_WriteMore     ///< ����д��
        };
    public:
        TempBufferCtl(bool autoDsy = true);
        virtual ~TempBufferCtl();

        /** �����Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b);

        /** ��ȡ�Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

        /** �����Ƶ������
        @note ���ڵ�������洢�ṹ
        @param[in] mark OpType�Ķ���򵥸�ö��
        */
        virtual void task(Nmark mark);

        /** ִ�й���
        @version NIIEngine 3.0.0
        */
        virtual void expire(Buffer * src);
    protected:
        bool mAutoDestroy;
    };
}

#endif