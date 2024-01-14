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
#include "NiiApuOpenCLBuffer.h"
#include "NiiApuOpenCLProgram.h"
#include "NiiLogManager.h"

#define FILL_CL_PROGRAM_PATH _T("builtin_Fill")

static const char* fillKernelsCL =
"/*\n"
"Copyright (c) 2012 Advanced Micro Devices, Inc.  \n"
"This software is provided 'as-is', without any express or implied warranty.\n"
"In no event will the authors be held liable for any damages arising from the use of this software.\n"
"Permission is granted to anyone to use this software for any purpose, \n"
"including commercial applications, and to alter it and redistribute it freely, \n"
"subject to the following restrictions:\n"
"1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n"
"2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n"
"3. This notice may not be removed or altered from any source distribution.\n"
"*/\n"
"//Originally written by Takahiro Harada\n"
"#pragma OPENCL EXTENSION cl_amd_printf : enable\n"
"#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable\n"
"typedef unsigned int u32;\n"
"#define GET_GROUP_IDX get_group_id(0)\n"
"#define GET_LOCAL_IDX get_local_id(0)\n"
"#define GET_GLOBAL_IDX get_global_id(0)\n"
"#define GET_GROUP_SIZE get_local_size(0)\n"
"#define GROUP_LDS_BARRIER barrier(CLK_LOCAL_MEM_FENCE)\n"
"#define GROUP_MEM_FENCE mem_fence(CLK_LOCAL_MEM_FENCE)\n"
"#define AtomInc(x) atom_inc(&(x))\n"
"#define AtomInc1(x, out) out = atom_inc(&(x))\n"
"#define make_uint4 (uint4)\n"
"#define make_uint2 (uint2)\n"
"#define make_int2 (int2)\n"
"typedef struct\n"
"{\n"
"	union\n"
"	{\n"
"		int4 m_data;\n"
"		uint4 m_unsignedData;\n"
"		float	m_floatData;\n"
"	};\n"
"	int m_offset;\n"
"	int m_n;\n"
"	int m_padding[2];\n"
"} ConstBuffer;\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(64,1,1)))\n"
"void FillIntKernel(__global int* dstInt, 			int num_elements, int value, const int offset)\n"
"{\n"
"	int gIdx = GET_GLOBAL_IDX;\n"
"	if( gIdx < num_elements )\n"
"	{\n"
"		dstInt[ offset+gIdx ] = value;\n"
"	}\n"
"}\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(64,1,1)))\n"
"void FillFloatKernel(__global float* dstFloat, 	int num_elements, float value, const int offset)\n"
"{\n"
"	int gIdx = GET_GLOBAL_IDX;\n"
"	if( gIdx < num_elements )\n"
"	{\n"
"		dstFloat[ offset+gIdx ] = value;\n"
"	}\n"
"}\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(64,1,1)))\n"
"void FillUnsignedIntKernel(__global unsigned int* dstInt, const int num, const unsigned int value, const int offset)\n"
"{\n"
"	int gIdx = GET_GLOBAL_IDX;\n"
"	if( gIdx < num )\n"
"	{\n"
"		dstInt[ offset+gIdx ] = value;\n"
"	}\n"
"}\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(64,1,1)))\n"
"void FillInt2Kernel(__global int2* dstInt2, 	const int num, const int2 value, const int offset)\n"
"{\n"
"	int gIdx = GET_GLOBAL_IDX;\n"
"	if( gIdx < num )\n"
"	{\n"
"		dstInt2[ gIdx + offset] = make_int2( value.x, value.y );\n"
"	}\n"
"}\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(64,1,1)))\n"
"void FillInt4Kernel(__global int4* dstInt4, 		const int num, const int4 value, const int offset)\n"
"{\n"
"	int gIdx = GET_GLOBAL_IDX;\n"
"	if( gIdx < num )\n"
"	{\n"
"		dstInt4[ offset+gIdx ] = value;\n"
"	}\n"
"}\n";

