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

#ifndef _NII_GPU_INFO_H_
#define _NII_GPU_INFO_H_

#include "NiiPreInclude.h"
#include "NiiDriverVersion.h"
#include "NiiString.h"
#include "NiiStrConv.h"

namespace NII
{
    /** GPU��Ӧ��
    @version NIIEngine 3.0.0
    */
    enum GPUVendor
    {
        GT_Unknow   = 0,
        GT_Nvidia   = 1,
        GT_Ati      = 2,
        GT_Intel    = 3,
        GT_PowerVR  = 4,
        GT_SIS      = 5,
        GT_S3       = 6,
        GT_Matrox   = 7,
        GT_3DLABS   = 8,
        GT_WARP     = 9,
        GT_Apple    = 10,
        GT_Count    = 11
    };

    /** GPU��Ӧ��ϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuVendorDetail : public GPUAlloc
    {
    public:
        GpuVendorDetail();
        GpuVendorDetail(GPUVendor v, const String & ser, Nui32 num);
        
        /** ת������
        @version NIIEngine 3.0.0
        */
        static GPUVendor toVendor(const String & vendor);

        /** ת������
        @version NIIEngine 3.0.0
        */
        static String toStr(GPUVendor v);
    public:
        GPUVendor mVendor;
        String mSer;
        Nui32 mNum;
    protected:
        static StringList mVendorStrList;
    };

    typedef vector<GpuVendorDetail>::type GpuVendorDetailList;

    /** Gpu��Ϣ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GPUInfo : public GPUAlloc
    {
    public:
        GPUInfo();

        /** �����豸����
        @version NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** ��ȡ�豸����
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** ���ù�Ӧ��
        @version NIIEngine 3.0.0
        */
        void setVendor(GPUVendor v);

        /** ���������ù�Ӧ��
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);
        
        /** ��ȡ��Ӧ��
        @version NIIEngine 3.0.0
        */
        GPUVendor getVendor() const;

        /** ��ȡ��Ӧ�����
        @version NIIEngine 3.0.0
        */
        Nui32 getVendorSerNum()const;

        /** ��ȡ��Ӧ������ַ���
        @version NIIEngine 3.0.0
        */
        String getVendorSer() const;
        
        /** ���������汾
        @version NIIEngine 3.0.0
        */
        void setVersion(const DriverVersion & v);

        /** ���������汾
        @version NIIEngine 3.0.0
        */
        void setVersion(const String & v);
        
        /** ��ȡ�����汾
        @version NIIEngine 3.0.0
        */
        DriverVersion getVersion() const;
    protected:
        String mName;
        GPUVendor mVendor;
        DriverVersion mVersion;
    };
}

#endif