/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-5-4

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
    
#ifndef _NII_DriverVersion_H_
#define _NII_DriverVersion_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �����汾
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DriverVersion : public SysAlloc
    {
    public:
        DriverVersion();

        /** �����汾�Ƿ����һ����
        @param[in] o ��һ��
        @version NIIEngine 3.0.0
        */
        bool isOlder(DriverVersion o) const;
        
        /** �����汾�ַ���
        @version NIIEngine 3.0.0
        */
        String toString() const;

        /** ���ַ�����ȡ�汾
        @version NIIEngine 3.0.0
        */
        void fromString(const String & str);
    public:
        NIIi major;
        NIIi minor;
        NIIi build;
        NIIi release;
    };
}

#endif