namespace NII
{
    OpenCLApuProgram * OpenCLApuBuffer::mFillProgram = 0;
    //-----------------------------------------------------------------------------
    OpenCLApuBuffer::~OpenCLApuBuffer()
    {
        if (mData && mCopyData)
        {
            clReleaseMemObject(mData);
            mData = 0;
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::setup(const ApuContext * ctx)
    {
        const OpenCLApuContext * tctx = static_cast<const OpenCLApuContext *>(ctx);
        VString tkernel = "FillIntKernel;FillUnsignedIntKernel;FillFloatKernel;FillInt2Kernel";
        VString tprogram = fillKernelsCL;
        mFillProgram = static_cast<OpenCLApuProgram *>(N_Only(Simd).create(0, GroupGlobal, tkernel, ctx, tprogram));
        mFillProgram->setName(FILL_CL_PROGRAM_PATH);
        mFillProgram->load();
        //cl_int erno;
        //cl_program fillProg = OpenCL::compileProgram(ctx, device, fillKernelsCL, erno, "", FILL_CL_PROGRAM_PATH);
        //N_assert(fillProg, "");
        //mFillInt = OpenCL::compileKernel(ctx, device, fillKernelsCL, "FillIntKernel", erno, fillProg, "");
        //N_assert(mFillInt, "");
       // mFillUint = OpenCL::compileKernel(ctx, device, fillKernelsCL, "FillUnsignedIntKernel", erno, fillProg, "");
       // N_assert(mFillUint, "");
       // mFillFloat = OpenCL::compileKernel(ctx, device, fillKernelsCL, "FillFloatKernel", erno, fillProg, "");
       // N_assert(mFillFloat, "");
        //mFillInt2X = OpenCL::compileKernel(ctx, device, fillKernelsCL, "FillInt2Kernel", erno, fillProg, "");
       // N_assert(mFillInt2X, "");
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::read(void * out, NCount oft, NCount size)
    {
        cl_int status = clEnqueueReadBuffer(mQueue, mData, 0, oft, size, out, 0, 0, 0);
        N_assert(status == CL_SUCCESS, "");

        if (mRunFinish)
            clFinish(mQueue);
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::write(const void * in, NCount oft, NCount size)
    {
        cl_int status = clEnqueueWriteBuffer(mQueue, mData, 0, oft, size, in, 0, 0, 0);
        N_assert(status == CL_SUCCESS, "");

        if (mRunFinish)
            clFinish(mQueue);
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size) const
    {
        OpenCLApuBuffer * temp = static_cast<OpenCLApuBuffer *>(src);

        cl_int status = clEnqueueCopyBuffer(mQueue, temp->getCLData(), mData, srcOft, oft, size, 0, 0, 0);

        N_assert(status == CL_SUCCESS, "");
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::write(Buffer * src) const
    {
        OpenCLApuBuffer * temp = static_cast<OpenCLApuBuffer *>(src);

        cl_int status = clEnqueueCopyBuffer(mQueue, temp->getCLData(), mData, 0, 0, src->getSize(), 0, 0, 0);

        N_assert(status == CL_SUCCESS, "");
    }
    //-----------------------------------------------------------------------------
    void * OpenCLApuBuffer::lockImpl(Nmark m, NCount oft, NCount size)
    {
        cl_int erno;
        cl_map_flags mapflag = 0;
        if (m & MM_READ)
            mapflag |= CL_MAP_READ;
        if (m & MM_WRITE)
            mapflag |= CL_MAP_WRITE;
        if (m & MM_WHOLE)
            mapflag |= CL_MAP_WRITE_INVALIDATE_REGION;
        void * re = clEnqueueMapBuffer(mQueue, mData, true, mapflag, oft, size, 0, 0, 0, &erno);
        if (erno == CL_SUCCESS)
            return re;
        return 0;
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::unlockImpl()
    {
        if (mRunFinish)
            clFinish(mQueue);
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::setData(cl_mem data, size_t size)
    {
        if (mData && mCopyData)
        {
            clReleaseMemObject(mData);
        }
        mCopyData = false;
        mGrowSize = false;
        mData = data;
        mUnitCount = size;
        mReserveSize = size * mUnitSize;
    }
    //-----------------------------------------------------------------------------
    bool OpenCLApuBuffer::reserveImpl(NCount cnt, Nmark newMode, bool oldData)
    {
        bool result = true;
        //if (mUnitReserveCount * mUnitSize < cnt)
        //{
            if (mGrowSize)
            {
                cl_int erno;
                cl_mem_flags dstflag = 0;
                void * targetSrc = mSrc;
                if ((mMark & M_WRITE) || (mMark & M_READ))
                {
                    if (mMark & M_READ)
                    {
                        if (mMark & M_DEV)
                        {
                            if (mMark & M_WRITE)
                                dstflag |= CL_MEM_READ_WRITE;
                            else
                                dstflag |= CL_MEM_READ_ONLY;
                        }
                        else
                            dstflag |= CL_MEM_HOST_READ_ONLY;
                    }
                    if (mMark & M_WRITE)
                    {
                        if (mMark & M_DEV)
                        {
                            if(mMark & M_READ)
                                dstflag |= CL_MEM_READ_WRITE;
                            else
                                dstflag |= CL_MEM_WRITE_ONLY;
                        }
                        else
                            dstflag |= CL_MEM_HOST_WRITE_ONLY;
                    }
                }
                else
                {
                    dstflag |= CL_MEM_HOST_NO_ACCESS;
                }

                if (mMark & EM_AllocHost)
                {
                    dstflag |= CL_MEM_ALLOC_HOST_PTR;
                    targetSrc = 0;
                }
                if ((mMark & EM_RefHost) && targetSrc)
                {
                    dstflag |= CL_MEM_USE_HOST_PTR;
                }
                if (mMark & EM_CopyHost)
                {
                    if(targetSrc)
                        dstflag |= CL_MEM_COPY_HOST_PTR;
                }

                //create a new OpenCL buffer
                cl_mem buf = clCreateBuffer(mContext, dstflag, cnt, targetSrc, &erno);
                if (erno != CL_SUCCESS)
                {
                    N_error("OpenCL out-of-memory\n");
                    cnt = 0;
                    result = false;
                }
                if (!oldData && (dstflag & CL_MEM_ALLOC_HOST_PTR) && mSrc)
                {
                    void * target = lock(MM_WRITE);
                    memcpy(target, mSrc, mUnitCount * mUnitSize);
                    unlock();
                }
                //#define NII_INITBUFFER
#ifdef NII_INITBUFFER
                Nui8 * src = (Nui8 *)malloc(cnt);
                for (size_t i = 0; i < cnt; i++)
                    src[i] = 0xbb;
                erno = clEnqueueWriteBuffer(mQueue, buf, CL_TRUE, 0, cnt, src, 0, 0, 0);
                N_assert(erno == CL_SUCCESS);
                clFinish(mQueue);
                free(src);
#endif  //NII_INITBUFFER

                if (result && oldData)
                {
                    cl_int status = clEnqueueCopyBuffer(mQueue, mData, buf, 0, 0, mUnitSize * mUnitCount, 0, 0, 0);
                    N_assert(status == CL_SUCCESS, "");
                    readUnit(buf, 0, getUnitCount());
                }

                if (mData && mCopyData)
                {
                    clReleaseMemObject(mData);
                }
                mData = buf;
                mReserveSize = cnt * mUnitSize;
            }
            else
            {
                N_assert(0, "");
                if (mData && mCopyData)
                {
                    clReleaseMemObject(mData);
                }
                mData = 0;
                mReserveSize = 0;
                result = false;
            }
        //}
        return result;
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::execute(NIIf value, NCount n, NCount offset)
    {
        N_assert(n > 0, "");
        {
            mFillProgram->setAtciveQueue(mQueue);
            mFillProgram->setActiveKernel(2);
            mFillProgram->setBuffer(1, mData);
            mFillProgram->setConst(2, &n, sizeof(n));
            mFillProgram->setConst(3, &value, sizeof(value));
            mFillProgram->setConst(4, &offset, sizeof(offset));
            mFillProgram->run(n, 64);
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::execute(Ni32 value, NCount n, NCount offset)
    {
        N_assert(n > 0, "");
        {
            mFillProgram->setAtciveQueue(mQueue);
            mFillProgram->setActiveKernel(0);
            mFillProgram->setBuffer(1, mData);
            mFillProgram->setConst(2, &n, sizeof(n));
            mFillProgram->setConst(3, &value, sizeof(value));
            mFillProgram->setConst(4, &offset, sizeof(offset));
            mFillProgram->run(n, 64);
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::execute(Nui32 value, NCount n, NCount offset)
    {
        N_assert(n > 0, "");
        {
            mFillProgram->setAtciveQueue(mQueue);
            mFillProgram->setActiveKernel(1);
            mFillProgram->setBuffer(1, mData);
            mFillProgram->setConst(2, &n, sizeof(n));
            mFillProgram->setConst(3, &value, sizeof(value));
            mFillProgram->setConst(4, &offset, sizeof(offset));
            mFillProgram->run(n,64);
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::execute(const Vector2i & value, NCount n, NCount offset)
    {
        N_assert(n > 0, "");
        {
            mFillProgram->setAtciveQueue(mQueue);
            mFillProgram->setActiveKernel(3);
            mFillProgram->setBuffer(1, mData);
            mFillProgram->setConst(2, &n, sizeof(n));
            mFillProgram->setConst(3, &value, sizeof(value));
            mFillProgram->setConst(4, &offset, sizeof(offset));
            mFillProgram->run(n, 64);
        }
    }
    //-----------------------------------------------------------------------------
    Buffer * OpenCLApuBuffer::clone(Nmark m) const
    {
        ApuBuffer * re;
        re = N_new OpenCLApuBuffer(mSrc, mUnitSize, mUnitCount, mMark, mContext, mQueue, mReserveSize);
        return re;
    }
    //-----------------------------------------------------------------------------
    /*void copyFromHost(const b3AlignedObjectArray<T> & srcArray)
    {
        size_t newSize = srcArray.size();
        resize(newSize, false);
        if (newSize)
        {
            write(&srcArray[0], 0, newSize);
        }
    }*/
    //-----------------------------------------------------------------------------
    /*void copyToHost(b3AlignedObjectArray<T>& destArray) const
    {
        destArray.resize(size());
        if (size())
            read(&destArray[0], 0, size());
    }*/
    //-----------------------------------------------------------------------------
    /*void copyFromOpenCLArray(const OpenCLApuBuffer<T> & src)
    {
        resize(src.size());
        if (size())
        {
            src.read(mData, 0, size());
        }
    }*/
    //-----------------------------------------------------------------------------
    /*void OpenCLApuBuffer::executeHost(b3AlignedObjectArray<b3Int2>& src, const b3Int2& value, int n, int offset)
    {
        for (int i = 0; i < n; i++)
        {
            src[i + offset] = value;
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuBuffer::executeHost(b3AlignedObjectArray<int>& src, const int value, int n, int offset)
    {
        for (int i = 0; i < n; i++)
        {
            src[i + offset] = value;
        }
    }*/
    //-----------------------------------------------------------------------------
}