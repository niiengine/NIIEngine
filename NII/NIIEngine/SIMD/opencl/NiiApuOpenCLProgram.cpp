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
#include "NiiApuOpenCLProgram.h"
#include "NiiApuOpenCLUtils.h"
#include "NiiString.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    OpenCLApuProgram::OpenCLApuProgram(const OpenCLApuContext * ctx, cl_program program,
        ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
        ApuProgram(rid, gid, ls, rs),
        mCtx(ctx),
        mActiveQueue(ctx->mQueue),
        mProgram(program),
        mEnableSerializer(false)
    {
        mSerializerSize = sizeof(int);
        if (program)
            mRefProgram = true;
        else
            mRefProgram = false;
    }
    //-----------------------------------------------------------------------------
    OpenCLApuProgram::~OpenCLApuProgram()
    {
        unloadImpl();
        for(NCount i = 0; i < mBufferList.size(); i++)
        {
            delete mBufferList[i];
        }
        mBufferList.clear();
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::setBuffer(const IdxList & ilist, const BufferList & blist)
    {
        N_assert(ilist.size() == blist.size(), "");
        BufferList::const_iterator i, iend = blist.end();
        IdxList::const_iterator j, jend = ilist.end();
        for (i = blist.begin(), j = ilist.begin(); i != iend; ++i, ++j)
        {
            OpenCLApuBuffer * arg = static_cast<OpenCLApuBuffer *>(*i);
            if (mEnableSerializer)
            {
                OpenCLKernelArg kernelArg;
                kernelArg.mArgIndex = *j;
                kernelArg.mBufferArg = 1;
                kernelArg.mData = arg->getCLData();

                cl_mem_info param_name = CL_MEM_SIZE;
                size_t param_value;
                size_t sizeInBytes = sizeof(size_t);
                size_t actualSizeInBytes;
                cl_int err = clGetMemObjectInfo(kernelArg.mData, param_name, sizeInBytes, &param_value, &actualSizeInBytes);
                N_assert(err == CL_SUCCESS, "");
                kernelArg.mArgSize = param_value;

                mKernelArgList.push_back(kernelArg);
                mSerializerSize += sizeof(OpenCLKernelArg);
                mSerializerSize += param_value;
            }
            cl_mem adata = arg->getCLData();
            cl_int status = clSetKernelArg(mKernelList[mActiveKernel], *j, sizeof(cl_mem), &adata);
            N_assert(status == CL_SUCCESS, "");
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::setBuffer(Nidx argidx, const cl_mem & data)
    {
        if (mEnableSerializer)
        {
            OpenCLKernelArg kernelArg;
            kernelArg.mArgIndex = argidx;
            kernelArg.mBufferArg = 1;
            kernelArg.mData = data;

            cl_mem_info param_name = CL_MEM_SIZE;
            size_t param_value;
            size_t sizeInBytes = sizeof(size_t);
            size_t actualSizeInBytes;
            cl_int err = clGetMemObjectInfo(kernelArg.mData, param_name, sizeInBytes, &param_value, &actualSizeInBytes);
            N_assert(err == CL_SUCCESS, "");
            kernelArg.mArgSize = param_value;

            mKernelArgList.push_back(kernelArg);
            mSerializerSize += sizeof(OpenCLKernelArg);
            mSerializerSize += param_value;
        }
        cl_int status = clSetKernelArg(mKernelList[mActiveKernel], argidx, sizeof(cl_mem), &data);
        N_assert(status == CL_SUCCESS, "");
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::setConst(Nidx argidx, const void * data, NCount size)
    {
        N_assert(size <= N_ApuMaxConstSize, "");
        if (mEnableSerializer)
        {
            OpenCLKernelArg kernelArg;
            kernelArg.mArgIndex = argidx;
            kernelArg.mBufferArg = 0;
            memcpy(kernelArg.mRawData, data, size);
            kernelArg.mArgSize = size;
            mKernelArgList.push_back(kernelArg);
            mSerializerSize += sizeof(OpenCLKernelArg);
        }
        cl_int status = clSetKernelArg(mKernelList[mActiveKernel], argidx, size, data);
        N_assert(status == CL_SUCCESS, "");
    }
    //-----------------------------------------------------------------------------
    bool OpenCLApuProgram::run(NCount numThreadsX, NCount numThreadsY, NCount localSizeX, NCount localSizeY)
    {
        size_t lRange[3];
        size_t gRange[3];
        lRange[0] = localSizeX;
        lRange[1] = localSizeY;
        lRange[2] = 1;
        gRange[0] = N_MAX(size_t, 1, (numThreadsX / lRange[0]) + (!(numThreadsX % lRange[0]) ? 0 : 1));
        gRange[0] *= lRange[0];
        gRange[1] = N_MAX(size_t, 1, (numThreadsY / lRange[1]) + (!(numThreadsY % lRange[1]) ? 0 : 1));
        gRange[1] *= lRange[1];
        gRange[2] = 1;

        cl_int status = clEnqueueNDRangeKernel(mCtx->mQueue, mKernelList[mActiveKernel], 2, NULL, gRange, lRange, 0, 0, 0);
        if (status != CL_SUCCESS)
        {
            printf("Error: OpenCL status = %d\n", status);
        }
        return status == CL_SUCCESS;
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::setParam(ApuProgramParam * param)
    {
        ApuProgram::setParam(param);
        vector<bool>::type argok;
        NCount argcnt = param->getCount();
        NCount setarg = 0;
        argok.resize(argcnt, false);
        ApuParamMap::UnitList::const_iterator i, iend = param->getMap()->getUnitList().end();
        for (i = param->getMap()->getUnitList().begin(); i != iend;)
        {
            if (i->first < argcnt)
            {
                argok[i->first] = true;
                ++setarg;
                const ApuParamMap::Unit & argunit = i->second;
                if (argunit.isConst())
                {
                    setBuffer(i->first, static_cast<OpenCLApuBuffer *>(argunit.mDst)->getCLData());
                }
                else
                {
                    setConst(i->first, argunit.getConstData(), argunit.mUnitCount * argunit.mUnitSize);
                }
            }
        }
        if (setarg < argcnt)
        {
            setarg = 0;
            vector<bool>::iterator a, aend = argok.end();
            for (a = argok.begin(); a != aend; ++a, ++setarg)
            {
                if (*a == false)
                {
                    i = param->getDefaultUnitList().find(setarg);
                    if (i != param->getDefaultUnitList().end())
                    {
                        const ApuParamMap::Unit & argunit = i->second;
                        if (argunit.isConst())
                        {
                            setBuffer(i->first, static_cast<OpenCLApuBuffer *>(argunit.mDst)->getCLData());
                        }
                        else
                        {
                            setConst(i->first, argunit.getConstData(), argunit.mUnitCount * argunit.mUnitSize);
                        }
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::unloadImpl()
    {
        if (mProgram != 0 && !mRefProgram)
        {
            clReleaseProgram(mProgram);
        }
        mProgram = 0;
        KernelList::iterator i, iend = mKernelList.end();
        for (i = mKernelList.begin(); i != iend; ++i)
        {
            clReleaseKernel(*i);
        }
        mKernelList.clear();
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::loadCodeImpl(const VString & code)
    {
        cl_int erno = CL_SUCCESS;
        if(mProgram == 0)
            mProgram = OpenCL::compileProgram(mCtx->mContext, mCtx->mDevice, code, erno, mExtMacros, mName, N_Only(Simd).isCacheBinary());
        N_assert(erno == CL_SUCCESS, "");
        VStringList itemlist;
        StrUtil::split(mKernel, itemlist, ";");
        VStringList::iterator i, iend = itemlist.end();
        for(i = itemlist.begin(); i != iend; ++i)
        {
            cl_kernel tkernel = OpenCL::compileKernel(mCtx->mContext, mCtx->mDevice, code, mKernel, erno, mProgram, mExtMacros);
            N_assert(tkernel, "");
            mKernelList.push_back(tkernel);
        }
    }
    //-----------------------------------------------------------------------------
    struct OpenCLKernelArgUnaligned
    {
        int mBufferArg;
        int mArgIndex;
        int mArgSize;
        int mReserve;
        union 
        {
            cl_mem mData;
            Nui8 mRawData[N_ApuMaxConstSize];
        };
    };
    //-----------------------------------------------------------------------------
    int OpenCLApuProgram::deserializeArgs(Nui8 * buf, NCount bufSize, cl_context ctx)
    {
        int index = 0;
        int argCnt = *(int*)&buf[index];
        index += sizeof(int);
        for (int i = 0; i < argCnt; i++)
        {
            OpenCLKernelArgUnaligned* arg = (OpenCLKernelArgUnaligned *)&buf[index];

            index += sizeof(OpenCLKernelArg);
            if (arg->mBufferArg)
            {
                OpenCLApuBuffer * clData = new OpenCLApuBuffer(0, 1, arg->mArgSize, Buffer::M_WRITE | Buffer::M_HOST, ctx, mCtx->mQueue, arg->mArgSize);
                
                clData->resize(arg->mArgSize);
                clData->write(&buf[index], 0, arg->mArgSize);

                arg->mData = clData->getCLData();

                mBufferList.push_back(clData);

                cl_int status = clSetKernelArg(mKernelList[mActiveKernel], i, sizeof(cl_mem), &arg->mData);
                N_assert(status == CL_SUCCESS, "");
                index += arg->mArgSize;
            }
            else
            {
                cl_int status = clSetKernelArg(mKernelList[mActiveKernel], i, arg->mArgSize, &arg->mRawData);
                N_assert(status == CL_SUCCESS, "");
            }
            OpenCLKernelArg b;
            memcpy(&b, arg, sizeof(OpenCLKernelArgUnaligned));
            mKernelArgList.push_back(b);
        }
        mSerializerSize = index;
        return index;
    }
    //-----------------------------------------------------------------------------
    int OpenCLApuProgram::validateResults(Nui8 * goldBuffer, int goldBufferCapacity, cl_context ctx)
    {
        int index = 0;

        int argCnt = *(int*)&goldBuffer[index];
        index += sizeof(int);

        if (argCnt != mKernelArgList.size())
        {
            printf("failed validation: expected %d arguments, found %d\n", argCnt, mKernelArgList.size());
            return -1;
        }

        for (int ii = 0; ii < argCnt; ii++)
        {
            OpenCLKernelArg* argGold = (OpenCLKernelArg*)&goldBuffer[index];

            if (mKernelArgList[ii].mArgSize != argGold->mArgSize)
            {
                printf("failed validation: argument %d sizeInBytes expected: %d, found %d\n", ii, argGold->mArgSize, mKernelArgList[ii].mArgSize);
                return -2;
            }

            {
                int expected = argGold->mBufferArg;
                int found = mKernelArgList[ii].mBufferArg;

                if (expected != found)
                {
                    printf("failed validation: argument %d isBuffer expected: %d, found %d\n", ii, expected, found);
                    return -3;
                }
            }
            index += sizeof(OpenCLKernelArg);

            if (argGold->mBufferArg)
            {
                unsigned char* memBuf = (unsigned char*)malloc(mKernelArgList[ii].mArgSize);
                unsigned char* goldBuf = &goldBuffer[index];
                for (int j = 0; j < mKernelArgList[j].mArgSize; j++)
                {
                    memBuf[j] = 0xaa;
                }

                cl_int status = 0;
                status = clEnqueueReadBuffer(mCtx->mQueue, mKernelArgList[ii].mData, CL_TRUE, 0, mKernelArgList[ii].mArgSize,
                    memBuf, 0, 0, 0);
                N_assert(status == CL_SUCCESS, "");
                clFinish(mCtx->mQueue);

                for (int b = 0; b < mKernelArgList[ii].mArgSize; b++)
                {
                    int expected = goldBuf[b];
                    int found = memBuf[b];
                    if (expected != found)
                    {
                        printf("failed validation: argument %d OpenCL data at byte position %d expected: %d, found %d\n",
                            ii, b, expected, found);
                        return -4;
                    }
                }

                index += argGold->mArgSize;
            }
            else
            {
                //compare content
                for (int b = 0; b < mKernelArgList[ii].mArgSize; b++)
                {
                    int expected = argGold->mRawData[b];
                    int found = mKernelArgList[ii].mRawData[b];
                    if (expected != found)
                    {
                        printf("failed validation: argument %d const data at byte position %d expected: %d, found %d\n",
                            ii, b, expected, found);
                        return -5;
                    }
                }
            }
        }
        return index;
    }
    //-----------------------------------------------------------------------------
    int OpenCLApuProgram::serializeArguments(Nui8 * destBuffer, int destBufferCapacity)
    {
        //initialize to known values
        for (int i = 0; i < destBufferCapacity; i++)
            destBuffer[i] = 0xec;

        assert(destBufferCapacity >= mSerializerSize);

        //todo: use the b3Serializer for this to allow for 32/64bit, endianness etc
        int argCnt = mKernelArgList.size();
        int curBufferSize = 0;
        int* dest = (int*)&destBuffer[curBufferSize];
        *dest = argCnt;
        curBufferSize += sizeof(int);

        for (NCount i = 0; i < mKernelArgList.size(); i++)
        {
            OpenCLKernelArg * arg = (OpenCLKernelArg*)&destBuffer[curBufferSize];
            *arg = mKernelArgList[i];
            curBufferSize += sizeof(OpenCLKernelArg);
            if (arg->mBufferArg == 1)
            {
                //copy the OpenCL buffer content
                cl_int status = 0;
                status = clEnqueueReadBuffer(mCtx->mQueue, arg->mData, 0, 0, arg->mArgSize,
                    &destBuffer[curBufferSize], 0, 0, 0);
                N_assert(status == CL_SUCCESS, "");
                clFinish(mCtx->mQueue);
                curBufferSize += arg->mArgSize;
            }
        }
        return curBufferSize;
    }
    //-----------------------------------------------------------------------------
    void OpenCLApuProgram::serializeToFile(const String & fileName, int numWorkItems)
    {
        int buffSize = getSerializerSize();
        Nui8 * buf = new Nui8[buffSize + sizeof(int)];
        for (int i = 0; i < buffSize + 1; i++)
        {
            Nui8 * ptr = (Nui8 *)&buf[i];
            *ptr = 0xff;
        }
        //	int actualWrite = serializeArguments(buf,buffSize);

        //	unsigned char* cptr = (unsigned char*)&buf[buffSize];
        //            printf("buf[buffSize] = %d\n",*cptr);

        assert(buf[buffSize] == 0xff);  //check for buffer overrun
        int* ptr = (int*)&buf[buffSize];

        *ptr = numWorkItems;

        FILE * f = Nfopen(fileName.c_str(), _T("wb"));
        fwrite(buf, buffSize + sizeof(int), 1, f);
        fclose(f);

        delete[] buf;
    }
    //-----------------------------------------------------------------------------
}