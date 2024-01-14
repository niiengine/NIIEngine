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
#include "NiiApuProgram.h"
#include "NiiSimdManager.h"
#include "NiiLogManager.h"
#include "NiiSysSyncParam.h"
#include "NiiSerializer.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ApuProgram
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ApuProgramListener::~ApuProgramListener()
    {

    }
    //-----------------------------------------------------------------------------

    class _EngineInner CmdSyntax : public PropertyCmd
    {
    public:
        CmdSyntax() : PropertyCmd(N_PropertyGpuProgram_Syntax, _T("syntax"), _T("Syntax code, e.g. cl"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ApuLanguage syntax = static_cast<const ApuProgram *>(own)->getSyntaxType();
            if (ApuL_OPENCL == syntax)
            {
                out = _T("cl");
            }
            else if (ApuL_CUDA == syntax)
            {
                out = _T("cuda");
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ApuLanguage lang;
            if (val == _T("cl"))
            {
                lang = ApuL_OPENCL;
            }
            else if (val == _T("cuda"))
            {
                lang = ApuL_CUDA;
            }
            static_cast<ApuProgram *>(own)->setSyntaxType(lang);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ApuLanguage * temp = (ApuLanguage *)out;
            *temp = static_cast<const ApuProgram *>(own)->getSyntaxType();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            ApuLanguage * temp = (ApuLanguage *)in;
            static_cast<ApuProgram *>(own)->setSyntaxType(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSyntax();
        }
    };
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ApuProgram
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ApuProgram::ApuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * rl, ResResultScheme * rs, ScriptTypeID stid, LangID lid) :
            Resource(stid, rid, gid, rl, rs, lid),
            mErrorCode(0),
            mActiveKernel(0),
            mProgramSrcValid(true),
            mParamValid(false)
    {
        mCreator = N_OnlyPtr(Simd);
    }
    //-----------------------------------------------------------------------------
    ApuProgram::~ApuProgram()
    {
        /*if(mParams)
        {
            N_delete mParams;
            mParams = 0;
        }*/
    }
    //-----------------------------------------------------------------------------
    bool ApuProgram::run(NCount workerXCnt, NCount workerYCnt, NCount localXCnt, NCount localYCnt)
    {
        return false;
    }
    //-----------------------------------------------------------------------------
    void ApuProgram::setParam(ApuProgramParam * param)
    {
        N_assert(param, "");
        mParams = param;
    }
    //-----------------------------------------------------------------------------
    void ApuProgram::setProgramSrc(const String & filename)
    {
        mFile = filename;
        mSource.clear();
        mProgramSrcValid = true;
        mErrorCode = 0;
    }
    //-----------------------------------------------------------------------------
    void ApuProgram::setProgramCode(const VString & source)
    {
        mSource = source;
        mFile.clear();
        mProgramSrcValid = false;
        mErrorCode = 0;
    }
    //-----------------------------------------------------------------------------
    NCount ApuProgram::calcSize() const
    {
        return 0;
    }
    //-----------------------------------------------------------------------------
    void ApuProgram::loadImpl()
    {
        DataStream * stream = 0;
        if(mProgramSrcValid)
        {
            stream = N_Only(ResourceScheme).open(mFile, mGroup, true, this);
            Nui8 * data;
            NCount size = stream->cloneData(data);
            mSource.assign((const char *)data, size);
            N_free(data);
        }

        try
        {
            N_assert(mSource.size(), "");
            loadCodeImpl(mSource);

            /*if(mParams != 0)
            {
                N_delete mParams;
                mParams = 0;
            }*/
        }
        catch(const Exception &)
        {
            // 将已经被记录
            N_Only(Log).stream() << _I18n("Gpu program ") << N_conv(mID)
                << _I18n(" encountered an error during loading and is thus not supported.");

            mErrorCode = 4;
        }
        if(stream)
            N_delete stream;
    }
    //-----------------------------------------------------------------------------
    bool ApuProgram::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSyntax());
        return true;
    }
    //-----------------------------------------------------------------------------
    bool ApuProgram::isSysSupport() const
    {
        return true;
    }
    //----------------------------------------------------------------------
    bool ApuProgram::isValid() const
    {
        if((mErrorCode != 0) || !isSysSupport())
            return false;

        return N_Only(Simd).isSupport(mSyntax);
    }
    //-------------------------------------------------------------------------
}