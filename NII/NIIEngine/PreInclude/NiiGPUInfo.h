/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef _NII_GPU_INFO_H_
#define _NII_GPU_INFO_H_

#include "NiiPreInclude.h"
#include "NiiDriverVersion.h"
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
        GT_Amd      = 2,
        GT_Arm      = 3,
        GT_Intel    = 4,
        GT_PowerVR  = 5,
        GT_SIS      = 6,
        GT_S3       = 7,
        GT_Matrox   = 8,
        GT_3DLABS   = 9,
        GT_WARP     = 10,   // Microsoft WARP http://msdn.microsoft.com/en-us/library/dd285359.aspx
        GT_Apple    = 11,
        GT_Software = 12, 
        GT_Qualcomm = 13,
        GT_Mozilla  = 14,   // WebGL Mozilla/Firefox
        GT_Webkit   = 15,   // WebGL WebKit/Chrome
        GT_Count    = 16
    };

    /** GPU��Ӧ��ϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuVendorDetail : public GPUAlloc
    {
    public:
        GpuVendorDetail();
        GpuVendorDetail(GPUVendor v, const String & ser, Nui32 num);
        
        /// 
        static void init() const;
        
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
        void setName(const String & name)           { mName = name; }

        /** ��ȡ�豸����
        @version NIIEngine 3.0.0
        */
        const String & getName() const              { return mName;  }

        /** ���ù�Ӧ��
        @version NIIEngine 3.0.0
        */
        void setVendor(GPUVendor v)                 { mVendor = v; }

        /** ���������ù�Ӧ��
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);
        
        /** ��ȡ��Ӧ��
        @version NIIEngine 3.0.0
        */
        GPUVendor getVendor() const                 { return mVendor; }

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
        void setVersion(const DriverVersion & v)    { mVersion = v;  }

        /** ���������汾
        @version NIIEngine 3.0.0
        */
        void setVersion(const String & v);
        
        /** ��ȡ�����汾
        @version NIIEngine 3.0.0
        */
        DriverVersion getVersion() const            { return mVersion; }
    protected:
        String mName;
        GPUVendor mVendor;
        DriverVersion mVersion;
    };
}

#endif