/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-31

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

#ifndef _NII_RENDERSYS_DATA_H_
#define _NII_RENDERSYS_DATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ƽ̨�ض�����
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI RenderSysData
    {
    public:
        enum Type
        {
            T_Unknow        = 0,
            T_DX9_0         = 0x01,
            T_DX10_0        = 0x02,
            T_DX11_0        = 0x04,
            T_DX12_0        = 0x08,
            T_OPENGL1_1     = 0x10,
            T_OPENGL2_1     = 0x20,
            T_OPENGL3_0     = 0x30,
            T_OPENGL4_0     = 0x40,
            T_OPENGL4_1     = 0x80,
            T_OPENGL_ES1_1  = 0x100,
            T_OPENGL_ES2_1  = 0x200,
            T_OPENGL_ES3_1  = 0x400,
            T_DIRECT_FB     = 0x800,

            T_WIN32         = 0x100000,
            T_WINRT         = 0x200000,     ///< win8��̿��
            T_WIN8_PHONE    = 0x400000,
            T_LINUX         = 0x800000,     ///< linux�ں�
            T_LINUX_X       = 0x1000000,    ///< x���弶���linuxϵͳ
            T_OSX           = 0x2000000,    ///< ƻ��mac os x(darwin)
            T_IOS           = 0x4000000,    ///< ƻ��iphone(darwin)
            T_ANDROID       = 0x8000000,    ///< ����ϵͳ(linux�ں˼�)
            T_EMSCRIPTEN    = 0x10000000,   ///< web��̿��
            T_FREEBSD       = 0x20000000,   ///< freeBSD
        };
    public:
        /** ��ȡƽ̨������Ϣ
        @param[in] name ��Ҫ��ȡ����������
        @param[in] data ָ��洢��ȡ���ݵ��ڴ�ָ��
        @return �Ƿ��ȡ�ɹ�
        @note һ������׶˽ӿ���д�������
        @version NIIEngien 3.0.0 �߼�api
        */
        virtual bool getPlatformData(const String & name, void * data) const;

        /** ��ȡƽ̨������Ϣ
        @param[in] name ��Ҫ��ȡ����������
        @param[in] data ָ��洢��ȡ���ݵ��ڴ�ָ��
        @return �������
        @note һ������׶˽ӿ���д�������
        @version NIIEngien 3.0.0 �߼�api
        */
        virtual Nmark getPlatformDataType() const;
    };
}
#endif