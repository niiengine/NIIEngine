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
    /** GPU供应商
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

    /** GPU供应商细节
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuVendorDetail : public GPUAlloc
    {
    public:
        GpuVendorDetail();
        GpuVendorDetail(GPUVendor v, const String & ser, Nui32 num);
        
        /** 转换类型
        @version NIIEngine 3.0.0
        */
        static GPUVendor toVendor(const String & vendor);

        /** 转换类型
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

    /** Gpu信息
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GPUInfo : public GPUAlloc
    {
    public:
        GPUInfo();

        /** 设置设备名字
        @version NIIEngine 3.0.0
        */
        void setName(const String & name)           { mName = name; }

        /** 获取设备名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const              { return mName;  }

        /** 设置供应商
        @version NIIEngine 3.0.0
        */
        void setVendor(GPUVendor v)                 { mVendor = v; }

        /** 解析和设置供应商
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);
        
        /** 获取供应商
        @version NIIEngine 3.0.0
        */
        GPUVendor getVendor() const                 { return mVendor; }

        /** 获取供应商序号
        @version NIIEngine 3.0.0
        */
        Nui32 getVendorSerNum()const;

        /** 获取供应商序号字符串
        @version NIIEngine 3.0.0
        */
        String getVendorSer() const;
        
        /** 设置驱动版本
        @version NIIEngine 3.0.0
        */
        void setVersion(const DriverVersion & v)    { mVersion = v;  }

        /** 设置驱动版本
        @version NIIEngine 3.0.0
        */
        void setVersion(const String & v);
        
        /** 获取驱动版本
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