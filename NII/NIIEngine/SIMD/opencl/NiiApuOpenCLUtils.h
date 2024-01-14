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

#ifndef NIIAPU_OPENCL_UTILS_H
#define NIIAPU_OPENCL_UTILS_H

#define NII_USE_CLEW
#ifdef NII_USE_CLEW
#include "clew.h"
#else

#ifdef __APPLE__
#ifdef USE_MINICL
#include <MiniCL/cl.h>
#else
#include <OpenCL/cl.h>
#include <OpenCL/cl_ext.h>  //clLogMessagesToStderrAPPLE
#endif
#else
#ifdef USE_MINICL
#include <MiniCL/cl.h>
#else
#include <CL/cl.h>
#ifdef _WIN32
#include "CL/cl_gl.h"
#endif  //_WIN32
#endif
#endif  //__APPLE__
#endif

#include "NiiSimdManager.h"

namespace NII
{
    class OpenCLApuContext : public ApuContext , public GPUAlloc
    {
    public:
        OpenCLApuContext() :
            mPlatform(0),
            mContext(0),
            mDevice(0),
            mQueue(0),
            mInit(false)
        {
        }
        ~OpenCLApuContext() 
        {
            if (mInit)
            {
                clReleaseCommandQueue(mQueue);
                clReleaseContext(mContext);
                mInit = false;
            }
        }

        cl_platform_id mPlatform;
        cl_device_id mDevice;
        cl_context mContext;
        cl_command_queue mQueue;
        VString mDeviceName;
        bool mInit;
    };

    #define NII_CLINFOMAX 1024

    class OpenCL
    {
    public:
        typedef struct DeviceInfo
        {
            char mName[NII_CLINFOMAX];
            char mVendor[NII_CLINFOMAX];
            char mVersion[NII_CLINFOMAX];
            char mExtensions[NII_CLINFOMAX];

            cl_device_type m_deviceType;
            cl_uint mComputeUnitCount;
            cl_device_local_mem_type mMemType;
            size_t mWorkgroupSize;
            size_t mWorkitemDims;
            size_t mWorkitemSize[3];
            size_t m2DMaxWidth;
            size_t m2DMaxHeight;
            size_t m3DMaxWidth;
            size_t m3DMaxHeight;
            size_t m3DMaxDepth;
            cl_uint mWidthChar;
            cl_uint mWidthShort;
            cl_uint mWidthInt;
            cl_uint mWidthLong;
            cl_uint mWidthFloat;
            cl_uint mWidthDouble;
            cl_uint mClockFrequency;
            cl_uint mMaxReadImageArgs;
            cl_uint mMaxWriteImageArgs;

            cl_uint mAddressBits;
            cl_ulong mConstantBufferSize;
            cl_ulong mMemSize;
            cl_ulong mGlobalMemSize;
            cl_ulong m_maxMemAllocSize;
            cl_command_queue_properties mQueueProperties;
            cl_bool mErrorCorrectionSupport;
            cl_bool mImageSupport;
        } DeviceInfo;

        typedef struct PlatformInfo
        {
            PlatformInfo()
            {
                mVendor[0] = 0;
                mName[0] = 0;
                mVersion[0] = 0;
            }

            char mVendor[NII_CLINFOMAX];
            char mName[NII_CLINFOMAX];
            char mVersion[NII_CLINFOMAX];
        }PlatformInfo;
    public:
        /**
        @version NIIEngine 4.0.0
        */
        static bool setup();

        /**
        @version NIIEngine 4.0.0
        */
        static cl_context createContext(cl_platform_id platid, cl_device_type type, cl_int & erno, void * glCtx = 0,
            void * glDC = 0, Nidx devidx = -1);

        /**
        @version NIIEngine 4.0.0
        */
        static cl_context createContext(cl_device_type type, cl_int & erno, void * glCtx = 0, void * glDC = 0,
            Nidx devidx = -1, Nidx platidx = -1, cl_platform_id * platid = 0);

        /**
        @version NIIEngine 4.0.0
        */
        static NCount getDeviceCount(cl_context ctx);

        /**
        @version NIIEngine 4.0.0
        */
        static cl_device_id getDevice(cl_context ctx, Nidx devidx);

        /**
        @version NIIEngine 4.0.0
        */
        static void getInfo(cl_device_id device, DeviceInfo * info);

        /**
        @version NIIEngine 4.0.0
        */
        static void printInfo(cl_device_id device);

        /**
        @version NIIEngine 4.0.0
        */
        static cl_program compileProgram(cl_context ctx, cl_device_id device, const VString & source, cl_int & erno,
            const VString & extMacros = "", const String & srcFile = _T(""), bool binCache = true);

        /**
        @version NIIEngine 4.0.0
        */
        static cl_kernel compileKernel(cl_context ctx, cl_device_id device, const VString & source,
            const VString & kernel, cl_int & erno, cl_program prog = 0, const VString & extMacros = "");

        /**
        @version NIIEngine 4.0.0
        */
        static int getPlatformCount(cl_int & erno);

        /**
        @version NIIEngine 4.0.0
        */
        static cl_platform_id getPlatform(Nidx idx, cl_int & erno);

        /**
        @version NIIEngine 4.0.0
        */
        static void getInfo(cl_platform_id platid, PlatformInfo * info);

        /**
        @version NIIEngine 4.0.0
        */
        static void printInfo(cl_platform_id platid);
    };
}
#endif
