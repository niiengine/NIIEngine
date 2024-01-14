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

#include "NiiPreProcess.h"
#include "NiiGPUInfo.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GpuVendorDetail
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StringList GpuVendorDetail::mVendorStrList;
    //------------------------------------------------------------------------
    GpuVendorDetail::GpuVendorDetail() :
        mVendor(GT_Unknow),
        mNum(0)
    {
    }
    //------------------------------------------------------------------------
    GpuVendorDetail::GpuVendorDetail(GPUVendor v, const String & ser, Nui32 num) :
        mVendor(v),
        mSer(ser),
        mNum(num)
    {
    }
    //------------------------------------------------------------------------
    void GpuVendorDetail::init() const
    {
        if (mVendorStrList.empty())
        {
            // Always lower case!
            mVendorStrList.resize(GT_Count);
            mVendorStrList[GT_Unknow] = _T("unknown");
            mVendorStrList[GT_Nvidia] = _T("nvidia");
            mVendorStrList[GT_Amd]  = _T("amd");
            mVendorStrList[GT_Arm]  = _T("arm");
            mVendorStrList[GT_Intel] = _T("intel");
            mVendorStrList[GT_3DLABS] = _T("3dlabs");
            mVendorStrList[GT_S3] = _T("s3");
            mVendorStrList[GT_Matrox] = _T("matrox");
            mVendorStrList[GT_SIS] = _T("sis");
            mVendorStrList[GT_PowerVR] = _T("imagination technologies");
            mVendorStrList[GT_Apple] = _T("apple");
            mVendorStrList[GT_Software] = _T("microsoft"); // Microsoft software device
            mVendorStrList[GT_WARP] = _T("ms warp");
            mVendorStrList[GT_Qualcomm] = _T("qualcomm");
            mVendorStrList[GT_Mozilla] = _T("mozilla");
            mVendorStrList[GT_Webkit] = _T("webkit");
        }
    }
    //------------------------------------------------------------------------
    GPUVendor GpuVendorDetail::toVendor(const String & vendorString)
    {
        init();
        GPUVendor ret = GT_Unknow;
        String cmpString = vendorString;
        StrUtil::toLower(cmpString);
        for (NIIi i = 0; i < GT_Count; ++i)
        {
            // case insensitive (lower case)
            if (mVendorStrList[i] == cmpString)
            {
                ret = static_cast<GPUVendor>(i);
                break;
            }
        }
        return ret;
    }
    //---------------------------------------------------------------------
    String GpuVendorDetail::toStr(GPUVendor v)
    {
        init();
        return mVendorStrList[v];
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GPUInfo
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GPUInfo::GPUInfo():
        mVendor(GT_Unknow)
    {
    }
    //------------------------------------------------------------------------
    void GPUInfo::setVendor(const String & v)
    {
        mVendor = GpuVendorDetail::toVendor(v);
    }
    //------------------------------------------------------------------------
    Nui32 GPUInfo::getVendorSerNum()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    String GPUInfo::getVendorSer() const
    {
        return _T("");
    }
    //------------------------------------------------------------------------
    void GPUInfo::setVersion(const String & v)
    {
        mVersion.fromString(v);
    }
    //------------------------------------------------------------------------
}