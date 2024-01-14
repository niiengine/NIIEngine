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

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif
#include "NiiPreProcess.h"
#include "NiiApuOpenCLUtils.h"
#include "NiiLogManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B3_MAX_CL_DEVICES 16

#ifdef _WIN32
#include <windows.h>
#include <sys/stat.h>
#endif

#ifndef CL_PLATFORM_MINI_CL
#ifdef _WIN32
#ifndef NII_USE_CLEW
#include "CL/cl_gl.h"
#endif
#endif
#endif

namespace NII
{
    bool gDebugForceLoadingFromSource   = false;
    bool gDebugSkipLoadingBinary        = false;
    //-----------------------------------------------------------------------
    //Set the preferred platform vendor using the OpenCL SDK
    static const char* spPlatformVendor =
#if defined(CL_PLATFORM_MINI_CL)
        "MiniCL, SCEA";
#elif defined(CL_PLATFORM_AMD)
        "Advanced Micro Devices, Inc.";
#elif defined(CL_PLATFORM_NVIDIA)
        "NVIDIA Corporation";
#elif defined(CL_PLATFORM_INTEL)
        "Intel(R) Corporation";
#elif defined(NII_USE_CLEW)
        "clew (OpenCL Extension Wrangler library)";
#else
        "Unknown Vendor";
#endif
    //-----------------------------------------------------------------------
    void MyFatalBreakAPPLE(const char* errstr, const void* private_info, size_t cb, void* user_data)
    {
        const char* patloc = strstr(errstr, "Warning");
        //find out if it is a warning or error, exit if error

        if (patloc)
        {
            N_warning("Warning: %s\n", errstr);
        }
        else
        {
            N_error("Error: %s\n", errstr);
            N_assert(0, "");
        }
    }
    //-----------------------------------------------------------------------
    bool OpenCL::setup()
    {
        int result = -1;
#ifdef NII_USE_CLEW
        result = clewInit();
        if (result != CLEW_SUCCESS)
        {
            N_error("clewInit failed with error code %d\n", result);
        }
        else
        {
            N_info("clewInit succesfull \n");
            return true;
        }
#endif
        return false;
    }
    //-----------------------------------------------------------------------
    int OpenCL::getPlatformCount(cl_int & erno)
    {
        setup();

        cl_platform_id pPlatforms[10] = { 0 };

        cl_uint platcnt = 0;
        erno = clGetPlatformIDs(10, pPlatforms, &platcnt);
        //erno = clGetPlatformIDs(0, NULL, &platcnt);

        return platcnt;
    }
    //-----------------------------------------------------------------------
    cl_platform_id OpenCL::getPlatform(Nidx idx, cl_int & erno)
    {
        setup();
        cl_platform_id platform = 0;
        cl_uint platcnt;
        erno = clGetPlatformIDs(0, NULL, &platcnt);

        if (idx < platcnt)
        {
            cl_platform_id * platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * platcnt);
            erno = clGetPlatformIDs(platcnt, platforms, NULL);
            if (erno != CL_SUCCESS)
            {
                return platform;
            }

            platform = platforms[idx];

            free(platforms);
        }
        return platform;
    }
    //-----------------------------------------------------------------------
    void OpenCL::getInfo(cl_platform_id platform, PlatformInfo * info)
    {
        N_assert(platform, "");
        cl_int erno;
        erno = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, NII_CLINFOMAX, info->mVendor, NULL);
        N_assert(erno == CL_SUCCESS, "");
        erno = clGetPlatformInfo(platform, CL_PLATFORM_NAME, NII_CLINFOMAX, info->mName, NULL);
        N_assert(erno == CL_SUCCESS, "");
        erno = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, NII_CLINFOMAX, info->mVersion, NULL);
        N_assert(erno == CL_SUCCESS, "");
    }
    //-----------------------------------------------------------------------
    void OpenCL::printInfo(cl_platform_id platform)
    {
        PlatformInfo info;
        getInfo(platform, &info);
        N_info("Platform info:\n");
        N_info("  CL_PLATFORM_VENDOR: \t\t\t%s\n", info.mVendor);
        N_info("  CL_PLATFORM_NAME: \t\t\t%s\n", info.mName);
        N_info("  CL_PLATFORM_VERSION: \t\t\t%s\n", info.mVersion);
    }
    //-----------------------------------------------------------------------
    cl_context OpenCL::createContext(cl_platform_id platform, cl_device_type deviceType, cl_int & erno,
        void * pGLContext, void* pGLDC, Nidx devidx)
    {
        cl_context reCxt = 0;
        cl_device_id devices[B3_MAX_CL_DEVICES];
        cl_uint devCnt;
        cl_context_properties* cprops;

        /*
        * If we could find our platform, use it. Otherwise pass a NULL and get whatever the
        * implementation thinks we should be using.
        */
        cl_context_properties cps[7] = { 0, 0, 0, 0, 0, 0, 0 };
        cps[0] = CL_CONTEXT_PLATFORM;
        cps[1] = (cl_context_properties)platform;
#ifdef _WIN32
#ifndef NII_USE_CLEW
        if (pGLContext && pGLDC)
        {
            cps[2] = CL_GL_CONTEXT_KHR;
            cps[3] = (cl_context_properties)pGLContext;
            cps[4] = CL_WGL_HDC_KHR;
            cps[5] = (cl_context_properties)pGLDC;
        }
#endif  //NII_USE_CLEW
#endif  //_WIN32

        devCnt = -1;

        erno = clGetDeviceIDs(platform, deviceType, B3_MAX_CL_DEVICES, devices, &devCnt);

        if (erno < 0)
        {
            N_info("clGetDeviceIDs returned %d\n", erno);
            return 0;
        }
        cprops = (NULL == platform) ? NULL : cps;

        if (!devCnt)
            return 0;

        if (pGLContext)
        {
            //search for the GPU that relates to the OpenCL context
            unsigned int i;
            for (i = 0; i < devCnt; i++)
            {
                reCxt = clCreateContext(cprops, 1, &devices[i], NULL, NULL, &erno);
                if (erno == CL_SUCCESS)
                    break;
            }
        }
        else
        {
            if (devidx >= 0 && devidx < devCnt)
            {
                reCxt = clCreateContext(cprops, 1, &devices[devidx], NULL, NULL, &erno);
            }
            else
            {
                //create a context of all devices
#if defined(__APPLE__)
                reCxt = clCreateContext(cprops, devCnt, devices, MyFatalBreakAPPLE, NULL, &erno);
#else
                N_info("numDevices=%d\n", devCnt);

                reCxt = clCreateContext(cprops, devCnt, devices, NULL, NULL, &erno);
#endif
            }
        }

        return reCxt;
    }
    //-----------------------------------------------------------------------
    cl_context OpenCL::createContext(cl_device_type deviceType, cl_int & erno, void* pGLContext,
        void * pGLDC, Nidx devidx, Nidx platidx, cl_platform_id * retPlatformId)
    {
        setup();
        cl_uint platcnt;
        cl_context reCxt = 0;

        erno = clGetPlatformIDs(0, NULL, &platcnt);
        if (erno != CL_SUCCESS)
        {
            return NULL;
        }
        if (platcnt > 0)
        {
            cl_platform_id * platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platcnt);
            erno = clGetPlatformIDs(platcnt, platforms, NULL);
            if (erno != CL_SUCCESS)
            {
                free(platforms);
                return NULL;
            }

            cl_uint i;
            for (i = 0; i < platcnt; ++i)
            {
                char pbuf[128];
                erno = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pbuf), pbuf, NULL);
                if (erno != CL_SUCCESS)
                {
                    return NULL;
                }

                if (platidx >= 0 && i == platidx)
                {
                    cl_platform_id tmpPlatform = platforms[0];
                    platforms[0] = platforms[i];
                    platforms[i] = tmpPlatform;
                    break;
                }
                else
                {
                    if (!strcmp(pbuf, spPlatformVendor))
                    {
                        cl_platform_id tmpPlatform = platforms[0];
                        platforms[0] = platforms[i];
                        platforms[i] = tmpPlatform;
                    }
                }
            }

            for (i = 0; i < platcnt; ++i)
            {
                cl_platform_id platform = platforms[i];
                N_assert(platform, "");

                reCxt = createContext(platform, deviceType, erno, pGLContext, pGLDC, devidx);

                if (reCxt)
                {
                    PlatformInfo info;
                    getInfo(platform, &info);

                    if (retPlatformId)
                        *retPlatformId = platform;

                    break;
                }
            }

            free(platforms);
        }
        return reCxt;
    }
    //-----------------------------------------------------------------------
    //////////////////////////////////////////////////////////////////////////////
    //! Gets the id of the nth device from the context
    //!
    //! @return the id or -1 when out of range
    //! @param ctx         OpenCL context
    //! @param device_idx            index of the device of interest
    //////////////////////////////////////////////////////////////////////////////
    cl_device_id OpenCL::getDevice(cl_context ctx, Nidx deviceIndex)
    {
        N_assert(ctx, "");
        size_t szParmDataBytes;
        // get the list of devices associated with context
        clGetContextInfo(ctx, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);

        if (szParmDataBytes / sizeof(cl_device_id) < deviceIndex)
        {
            return (cl_device_id)-1;
        }

        cl_device_id * cdDevices = (cl_device_id *)malloc(szParmDataBytes);

        clGetContextInfo(ctx, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);

        cl_device_id device = cdDevices[deviceIndex];
        free(cdDevices);

        return device;
    }
    //-----------------------------------------------------------------------
    NCount OpenCL::getDeviceCount(cl_context ctx)
    {
        size_t devDataBytes;
        clGetContextInfo(ctx, CL_CONTEXT_DEVICES, 0, NULL, &devDataBytes);
        int device_count = (int)devDataBytes / sizeof(cl_device_id);
        return device_count;
    }
    //-----------------------------------------------------------------------
    void OpenCL::getInfo(cl_device_id device, DeviceInfo* info)
    {
        // CL_DEVICE_NAME
        clGetDeviceInfo(device, CL_DEVICE_NAME, NII_CLINFOMAX, &info->mName, NULL);

        // CL_DEVICE_VENDOR
        clGetDeviceInfo(device, CL_DEVICE_VENDOR, NII_CLINFOMAX, &info->mVendor, NULL);

        // CL_DRIVER_VERSION
        clGetDeviceInfo(device, CL_DRIVER_VERSION, NII_CLINFOMAX, &info->mVersion, NULL);

        // CL_DEVICE_INFO
        clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &info->m_deviceType, NULL);

        // CL_DEVICE_MAX_COMPUTE_UNITS
        clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(info->mComputeUnitCount), &info->mComputeUnitCount, NULL);

        // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(info->mWorkitemDims), &info->mWorkitemDims, NULL);

        // CL_DEVICE_MAX_WORK_ITEM_SIZES
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(info->mWorkitemSize), &info->mWorkitemSize, NULL);

        // CL_DEVICE_MAX_WORK_GROUP_SIZE
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(info->mWorkgroupSize), &info->mWorkgroupSize, NULL);

        // CL_DEVICE_MAX_CLOCK_FREQUENCY
        clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(info->mClockFrequency), &info->mClockFrequency, NULL);

        // CL_DEVICE_ADDRESS_BITS
        clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(info->mAddressBits), &info->mAddressBits, NULL);

        // CL_DEVICE_MAX_MEM_ALLOC_SIZE
        clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(info->m_maxMemAllocSize), &info->m_maxMemAllocSize, NULL);

        // CL_DEVICE_GLOBAL_MEM_SIZE
        clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(info->mGlobalMemSize), &info->mGlobalMemSize, NULL);

        // CL_DEVICE_ERROR_CORRECTION_SUPPORT
        clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(info->mErrorCorrectionSupport), &info->mErrorCorrectionSupport, NULL);

        // CL_DEVICE_LOCAL_MEM_TYPE
        clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(info->mMemType), &info->mMemType, NULL);

        // CL_DEVICE_LOCAL_MEM_SIZE
        clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(info->mMemSize), &info->mMemSize, NULL);

        // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
        clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(info->mConstantBufferSize), &info->mConstantBufferSize, NULL);

        // CL_DEVICE_QUEUE_PROPERTIES
        clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(info->mQueueProperties), &info->mQueueProperties, NULL);

        // CL_DEVICE_IMAGE_SUPPORT
        clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(info->mImageSupport), &info->mImageSupport, NULL);

        // CL_DEVICE_MAX_READ_IMAGE_ARGS
        clGetDeviceInfo(device, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(info->mMaxReadImageArgs), &info->mMaxReadImageArgs, NULL);

        // CL_DEVICE_MAX_WRITE_IMAGE_ARGS
        clGetDeviceInfo(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(info->mMaxWriteImageArgs), &info->mMaxWriteImageArgs, NULL);

        // CL_DEVICE_IMAGE2D_MAX_WIDTH, CL_DEVICE_IMAGE2D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_WIDTH, CL_DEVICE_IMAGE3D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_DEPTH
        clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &info->m2DMaxWidth, NULL);
        clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &info->m2DMaxHeight, NULL);
        clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &info->m3DMaxWidth, NULL);
        clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &info->m3DMaxHeight, NULL);
        clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &info->m3DMaxDepth, NULL);

        // CL_DEVICE_EXTENSIONS: get device extensions, and if any then parse & log the string onto separate lines
        clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, NII_CLINFOMAX, &info->mExtensions, NULL);

        // CL_DEVICE_PREFERRED_VECTOR_WIDTH_<type>
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &info->mWidthChar, NULL);
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &info->mWidthShort, NULL);
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &info->mWidthInt, NULL);
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &info->mWidthLong, NULL);
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &info->mWidthFloat, NULL);
        clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &info->mWidthDouble, NULL);
    }
    //-----------------------------------------------------------------------
    void OpenCL::printInfo(cl_device_id device)
    {
        DeviceInfo info;
        getInfo(device, &info);
        N_info("Device Info:\n");
        N_info("  CL_DEVICE_NAME: \t\t\t%s\n", info.mName);
        N_info("  CL_DEVICE_VENDOR: \t\t\t%s\n", info.mVendor);
        N_info("  CL_DRIVER_VERSION: \t\t\t%s\n", info.mVersion);

        if (info.m_deviceType & CL_DEVICE_TYPE_CPU)
            N_info("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_CPU");
        if (info.m_deviceType & CL_DEVICE_TYPE_GPU)
            N_info("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_GPU");
        if (info.m_deviceType & CL_DEVICE_TYPE_ACCELERATOR)
            N_info("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_ACCELERATOR");
        if (info.m_deviceType & CL_DEVICE_TYPE_DEFAULT)
            N_info("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_DEFAULT");

        N_info("  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t%u\n", info.mComputeUnitCount);
        N_info("  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t%u\n", info.mWorkitemDims);
        N_info("  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%u / %u / %u \n", info.mWorkitemSize[0], info.mWorkitemSize[1], info.mWorkitemSize[2]);
        N_info("  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%u\n", info.mWorkgroupSize);
        N_info("  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t%u MHz\n", info.mClockFrequency);
        N_info("  CL_DEVICE_ADDRESS_BITS:\t\t%u\n", info.mAddressBits);
        N_info("  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n", (unsigned int)(info.m_maxMemAllocSize / (1024 * 1024)));
        N_info("  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte\n", (unsigned int)(info.mGlobalMemSize / (1024 * 1024)));
        N_info("  CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s\n", info.mErrorCorrectionSupport == CL_TRUE ? "yes" : "no");
        N_info("  CL_DEVICE_LOCAL_MEM_TYPE:\t\t%s\n", info.mMemType == 1 ? "local" : "global");
        N_info("  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte\n", (unsigned int)(info.mMemSize / 1024));
        N_info("  CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%u KByte\n", (unsigned int)(info.mConstantBufferSize / 1024));
        if (info.mQueueProperties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE)
            N_info("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
        if (info.mQueueProperties & CL_QUEUE_PROFILING_ENABLE)
            N_info("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_PROFILING_ENABLE");

        N_info("  CL_DEVICE_IMAGE_SUPPORT:\t\t%u\n", info.mImageSupport);

        N_info("  CL_DEVICE_MAX_READ_IMAGE_ARGS:\t%u\n", info.mMaxReadImageArgs);
        N_info("  CL_DEVICE_MAX_WRITE_IMAGE_ARGS:\t%u\n", info.mMaxWriteImageArgs);
        N_info("\n  CL_DEVICE_IMAGE <dim>");
        N_info("\t\t\t2D_MAX_WIDTH\t %u\n", info.m2DMaxWidth);
        N_info("\t\t\t\t\t2D_MAX_HEIGHT\t %u\n", info.m2DMaxHeight);
        N_info("\t\t\t\t\t3D_MAX_WIDTH\t %u\n", info.m3DMaxWidth);
        N_info("\t\t\t\t\t3D_MAX_HEIGHT\t %u\n", info.m3DMaxHeight);
        N_info("\t\t\t\t\t3D_MAX_DEPTH\t %u\n", info.m3DMaxDepth);
        if (*info.mExtensions != 0)
        {
            N_info("\n  CL_DEVICE_EXTENSIONS:%s\n", info.mExtensions);
        }
        else
        {
            N_info("  CL_DEVICE_EXTENSIONS: None\n");
        }
        N_info("  CL_DEVICE_PREFERRED_VECTOR_WIDTH_<t>\t");
        N_info("CHAR %u, SHORT %u, INT %u,LONG %u, FLOAT %u, DOUBLE %u\n\n\n",
            info.mWidthChar, info.mWidthShort, info.mWidthInt, info.mWidthLong, info.mWidthFloat, info.mWidthDouble);
    }
    //-----------------------------------------------------------------------
    static const Ntchar * strip2(const Ntchar * name, const Ntchar * pattern)
    {
        size_t const patlen = Nstrlen(pattern);
        size_t patcnt = 0;
        const Ntchar * oriptr;
        const Ntchar * patloc;
        // find how many times the pattern occurs in the original string
        for (oriptr = name; (patloc = Nstrstr(oriptr, pattern)); oriptr = patloc + patlen)
        {
            patcnt++;
        }
        return oriptr;
    }
    //-----------------------------------------------------------------------
    cl_program OpenCL::compileProgram(cl_context ctx, cl_device_id device, const VString & kernelSourceOrg,
        cl_int & erno, const VString & extMacros, const String & srcFile, bool binCache)
    {
        cl_int status;

        Ntchar binFile[NII_CLINFOMAX];
        const Ntchar * strippedName;
        int fileUpToDate = 0;

        if (binCache && srcFile.size())
        {
            char devname[256];
            char driverver[256];
            clGetDeviceInfo(device, CL_DEVICE_NAME, 256, &devname, NULL);
            clGetDeviceInfo(device, CL_DRIVER_VERSION, 256, &driverver, NULL);

            strippedName = strip2(srcFile.c_str(), _T("\\"));
            strippedName = strip2(strippedName, _T("/"));
            String tdevname;
            String tdriverver;
            StrConv::conv(devname, tdevname);
            StrConv::conv(driverver, tdriverver);
#ifdef _MSC_VER
            Nstprintf_s(binFile, NII_CLINFOMAX, _T("%s/%s.%s.%s.bin"), N_Only(Simd).getCachePath().c_str(), strippedName, tdevname.c_str(), tdriverver.c_str());
#else
            Nstprintf(binFile, "%s/%s.%s.%s.bin", N_Only(Simd).getCachePath().c_str(), strippedName, tdevname.c_str(), tdriverver.c_str());
#endif
        }
        if (srcFile.size() && (binCache || !(gDebugSkipLoadingBinary || gDebugForceLoadingFromSource)))
        {
#ifdef _WIN32
            FILETIME modtimeBinary;
            CreateDirectory(N_Only(Simd).getCachePath().c_str(), 0);
            {
                HANDLE binaryFileHandle = CreateFile(binFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                if (binaryFileHandle == INVALID_HANDLE_VALUE)
                {
                    DWORD errorCode;
                    errorCode = GetLastError();
                    switch (errorCode)
                    {
                    case ERROR_FILE_NOT_FOUND:
                    {
                        N_warning("\nCached file not found %s\n", binFile);
                        break;
                    }
                    case ERROR_PATH_NOT_FOUND:
                    {
                        N_warning("\nCached file path not found %s\n", binFile);
                        break;
                    }
                    default:
                        N_warning("\nFailed reading cached file with errorCode = %d\n", errorCode);
                        break;
                    }
                }
                else
                {
                    if (GetFileTime(binaryFileHandle, NULL, NULL, &modtimeBinary) == 0)
                    {
                        DWORD errorCode;
                        errorCode = GetLastError();
                        N_warning("\nGetFileTime errorCode = %d\n", errorCode);
                    }
                    else
                    {
                        if (!StrUtil::beginsWith(srcFile, _T("builtin_"), false))
                        {
                            HANDLE srcFileHandle = CreateFile(srcFile.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

                            if (srcFileHandle == INVALID_HANDLE_VALUE)
                            {
                                const Ntchar * prefix[] = { _T("./"), _T("../"), _T("../../") };
                                for (int i = 0; (srcFileHandle == INVALID_HANDLE_VALUE) && i < 3; i++)
                                {
                                    Ntchar relativeFileName[1024];
                                    Nsntprintf(relativeFileName, 1024, _T("%s%s"), prefix[i], srcFile.c_str());
                                    srcFileHandle = CreateFile(relativeFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                                }
                            }

                            if (srcFileHandle != INVALID_HANDLE_VALUE)
                            {
                                FILETIME modtimeSrc;
                                if (GetFileTime(srcFileHandle, NULL, NULL, &modtimeSrc) == 0)
                                {
                                    DWORD errorCode = GetLastError();
                                    N_warning("\nGetFileTime errorCode = %d\n", errorCode);
                                }
                                else if ((modtimeSrc.dwHighDateTime < modtimeBinary.dwHighDateTime) || ((modtimeSrc.dwHighDateTime == modtimeBinary.dwHighDateTime) && (modtimeSrc.dwLowDateTime <= modtimeBinary.dwLowDateTime)))
                                {
                                    fileUpToDate = 1;
                                }
                                else
                                {
                                    N_warning("\nCached binary file out-of-date (%s)\n", binFile);
                                }
                                CloseHandle(srcFileHandle);
                            }
                            else
                            {
#ifdef _DEBUG
                                DWORD errorCode = GetLastError();
                                switch (errorCode)
                                {
                                case ERROR_FILE_NOT_FOUND:
                                {
                                    N_warning("\nSrc file not found %s\n", srcFile.c_str());
                                    break;
                                }
                                case ERROR_PATH_NOT_FOUND:
                                {
                                    N_warning("\nSrc path not found %s\n", srcFile.c_str());
                                    break;
                                }
                                default:
                                {
                                    N_warning("\nnSrc file reading errorCode = %d\n", errorCode);
                                }
                                }

                                //we should make sure the src file exists so we can verify the timestamp with binary
                                N_warning("Warning: cannot find OpenCL kernel %s to verify timestamp of binary cached kernel %s\n", srcFile.c_str(), binFile);
                                fileUpToDate = true;
#else
                                //if we cannot find the source, assume it is OK in release builds
                                fileUpToDate = true;
#endif
                            }
                        }
                        else
                        {
                            fileUpToDate = true;
                        }
                    }
                    CloseHandle(binaryFileHandle);
                }
            }
#else
            fileUpToDate = true;
            if (mkdir(N_Only(Simd).getCachePath().c_str(), 0777) != -1)
            {
                N_info("Succesfully created cache directory: %s\n", N_Only(Simd).getCachePath().c_str());
            }
#endif  //_WIN32
        }

        cl_program dstprog = 0;

        if (fileUpToDate)
        {
#ifdef _MSC_VER
            FILE * file;
            if (Nfopen_s(&file, binFile, _T("rb")) != 0)
                file = 0;
#else
            FILE * file = Nfopen(binFile, _T("rb"));
#endif
            if (file)
            {
                size_t binarySize = 0;
                char * binary = 0;

                fseek(file, 0L, SEEK_END);
                binarySize = ftell(file);
                rewind(file);
                binary = (char*)malloc(sizeof(char) * binarySize);
                int bytesRead = fread(binary, sizeof(char), binarySize, file);
                fclose(file);

                dstprog = clCreateProgramWithBinary(ctx, 1, &device, &binarySize, (const unsigned char**)&binary, 0, &status);
                N_assert(status == CL_SUCCESS, "");
                status = clBuildProgram(dstprog, 1, &device, extMacros.c_str(), 0, 0);
                N_assert(status == CL_SUCCESS, "");

                if (status != CL_SUCCESS)
                {
                    char* build_log;
                    size_t ret_val_size;
                    clGetProgramBuildInfo(dstprog, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
                    build_log = (char*)malloc(sizeof(char) * (ret_val_size + 1));
                    clGetProgramBuildInfo(dstprog, device, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
                    build_log[ret_val_size] = '\0';
                    N_error("%s\n", build_log);
                    free(build_log);
                    N_assert(0, "");
                    dstprog = 0;

                    N_warning("clBuildProgram reported failure on cached binary: %s\n", binFile);
                }
                else
                {
                    N_info("clBuildProgram successfully compiled cached binary: %s\n", binFile);
                }
                free(binary);
            }
            else
            {
                N_warning("Cannot open cached binary: %s\n", binFile);
            }
        }

        if (!dstprog)
        {
            char * compileFlags;
            int flagsize;

            const char* kernelSource = kernelSourceOrg.c_str();

            if (!StrUtil::beginsWith(srcFile, _T("builtin_"), false) &&
                (!kernelSourceOrg.size() || gDebugForceLoadingFromSource))
            {
                if (srcFile.size())
                {
                    FILE* file = Nfopen(srcFile.c_str(), _T("rb"));
                    //in many cases the relative path is a few levels up the directory hierarchy, so try it
                    if (!file)
                    {
                        const Ntchar * prefix[] = { _T("../"), _T("../../"), _T("../../../") };
                        for(int i = 0; !file && i < 3; i++)
                        {
                            Ntchar relativeFileName[1024];
                            Nsntprintf(relativeFileName, 1024, _T("%s%s"), prefix[i], srcFile.c_str());
                            file = Nfopen(relativeFileName, _T("rb"));
                        }
                    }

                    if (file)
                    {
                        fseek(file, 0L, SEEK_END);
                        int kernelSize = ftell(file);
                        rewind(file);
                        char * kernelSrc = (char*)malloc(kernelSize + 1);
                        int readBytes;
                        readBytes = fread((void*)kernelSrc, 1, kernelSize, file);
                        kernelSrc[kernelSize] = 0;
                        fclose(file);
                        kernelSource = kernelSrc;
                    }
                }
            }

            size_t program_length = kernelSource ? strlen(kernelSource) : 0;
#ifdef MAC  //or __APPLE__?
            char* flags = "-cl-mad-enable -DMAC ";
#else
            const char* flags = "";
#endif
            dstprog = clCreateProgramWithSource(ctx, 1, (const char**)&kernelSource, &program_length, &erno);
            if (erno != CL_SUCCESS)
            {
                return 0;
            }

            // Build the program with 'mad' Optimization option
            flagsize = sizeof(char) * (extMacros.size() + strlen(flags) + 5);
            compileFlags = (char*)malloc(flagsize);
#ifdef _MSC_VER
            sprintf_s(compileFlags, flagsize, "%s %s", flags, extMacros.c_str());
#else
            sprintf(compileFlags, "%s %s", flags, extMacros.c_str());
#endif
            erno = clBuildProgram(dstprog, 1, &device, compileFlags, NULL, NULL);
            if (erno != CL_SUCCESS)
            {
                char* build_log;
                size_t ret_val_size;
                clGetProgramBuildInfo(dstprog, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
                build_log = (char*)malloc(sizeof(char) * (ret_val_size + 1));
                clGetProgramBuildInfo(dstprog, device, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);

                // to be carefully, terminate with \0
                // there's no information in the reference whether the string is 0 terminated or not
                build_log[ret_val_size] = '\0';

                N_error("Error in clBuildProgram, Line %u in file %s, Log: \n%s\n !!!\n\n", __LINE__, __FILE__, build_log);
                free(build_log);
                return 0;
            }

            if (binCache && srcFile.size())
            {  //	write to binary
                cl_uint numAssociatedDevices;
                status = clGetProgramInfo(dstprog, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &numAssociatedDevices, 0);
                N_assert(status == CL_SUCCESS, "");
                if (numAssociatedDevices == 1)
                {
                    size_t binarySize;
                    status = clGetProgramInfo(dstprog, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binarySize, 0);
                    N_assert(status == CL_SUCCESS, "");

                    char * binary = (char*)malloc(sizeof(char) * binarySize);

                    status = clGetProgramInfo(dstprog, CL_PROGRAM_BINARIES, sizeof(char*), &binary, 0);
                    N_assert(status == CL_SUCCESS, "");

                    {
                        FILE * file = 0;
#ifdef _MSC_VER
                        if (Nfopen_s(&file, binFile, _T("wb")) != 0)
                            file = 0;
#else
                        file = fopen(binFile, "wb");
#endif
                        if (file)
                        {
                            fwrite(binary, sizeof(char), binarySize, file);
                            fclose(file);
                        }
                        else
                        {
                            N_warning("cannot write file %s\n", binFile);
                        }
                    }

                    free(binary);
                }
            }

            free(compileFlags);
        }
        return dstprog;
    }
    //-----------------------------------------------------------------------
    cl_kernel OpenCL::compileKernel(cl_context ctx, cl_device_id device, const VString & kernel,
        const VString & kernelName, cl_int & erno, cl_program prog, const VString & extmacros)
    {
        cl_kernel rekernel;
        cl_program dstprog = prog;

        N_info("compiling kernel %s ", kernelName);

        if (!dstprog)
        {
            dstprog = compileProgram(ctx, device, kernel, erno, extmacros, _T(""), true);
        }

        rekernel = clCreateKernel(dstprog, kernelName.c_str(), &erno);
        if (erno != CL_SUCCESS)
        {
            N_error("Error in clCreateKernel, Line %u in file %s, cannot find kernel function %s !!!\n\n", __LINE__, __FILE__, kernelName);
            N_assert(0, "");
            return 0;
        }

        if (!prog && dstprog)
        {
            clReleaseProgram(dstprog);
        }
        N_info("ready. \n");

        erno = CL_SUCCESS;
        return rekernel;
    }
    //-----------------------------------------------------------------------
}