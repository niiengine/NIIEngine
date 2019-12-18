/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-4

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        void setName(const String & name);

        /** 获取设备名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 设置供应商
        @version NIIEngine 3.0.0
        */
        void setVendor(GPUVendor v);

        /** 解析和设置供应商
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);
        
        /** 获取供应商
        @version NIIEngine 3.0.0
        */
        GPUVendor getVendor() const;

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
        void setVersion(const DriverVersion & v);

        /** 设置驱动版本
        @version NIIEngine 3.0.0
        */
        void setVersion(const String & v);
        
        /** 获取驱动版本
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