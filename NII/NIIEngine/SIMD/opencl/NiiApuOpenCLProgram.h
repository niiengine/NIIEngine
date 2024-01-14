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

#ifndef NII_APU_LAUNCHERCL_H_
#define NII_APU_LAUNCHERCL_H_

#include "NiiPreInclude.h"
#include "NiiApuOpenCLBuffer.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace NII
{
    class OpenCLApuContext;
    N_ALIGNED_DECL(struct, OpenCLKernelArg, 16)
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

    class OpenCLApuProgram : public ApuProgram
    {
    public:
        OpenCLApuProgram(const OpenCLApuContext * ctx, cl_program program, 
            ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        virtual ~OpenCLApuProgram();

        void setBuffer(const IdxList & ilist, const BufferList & blist);

        void setBuffer(Nidx argidx, const cl_mem & data);

        void setConst(Nidx argidx, const void * data, NCount size);

        inline NCount getArgCount() const                       { return mKernelArgList.size(); }

        inline OpenCLKernelArg getArg(int index)                { return mKernelArgList[index]; }

        inline void setSerializer(bool set)                     { mEnableSerializer = set; }

        inline int getSerializerSize() const                    { return mSerializerSize; }

        inline void setAtciveQueue(cl_command_queue queue)      { mActiveQueue = queue; }

        inline const cl_command_queue & getAtciveQueue() const  { return mActiveQueue; }

        void serializeToFile(const String & fileName, int numWorkItems);

        int deserializeArgs(Nui8 * buf, NCount bufSize, cl_context ctx);

        int validateResults(Nui8 * goldBuffer, int goldBufferCapacity, cl_context ctx);

        int serializeArguments(Nui8 * destBuffer, int destBufferCapacity);

        using ApuProgram::run;

        /// @copydetails ApuProgram::run
        virtual bool run(NCount numThreadsX, NCount numThreadsY, NCount localSizeX, NCount localSizeY);

        /// @copydetails ApuProgram::setParam
        void setParam(ApuProgramParam * param);
    protected:
        /// @copydetails ApuProgram::unloadImpl
        void unloadImpl();

        /// @copydetails ApuProgram::loadCodeImpl
        void loadCodeImpl(const VString & code);
    private:
        typedef vector<OpenCLKernelArg>::type ArgList;
        typedef vector<cl_kernel>::type KernelList;
        const OpenCLApuContext * mCtx;
        cl_command_queue mActiveQueue;
        cl_program mProgram;
        KernelList mKernelList;
        cl_device_id mDevice;
        cl_platform_id mPlatid;
        BufferList mBufferList;
        ArgList mKernelArgList;
        NCount mSerializerSize;
        bool mEnableSerializer;
        bool mRefProgram;
    };
}
#endif
