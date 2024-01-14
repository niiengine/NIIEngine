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
#include "NiiSimdManager.h"
#include "NiiDefaultStream.h"
#include "NiiResourceSchemeManager.h"
#include "NiiApuOpenCLUtils.h"
#include "NiiApuOpenCLProgram.h"
#include "NiiEngine.h"

#define _APM_SAVE       0x01
#define _APM_SAVEFile   0x02
#define _APM_DIRTY      0x04

namespace NII
{
    class UnsupportedApuProgram : public ApuProgram
    {
    public:
        UnsupportedApuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0,
            ScriptTypeID stid = N_CmdObj_GpuProgram, LangID lid = N_PrimaryLang) :
            ApuProgram(rid, gid, ls, rs, stid, lid) { }

        /// @copydetails ApuProgram::isValid
        bool isValid() const { return false; }
    protected:
        void loadImpl() { N_EXCEPT(RenderingAPI, _T("no support gpu program, shader name:") + mSrc + _T("\n")); }
        void loadCodeImpl(const VString & code) { N_EXCEPT(RenderingAPI, _T("no support gpu program, shader name:") + mSrc + _T("\n")); }
        void unloadImpl() { }
    };
    //------------------------------------------------------------------------
    template<> SimdManager * Singleton<SimdManager>::mOnly = 0;
    //------------------------------------------------------------------------
    SimdManager::SimdManager() :
        ResourceManager(RT_ApuProgram)
    {
        mScriptLevel = 50;
        mCacheMark = 0;
    }
    //------------------------------------------------------------------------
    SimdManager::~SimdManager()
    {
        CacheList::iterator j, jend = mCacheList.end();
        for (j = mCacheList.begin(); j != jend; ++j)
        {
            N_delete j->second;
        }
        mCacheList.clear();
    }
    //-----------------------------------------------------------------------
    bool SimdManager::setup()
    {
        return OpenCL::setup();
    }
    //-----------------------------------------------------------------------
    Resource * SimdManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        //String type;
        //String syntax;
        String kernel;
        NCount ctxidx;
        /*if (!params || (!params->get(N_PropertyApuProgram_Syntax, syntax)))
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply 'syntax' parameters"));
        }*/
        /*else if (!params->get(N_PropertyApuProgram_Type, type))
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply 'type' parameters"));
        }*/
        if (!params->get(N_PropertyApuProgram_ContextIndex, ctxidx))
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply parameters"));
        }
        else if (!params->get(N_PropertyApuProgram_Kernel, kernel))
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply parameters"));
        }

        /*ApuLanguage paramSyntax;
        if (syntax == _T("cl"))
        {
            paramSyntax = ApuL_OPENCL;
        }
        else if (syntax == _T("cuda"))
        {
            paramSyntax = ApuL_CUDA;
        }*/

        /*Type paramtype;
        if(type == _T("cpu"))
        {
            paramtype = T_CPU;
        }
        else if (type == _T("gpu"))
        {
            paramtype = T_GPU;
        }
        else if (type == _T("apu"))
        {
            paramtype = T_APU;
        }
        else if (type == _T("all"))
        {
            paramtype = T_ALL;
        }*/
        VString paramkernel;
        StrConv::conv(kernel, paramkernel);

        return createImpl(rid, gid, ls, rs, paramkernel, mApuContextList[ctxidx]);
    }
    //------------------------------------------------------------------------
    Resource * SimdManager::createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, 
        const VString & kernel, const ApuContext * ctx)
    {
        if (ctx->getLang() == ApuL_OPENCL)
        {
            const OpenCLApuContext * clctx = static_cast<const OpenCLApuContext *>(ctx);
            OpenCLApuProgram * re = N_new OpenCLApuProgram(clctx, 0, rid, gid, ls, rs);
            re->setKernel(kernel);
            return re;
        }
        else if (ctx->getLang() == ApuL_CUDA)
        {
            return 0;
        }
        return N_new UnsupportedApuProgram(rid, gid, ls, rs);
    }
    //------------------------------------------------------------------------
    ApuProgram * SimdManager::load(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
        const ApuContext * ctx)
    {
        ApuProgram * prg;
        {
            N_mutex1_lock
            prg = static_cast<ApuProgram *>(get(rid));

            if (prg == 0)
            {
                prg = create(rid, gid, file, kernel, ctx);
            }
        }

        prg->load();
        return prg;
    }
    //------------------------------------------------------------------------
    ApuProgram * SimdManager::load(ResourceID rid, GroupID gid, const VString & kernel,
        const ApuContext * ctx, const VString & src)
    {
        ApuProgram * prg;
        {
            N_mutex1_lock
            prg = static_cast<ApuProgram *>(get(rid));

            if (prg == 0)
            {
                prg = create(rid, gid, kernel, ctx, src);
            }
        }
        prg->load();
        return prg;
    }
    //------------------------------------------------------------------------
    ApuProgram * SimdManager::create(ResourceID rid, GroupID gid, const VString & kernel,
        const ApuContext * ctx, ResLoadScheme * ls, ResResultScheme * rs)
    {
        ApuProgram * re = static_cast<ApuProgram *>(createImpl(rid, gid, ls, rs, kernel, ctx));
        addImpl(re);

        N_Only(ResourceScheme).onCreate(re);
        return re;
    }
    //------------------------------------------------------------------------
    ApuProgram * SimdManager::create(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
        const ApuContext * ctx)
    {
        N_Only(ResourceScheme).create(rid, RT_ApuProgram, file, gid);
        ApuProgram * prg = create(rid, gid, kernel, ctx);
        prg->setKernel(kernel);
        prg->setSyntaxType(ctx->getLang());
        prg->setProgramSrc(file);
        return prg;
    }
    //------------------------------------------------------------------------
    ApuProgram * SimdManager::create(ResourceID rid, GroupID gid, const VString & kernel, 
        const ApuContext * ctx, const VString & src)
    {
        ApuProgram * prg = create(rid, gid, kernel, ctx, src);
        prg->setSyntaxType(ctx->getLang());
        prg->setProgramCode(src);
        return prg;
    }
    //------------------------------------------------------------------------
    ApuLanguageMark SimdManager::getSupportMark() const
    {
        return ApuL_OPENCL;
    }
    //------------------------------------------------------------------------
    bool SimdManager::isSupport(ApuLanguage lang) const
    {
        if(lang == ApuL_OPENCL)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    void SimdManager::create(ApuBuffer *& out, const ApuContext * ctx, void * src, NCount unitSize, NCount unitCnt, Nmark mode)
    {
        if (ctx->getLang() == ApuL_OPENCL)
        {
            const OpenCLApuContext * tctx = static_cast<const OpenCLApuContext *>(ctx);
            out = N_new OpenCLApuBuffer(src, unitSize, unitCnt, mode, tctx->mContext, tctx->mQueue, unitSize * unitCnt);
        }
        else if (ctx->getLang() == ApuL_CUDA)
        {
            out = 0;
        }
        out = 0;
    }
    //------------------------------------------------------------------------
    void SimdManager::destroy(ApuBuffer * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void SimdManager::create(ApuContext *& out, ApuLanguage al, Type type, Nidx platidx, Nidx devidx)
    {
        if (al == ApuL_OPENCL)
        {
            cl_int erno = 0;
            cl_device_type devType = CL_DEVICE_TYPE_ALL;
            switch (type)
            {
            case T_CPU:
                devType = CL_DEVICE_TYPE_CPU;
                break;
            case T_GPU:
                devType = CL_DEVICE_TYPE_GPU;
                break;
            case T_APU:
                devType = CL_DEVICE_TYPE_ACCELERATOR;
                break;
            default :
                break;
            }
            out = N_new OpenCLApuContext();
            OpenCLApuContext * tempout = static_cast<OpenCLApuContext *>(out);
            tempout->mContext = OpenCL::createContext(devType, erno, 0, 0, devidx, platidx, &tempout->mPlatform);

            N_assert(erno == CL_SUCCESS, "");

            NCount numDev = OpenCL::getDeviceCount(tempout->mContext);

            if (numDev > 0)
            {
                tempout->mDevice = OpenCL::getDevice(tempout->mContext, 0);
                tempout->mQueue = clCreateCommandQueue(tempout->mContext, tempout->mDevice, 0, &erno);
                N_assert(erno == CL_SUCCESS, "");

                OpenCL::DeviceInfo info;
                OpenCL::getInfo(tempout->mDevice, &info);
                tempout->mDeviceName = info.mName;
                tempout->mInit = true;
                mApuContextList.push_back(tempout);
            }
        }
        else if(al == ApuL_CUDA)
        {
            out = 0;
        }
        out = 0;
    }
    //------------------------------------------------------------------------
    void SimdManager::destroy(ApuContext * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    Resource * SimdManager::get(ResourceID rid, GroupID gid)
    {
        return ResourceManager::get(rid, gid);
    }
    //------------------------------------------------------------------------
    void SimdManager::setCacheBinary(bool val)
    {
        if (!N_Engine().getRender()->getFeature()->isSupport(GF_Shader_Binary_Get))
            mCacheMark &= ~_APM_SAVE;
        else
        {
            if (val)
                mCacheMark |= _APM_SAVE;
            else
                mCacheMark &= ~_APM_SAVE;
        }
    }
    //------------------------------------------------------------------------
    bool SimdManager::isCacheBinary() const
    {
        return mCacheMark & _APM_SAVE;
    }
    //------------------------------------------------------------------------
    void SimdManager::setCacheBinaryFile(bool val)
    {
        if (!N_Engine().getRender()->getFeature()->isSupport(GF_Shader_Binary_Get))
            mCacheMark &= ~_APM_SAVEFile;
        else
        {
            if (val)
                mCacheMark |= _APM_SAVEFile;
            else
                mCacheMark &= ~_APM_SAVEFile;
        }
    }
    //------------------------------------------------------------------------
    bool SimdManager::isCacheBinaryFile() const
    {
        return mCacheMark & _APM_SAVEFile;
    }
    //------------------------------------------------------------------------
    bool SimdManager::isCacheExist(Nui32 cid) const
    {
        return mCacheList.find(cid) != mCacheList.end();
    }
    //------------------------------------------------------------------------
    MemStream * SimdManager::getCache(Nui32 cid) const
    {
        return mCacheList.find(cid)->second;
    }
    //------------------------------------------------------------------------
    void SimdManager::onDestroy(ApuBuffer * obj)
    {

    }
    //------------------------------------------------------------------------
    MemStream * SimdManager::createCache(Nui32 cid, NCount size)
    {
        MemStream * re = N_new MemStream(size);

        CacheList::iterator foundIter = mCacheList.find(cid);
        if (foundIter == mCacheList.end())
        {
            mCacheList.insert(Npair(cid, re));
            mCacheMark |= _APM_DIRTY;
        }
        else
        {
            foundIter->second = re;
        }

        return re;
    }
    //------------------------------------------------------------------------
    void SimdManager::destroyCache(Nui32 cid)
    {
        CacheList::iterator i = mCacheList.find(cid);
        if (i != mCacheList.end())
        {
            N_delete i->second;
            mCacheList.erase(i);
            mCacheMark |= _APM_DIRTY;
        }
    }
    //------------------------------------------------------------------------
    void SimdManager::exportCache(DataStream * stream) const
    {
        if (!(mCacheMark & _APM_DIRTY))
            return;
        if (!stream->isWriteable())
        {
            N_EXCEPT(IO, _I18n("Unable to write to stream ") + stream->getName());
        }

        // write the size of the array
        NCount sizeOfArray = mCacheList.size();
        stream->write(&sizeOfArray, sizeof(NCount));

        // loop the array and save it
        CacheList::const_iterator iter = mCacheList.begin();
        CacheList::const_iterator iterE = mCacheList.end();
        for (; iter != iterE; iter++)
        {
            // saves the name of the shader
            {
                stream->write(&iter->first, sizeof(Nui32));
            }
            // saves the microcode
            {
                const MemStream * microcodeOfShader = iter->second;
                NCount microcodeLength = microcodeOfShader->size();
                stream->write(&microcodeLength, sizeof(NCount));
                stream->write(microcodeOfShader->getData(), microcodeLength);
            }
        }
    }
    //------------------------------------------------------------------------
    void SimdManager::inportCache(const DataStream * stream)
    {
        mCacheList.clear();

        // write the size of the array
        NCount sizeOfArray = 0;
        stream->read(&sizeOfArray, sizeof(NCount));

        // loop the array and load it
        for (NCount i = 0; i < sizeOfArray; i++)
        {
            Nui32 cid;
            stream->read(&cid, sizeof(Nui32));

            // loads the microcode
            NCount microcodeLength = 0;
            stream->read(&microcodeLength, sizeof(NCount));

            MemStream * microcodeOfShader(N_new MemStream(microcodeLength, N_conv(cid)));
            microcodeOfShader->seek(0);
            stream->read(microcodeOfShader->getData(), microcodeLength);

            mCacheList.insert(Npair(cid, microcodeOfShader));
        }
        mCacheMark &= ~_APM_DIRTY;
    }
    //------------------------------------------------------------------------
}