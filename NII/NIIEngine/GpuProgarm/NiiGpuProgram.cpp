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
#include "NiiGpuProgram.h"
#include "NiiGpuProgramManager.h"
#include "NiiLogManager.h"
#include "NiiSysSyncParam.h"
#include "NiiSerializer.h"
#include "NiiLight.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiDataEquation.h"
#include "NiiAnimationManager.h"
#include "NiiEquationManager.h"
#include "NiiPredefinedControllers.h"
#include "NiiException.h"

namespace NII
{
    /** 预定义控制器值,设置一个单一的浮点值到一个顶点/片段着色程序常量参数
    @remark
        Any value is accepted, it is propagated into the 'x'
        component of the constant register identified by the index. If you
        need to use named parameters, retrieve the index from the param
        object before setting this controller up.
    @note
        Retrieving a value from the program parameters is not currently
        supported, therefore do not use this controller value as a source,
        only as a target.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FloatGpuParameterControllerValue : public DataValue<NIIf>
    {
    public:
        /** Constructor.
        @param[in] params The parameters object to access
        @param[in] index The index of the parameter to be set
        */
        FloatGpuParameterControllerValue(bool autoDsy, GpuProgramParam * params, NCount index) :
            DataValue<NIIf>(autoDsy),
            mParams(params),
            mParamIndex(index) {}

        ~FloatGpuParameterControllerValue() {}
        /// @copydetails DataValue::getValue
        const NIIf getValue() const
        {
            return 0.0f;
        }
        /// @copydetails DataValue::setValue
        void setValue(const NIIf & value)
        {
            Vector4f v4 = Vector4f(value, 0, 0, 0);
            mParams->set(mParamIndex, 0, v4);
        }
    protected:
        GpuProgramParam * mParams;  ///< 需要访问的参数源头
        NCount mParamIndex;
    };

    //------------------------------------------------------------------------
    class _EngineInner CmdType : public PropertyCmd
    {
    public:
        CmdType() : PropertyCmd(N_PropertyGpuProgram_Type, _T("type"),
            _T("'vertex_program', 'geometry_program' or 'fragment_program'"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const GpuProgram * t = static_cast<const GpuProgram *>(own);
            if(t->getType() == GpuProgram::ST_VS)
            {
                out = _T("vertex_program");
            }
            else if (t->getType() == GpuProgram::ST_GS)
            {
                out = _T("geometry_program");
            }
            else
            {
                out = _T("fragment_program");
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            GpuProgram * t = static_cast<GpuProgram *>(own);
            if (val == _T("vertex_program"))
            {
                t->setType(GpuProgram::ST_VS);
            }
            else if (val == _T("geometry_program"))
            {
                t->setType(GpuProgram::ST_GS);
            }
            else
            {
                t->setType(GpuProgram::ST_FS);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out) const
        {
            GpuProgram::ShaderType * temp = (GpuProgram::ShaderType *)out;
            *temp = static_cast<const GpuProgram *>(own)->getType();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            GpuProgram::ShaderType * temp = (GpuProgram::ShaderType *)in;
            static_cast<GpuProgram *>(own)->setType(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("vertex_program");
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdType();
        }
    };

    class _EngineInner CmdSyntax : public PropertyCmd
    {
    public:
        CmdSyntax() : PropertyCmd(N_PropertyGpuProgram_Syntax, _T("syntax"), _T("Syntax code, e.g. vs_1_1"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->getSyntaxType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ShaderLanguage lang;
            StrConv::conv(val, lang);
            static_cast<GpuProgram *>(own)->setSyntaxType(lang);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ShaderLanguage * temp = (ShaderLanguage *)out;
            *temp = static_cast<const GpuProgram *>(own)->getSyntaxType();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            ShaderLanguage * temp = (ShaderLanguage *)in;
            static_cast<GpuProgram *>(own)->setSyntaxType(*temp);
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

    class _EngineInner CmdSkeletal : public PropertyCmd
    {
    public:
        CmdSkeletal() : PropertyCmd(N_PropertyGpuProgram_Skeletal, _T("includes_skeletal_animation"),
            _T("Whether this vertex program includes skeletal animation"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->isVertexMatrixFusion(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<GpuProgram *>(own)->setVertexMatrixFusion(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const GpuProgram *>(own)->isVertexMatrixFusion();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<GpuProgram *>(own)->setVertexMatrixFusion(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSkeletal();
        }
    };

    class _EngineInner CmdMorph : public PropertyCmd
    {
    public:
        CmdMorph() : PropertyCmd(N_PropertyGpuProgram_VertexUnit, _T("includes_morph_animation"),
            _T("Whether this vertex program includes morph animation"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->isVertexInterpolation(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<GpuProgram *>(own)->setVertexInterpolation(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const GpuProgram *>(own)->isVertexInterpolation();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<GpuProgram *>(own)->setVertexInterpolation(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMorph();
        }
    };

    class _EngineInner CmdPose : public PropertyCmd
    {
    public:
        CmdPose() : PropertyCmd(N_PropertyGpuProgram_VertexOffset, _T("includes_pose_animation"),
            _T("The number of poses this vertex program supports for pose animation"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->getVertexOffsetCount(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<GpuProgram *>(own)->setVertexOffsetCount((Nui16)temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const GpuProgram *>(own)->getVertexOffsetCount();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<GpuProgram *>(own)->setVertexOffsetCount((Nui16)*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPose();
        }
    };

    class _EngineInner CmdVTF : public PropertyCmd
    {
    public:
        CmdVTF() : PropertyCmd(N_PropertyGpuProgram_VerTexFetch, _T("uses_vertex_texture_fetch"),
            _T("Whether this vertex program requires vertex texture fetch support."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->isVertexTextureFetch(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<GpuProgram *>(own)->setVertexTextureFetch(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const GpuProgram *>(own)->isVertexTextureFetch();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<GpuProgram *>(own)->setVertexTextureFetch(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVTF();
        }
    };

    class _EngineInner CmdManualNamedConstsFile : public PropertyCmd
    {
    public:
        CmdManualNamedConstsFile() : PropertyCmd(N_PropertyGpuProgram_ManualNamedConstant, _T("manual_named_constants"),
            _T("File containing named parameter mappings for low-level programs."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const GpuProgram *>(own)->getParamDefSrc();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<GpuProgram *>(own)->setParamDefSrc(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const GpuProgram *>(own)->getParamDefSrc();;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            String * temp = (String *)in;
            static_cast<GpuProgram *>(own)->setParamDefSrc(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdManualNamedConstsFile();
        }
    };

    class _EngineInner CmdAdjacency : public PropertyCmd
    {
    public:
        CmdAdjacency() : PropertyCmd(N_PropertyGpuProgram_Adjacency, _T("uses_adjacency_information"),
            _T("Whether this geometry program requires adjacency information from the input primitives."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GpuProgram *>(own)->isAdjacencyPrimitive(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<GpuProgram *>(own)->setAdjacencyPrimitive(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const GpuProgram *>(own)->isAdjacencyPrimitive();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<GpuProgram *>(own)->setAdjacencyPrimitive(*temp);
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAdjacency();
        }
    };
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GpuProgram
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GpuProgram::GpuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * rl, ResResultScheme * rs, ScriptTypeID stid, LangID lid) :
            Resource(stid, rid, gid, rl, rs, lid),
            mType(ST_VS),
            mFusionCount(0),
            mErrorCode(0),
            mSourceHash(0),
            mProgramSrcValid(true),
            mVertexMatrixValid(false),
            mVertexInterpolation(false),
            mVertexTextureFetch(false),
            mAdjacencyPrimitive(false),
            mParamValid(false)
    {
        mCreator = N_OnlyPtr(GpuProgram);
        createParamMap();
    }
    //-----------------------------------------------------------------------------
    GpuProgram::~GpuProgram()
    {
        if(mParams)
        {
            N_delete mParams;
            mParams = 0;
        }
        if(mDefines != 0)
        {
            N_delete mDefines;
            mDefines = 0;
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setType(ShaderType t)
    {
        mType = t;
    }
    //-----------------------------------------------------------------------------
    GpuProgram::ShaderType GpuProgram::getType() const
    {
        return mType;
    }
    //-----------------------------------------------------------------------------
    GpuProgram * GpuProgram::getMain()
    {
        return this;
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setProgramSrc(const String & filename)
    {
        mFile = filename;
        mSource.clear();
        mProgramSrcValid = true;
        mErrorCode = 0;
    }
    //-------------------------------------------------------------------------
    ShaderLanguage GpuProgram::getLanguage() const
    {
        return SL_ASM;
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::setProgramCode(const VString & source)
    {
        mSource = source;
        mFile.clear();
        mSourceHash = FastHash((const Nui8 *)mSource.c_str(), mSource.size());
        mProgramSrcValid = false;
        mErrorCode = 0;
    }
    //-----------------------------------------------------------------------------
    NCount GpuProgram::calcSize() const
    {
        return 0;
    }
    //-----------------------------------------------------------------------------
    void GpuProgram::loadImpl()
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
            loadCodeImpl(mSource);

            if(mParams != 0)
            {
                GpuProgramParam * savedParams = mParams;
                mParams = createParam();
                mParams->copyDefineValue(*savedParams);
                N_delete savedParams;
            }
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
    bool GpuProgram::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdType());
        dest->add(N_new CmdSyntax());
        dest->add(N_new CmdSkeletal());
        dest->add(N_new CmdMorph());
        dest->add(N_new CmdPose());
        dest->add(N_new CmdVTF());
        dest->add(N_new CmdManualNamedConstsFile());
        dest->add(N_new CmdAdjacency());
        return true;
    }
    //-----------------------------------------------------------------------------
    bool GpuProgram::isSysSupport() const
    {
        const RenderFeature * rsf = N_Engine().getRender()->getFeature();

        // 如果骨骼动画正在做，我们需要支持UBYTE4
        if(isVertexMatrixFusion() && !rsf->isSupport(GF_Vertex_UByte4))
        {
            return false;
        }

        // 顶点纹理拾获需要?
        if(isVertexTextureFetch() && !rsf->isSupport(GF_Program_VertexTexFetch))
        {
            return false;
        }

        return true;
    }
    //----------------------------------------------------------------------
    bool GpuProgram::isValid() const
    {
        if((mErrorCode != 0) || !isSysSupport())
            return false;

        return N_Only(GpuProgram).isSupport(mSyntax);
    }
    //---------------------------------------------------------------------
    void GpuProgram::createParamMap()
    {
        if(mDefines != 0)
            N_delete mDefines;
        mDefines = N_new GpuParamDefine();
    }
    //---------------------------------------------------------------------
    void GpuProgram::setParamDefSrc(const String & file)
    {
        mParamDefFile = file;
        mParamValid = false;
    }
    //---------------------------------------------------------------------
    Nui32 GpuProgram::getHash(Nui32 in) const
    {
        if(in)
        {
            return FastHash((const Nui8 *)mSource.c_str(), mSource.size(), in);
        }

        return mSourceHash;
    }
    //---------------------------------------------------------------------
    void GpuProgram::setParamDef(const GpuParamDefine & defs)
    {
        createParamMap();
        *mDefines = defs;
    }
    //------------------------------------------------------------------------
    const GpuParamDefine & GpuProgram::getParamDef() const
    {
        return *mDefines;
    }
    //------------------------------------------------------------------------
    GpuProgramParam * GpuProgram::createParam()
    {
        // 默认实现返回标准参数
        GpuProgramParam * ret = N_new GpuProgramParam();

        // 随意地加载，手动提供命名常量
        if(!mParamDefFile.empty() && !mParamValid)
        {
            DataStream * stream = 0;
            try
            {
                GpuParamDefine defs;
                stream = N_Only(ResourceScheme).open(mParamDefFile, mGroup, true, this);
                defs.load(stream);
                setParamDef(defs);
                N_delete stream;
            }
            catch(const Exception & e)
            {
                if(stream != 0)
                    N_delete stream;
                N_delete ret;
                N_Only(Log).stream() << _I18n("Unable to load manual named constants for GpuProgram ")
                    << N_conv(mID) << ": " << e.getVerbose();
            }
            mParamValid = true;
        }

        // 设置命名参数，如果有的话
        if((mDefines != 0) && !mDefines->getList().empty())
        {
            ret->setDefine(mDefines);
        }

        // 复制默认参数如果不是空的，复制内容
        if(mParams != 0)
            ret->copyParamValue(*mParams);

        return ret;
    }
    //-------------------------------------------------------------------------
    GpuProgramParam * GpuProgram::createDefaultParam()
    {
        if(mParams == 0)
        {
            mParams = createParam();
        }
        return mParams;
    }
    //-------------------------------------------------------------------------
    GpuProgramParam * GpuProgram::getDefaultParam() const
    {
        return mParams;
    }
    //-------------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * GpuProgram::createTimeParam
        (GpuProgramParam * params, NCount paramIndex, NIIf timeFactor)
    {
        DataValue<NIIf> * val = N_new FloatGpuParameterControllerValue(true, params, paramIndex);
        DataFunc<TimeDurMS, NIIf> * func = N_new ScaleControllerFunction(true, timeFactor);

        return N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isEnvColourEnable() const
    {
        return false;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isEnvLightEnable() const
    {
        return false;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isEnvFogEnable() const
    {
        return true;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isEnvSpaceEnable() const
    {
        return false;
    }
    //-------------------------------------------------------------------------
    void GpuProgram::setVertexMatrixFusion(bool b)
    {
        mVertexMatrixValid = b;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isVertexMatrixFusion() const
    {
        return mVertexMatrixValid;
    }
    //-------------------------------------------------------------------------
    void GpuProgram::setVertexInterpolation(bool b)
    {
        mVertexInterpolation = b;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isVertexInterpolation() const
    {
        return mVertexInterpolation;
    }
    //-------------------------------------------------------------------------
    void GpuProgram::setVertexOffsetCount(NCount count)
    {
        mFusionCount = count;
    }
    //-------------------------------------------------------------------------
    NCount GpuProgram::getVertexOffsetCount() const
    {
        return mFusionCount;
    }
    //-------------------------------------------------------------------------
    void GpuProgram::setVertexTextureFetch(bool b)
    {
        mVertexTextureFetch = b;
    }
    //-------------------------------------------------------------------------
    bool GpuProgram::isVertexTextureFetch() const
    {
        return mVertexTextureFetch;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // HighLevelGpuProgram
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    HighLevelGpuProgram::HighLevelGpuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, ScriptTypeID stid, LangID lid) :
            GpuProgram(rid, gid, ls, rs, stid, lid),
            mAssemblerCode(0),
            mSysCodeValid(false),
            mGenParamDef(false)
    {
        mCreator = N_OnlyPtr(HighLevelGpuProgram);
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::loadImpl()
    {
        if(isValid())
        {
            loadSysCode();

            genAssemblerImpl();
            // load constructed assembler program (if it exists)
            if(!mAssemblerCode == 0 && mAssemblerCode != this)
            {
                mAssemblerCode->ref();
            }
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::unloadImpl()
    {
        if(mAssemblerCode != 0 && mAssemblerCode != this)
        {
            mAssemblerCode->getCreator()->remove(mAssemblerCode->getPrc());
            mAssemblerCode->unref();
            mAssemblerCode = 0;
        }
        unloadSysCode();
        mErrorCode = false;
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgram::~HighLevelGpuProgram()
    {
        // superclasses will trigger unload
    }
    //---------------------------------------------------------------------------
    GpuProgramParam * HighLevelGpuProgram::createParam()
    {
        // Lock mutex before allowing this since this is a top-level method
        // called outside of the load()
        N_mutex1_lock

        // Make sure param defs are loaded
        GpuProgramParam * params = N_new GpuProgramParam();
        // Only populate named parameters if we can support this program
        if(isValid())
        {
            loadSysCode();
            // Errors during load may have prevented compile
            if(isValid())
            {
                configParamImpl(params);
            }
        }
        // Copy in default parameters if present
        if(mParams != 0)
            params->copyParamValue(*mParams);
        return params;
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::loadSysCode(void)
    {
        if(!mSysCodeValid)
        {
            try
            {
                loadSysCodeImpl();
                mSysCodeValid = true;
                if(mParams != 0)
                {
                    // Keep a reference to old ones to copy
                    GpuProgramParam * savedParams = mParams;
                    // Create new params
                    mParams = createParam();
                    // Copy old (matching) values across
                    // Don't use copyParamValue since program may be different
                    mParams->copyDefineValue(*savedParams);
                    N_delete savedParams;
                }
            }
            catch(const Exception & e)
            {
                // will already have been logged
                N_Only(Log).stream() << _I18n("High-level program ") << mOriginID
                    << _I18n("encountered an error during loading and is thus not supported.\n")
                        << e.getErrorVerbose();

                mErrorCode = true;
            }
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::unloadSysCode()
    {
        if(mSysCodeValid)
        {
            unloadSysCodeImpl();
            // Clear saved constant defs
            mGenParamDef = false;
            createParamMap();

            mSysCodeValid = false;
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::loadSysCodeImpl()
    {
        if(mProgramSrcValid)
        {
            // find & load source code
            DataStream * stream = N_Only(ResourceScheme).open(mFile, mGroup, true, this);

            Nui8 * data;
            NCount dsize = stream->cloneData(data);
            mSource.assign((char *)data, dsize);
            N_free(data);

            N_delete stream;
        }
        loadCodeImpl(mSource);
    }
    //---------------------------------------------------------------------
    const GpuParamDefine & HighLevelGpuProgram::getParamDef() const
    {
        if(!mGenParamDef)
        {
            HighLevelGpuProgram * temp = const_cast<HighLevelGpuProgram *>(this);
            temp->genParamDefImpl();
            temp->mGenParamDef = true;
        }
        return *mDefines;
    }
    //---------------------------------------------------------------------
    void HighLevelGpuProgram::configParamImpl(GpuProgramParam * params)
    {
        getParamDef();
        params->setDefine(mDefines);
    }
    //---------------------------------------------------------------------
    GpuProgram * HighLevelGpuProgram::getMain()
    {
        return mAssemblerCode;
    }
    //---------------------------------------------------------------------
    std::vector<std::pair<const char*, const char*>> HighLevelGpuProgram::parseDefines(VString & defines)
    {
        std::vector<std::pair<const char*, const char*>> ret;
        if (defines.empty())
            return ret;

        String::size_type pos = 0;
        while (pos != String::npos)
        {
            // Find delims
            String::size_type endPos = defines.find_first_of(";,=", pos);
            if (endPos != String::npos)
            {
                String::size_type macro_name_start = pos;
                pos = endPos;

                // Check definition part
                if (defines[pos] == '=')
                {
                    // Setup null character for macro name
                    defines[pos] = '\0';
                    // set up a definition, skip delim
                    ++pos;
                    String::size_type macro_val_start = pos;

                    endPos = defines.find_first_of(";,", pos);
                    if (endPos == String::npos)
                    {
                        pos = endPos;
                    }
                    else
                    {
                        defines[endPos] = '\0';
                        pos = endPos + 1;
                    }

                    ret.push_back({ &defines[macro_name_start], &defines[macro_val_start] });
                }
                else
                {
                    // Setup null character for macro name
                    defines[pos] = '\0';
                    // No definition part, define as "1"
                    ++pos;
                    ret.push_back({ &defines[macro_name_start], "1" });
                }
            }
            else
            {
                if (pos < defines.size())
                    ret.push_back({ &defines[pos], "1" });

                pos = endPos;
            }
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    VString HighLevelGpuProgram::genRef(const VString & instr, Resource * res, const String& fileName)
    {
        VString reSource;
        String outSource;
        String inSource;
        StrConv::conv(instr, inSource);
        // output will be at least this big
        outSource.reserve(inSource.length());

        size_t startMarker = 0;
        size_t i = inSource.find(_T("#include"));

        bool supportsFilename = StrUtil::endsWith(fileName, _T("cg"));
        String lineFilename;
        lineFilename.resize(128);
        if (supportsFilename)
        {
            NIIi ren = Nsntprintf(&lineFilename[0], 128, _T(" \"%s\""), fileName.c_str());
            if (ren > 0)
                lineFilename.resize(ren);
        }
        else
        {
            lineFilename = _T(" 0");
        }
        while (i != String::npos)
        {
            size_t includePos = i;
            size_t afterIncludePos = includePos + 8;
            size_t newLineBefore = inSource.rfind(_T('\n'), includePos);

            // check we're not in a comment
            size_t lineCommentIt = inSource.rfind(_T("//"), includePos);
            if (lineCommentIt != VString::npos)
            {
                if (newLineBefore == String::npos || lineCommentIt > newLineBefore)
                {
                    // commented
                    i = inSource.find(_T("#include"), afterIncludePos);
                    continue;
                }

            }
            size_t blockCommentIt = inSource.rfind(_T("/*"), includePos);
            if (blockCommentIt != VString::npos)
            {
                size_t closeCommentIt = inSource.rfind(_T("*/"), includePos);
                if (closeCommentIt == String::npos || closeCommentIt < blockCommentIt)
                {
                    // commented
                    i = inSource.find(_T("#include"), afterIncludePos);
                    continue;
                }

            }

            // find following newline (or EOF)
            size_t newLineAfter = inSource.find(_T('\n'), afterIncludePos);
            // find include file string container
            String endDelimeter = _T("\"");
            size_t startIt = inSource.find(_T('\"'), afterIncludePos);
            if (startIt == String::npos || startIt > newLineAfter)
            {
                // try <>
                startIt = inSource.find(_T('<'), afterIncludePos);
                if (startIt == String::npos || startIt > newLineAfter)
                {
                    N_EXCEPT(Internal, _T("Badly formed #include directive (expected \" or <) in file ") + fileName + 
                        _T(": ") + inSource.substr(includePos, newLineAfter - includePos));
                }
                else
                {
                    endDelimeter = _T(">");
                }
            }
            size_t endIt = inSource.find(endDelimeter, startIt + 1);
            if (endIt == String::npos || endIt <= startIt)
            {
                N_EXCEPT(Internal, _T("Badly formed #include directive (expected ") + endDelimeter + _T(") in file ") +
                    fileName + _T(": ") + inSource.substr(includePos, newLineAfter - includePos));
            }

            // extract _filename
            String _filename(inSource.substr(startIt + 1, endIt - startIt - 1));

            // open included file
            DataStream * resource = N_Only(ResourceScheme).open(_filename, res->getGroup(), res);

            // replace entire include directive line
            // copy up to just before include
            if (newLineBefore != String::npos && newLineBefore >= startMarker)
                outSource.append(inSource.substr(startMarker, newLineBefore - startMarker + 1));

            size_t lineCount = 0;
            size_t lineCountPos = 0;

            // Count the line number of #include statement
            lineCountPos = outSource.find(_T('\n'));
            while (lineCountPos != String::npos)
            {
                lineCountPos = outSource.find(_T('\n'), lineCountPos + 1);
                lineCount++;
            }

            // use include _filename if supported (cg) - else use include line as id (glsl)
            String incLineFilename;

            incLineFilename.resize(128);
            if (supportsFilename)
            {
                NIIi ren = Nsntprintf(&incLineFilename[0], 128, _T(" \"%s\""), _filename.c_str());
                if (ren > 0)
                    incLineFilename.resize(ren);
            }
            else
            {
                NIIi ren = Nsntprintf(&incLineFilename[0], 128, _T(" %zu"), lineCount);
                if(ren > 0)
                    incLineFilename.resize(ren);
            }

            // Add #line to the start of the included file to correct the line count)
            outSource.append(_T("#line 1 ") + incLineFilename + _T("\n"));

            if(resource->size())
            {
                String tempst2;
                Nui8 * resdata = 0;
                NCount ressize = 0;
                resource->cloneData(resdata, ressize);
                VString tempst((char *)resdata, ressize);
                N_free(resdata);

                StrConv::conv(tempst, tempst2);
                outSource.append(tempst2);

            }
            // Add #line to the end of the included file to correct the line count
            outSource.append(_T("\n#line ") + N_conv(lineCount) + lineFilename + _T("\n"));

            startMarker = newLineAfter;

            if (startMarker != String::npos)
                i = inSource.find(_T("#include"), startMarker);
            else
                i = String::npos;

        }
        // copy any remaining characters
        outSource.append(inSource.substr(startMarker));

        StrConv::conv(outSource, reSource);
        return reSource;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    class _EngineInner CmdDelegate : public PropertyCmd
    {
    public:
        CmdDelegate() : PropertyCmd(N_PropertyAlterGpuProgram_Alter, _T("delegate"), 
            _T("Additional delegate programs containing implementations."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            (void)own;
            (void)out;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 rid;
            StrConv::conv(val, rid);
            static_cast<ProxyGpuProgram *>(own)->addCandidate(rid);
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            (void)own;
            (void)out;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<ProxyGpuProgram *>(own)->addCandidate(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDelegate();
        }
        /// @copydetails PropertyCmd::isReadable
        bool isReadable() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ProxyGpuProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ProxyGpuProgram::ProxyGpuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, LangID lid) :
            HighLevelGpuProgram(rid, gid, ls, rs, N_CmdObj_ProxyGpuProgram, lid),
            mCurrent(0)
    {
        setupCmd(N_CmdObj_ProxyGpuProgram);
    }
    //-----------------------------------------------------------------------
    ProxyGpuProgram::~ProxyGpuProgram()
    {
        if(mCurrent)
        {
            mCurrent->unref();
            mCurrent = 0;
        }
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::initCmd(PropertyCmdSet * dest)
    {
        GpuProgram::initCmd(dest);

        dest->add(N_new CmdDelegate());
        return true;
    }
    //-----------------------------------------------------------------------
    const HighLevelGpuProgram * ProxyGpuProgram::getCurrent() const
    {
        if(mCurrent == 0)
        {
            ResourceIDList::const_iterator i, end = mCandidateList.end();
            for (i = mCandidateList.begin(); i != end; ++i)
            {
                HighLevelGpuProgram * deleg = static_cast<HighLevelGpuProgram *>
                    (N_Only(HighLevelGpuProgram).get(*i));
                if(deleg != 0 && deleg->isValid())
                {
                    const_cast<ProxyGpuProgram *>(this)->mCurrent = deleg;
                    mCurrent->ref(false);
                    break;
                }
            }
        }
        return mCurrent;
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::addCandidate(ResourceID rid)
    {
        mCandidateList.push_back(rid);

        if(mCurrent)
        {
            mCurrent->unref();
            mCurrent = 0;
        }
        getCurrent();
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::removeAllCandidate()
    {
        mCandidateList.clear();
        if(mCurrent != 0)
        {
            mCurrent->unref();
            mCurrent = 0;
        }
    }
    //-----------------------------------------------------------------------
    ShaderLanguage ProxyGpuProgram::getLanguage() const
    {
        return SL_ALTER;
    }
    //-----------------------------------------------------------------------
    GpuProgramParam * ProxyGpuProgram::createParam()
    {
        if(mCurrent)
        {
            return mCurrent->createParam();
        }

        // return a default set
        GpuProgramParam * params = N_new GpuProgramParam();
        // avoid any errors on parameter names that don't exist
        params->setAllowMissing(true);
        return params;
    }
    //-----------------------------------------------------------------------
    GpuProgram * ProxyGpuProgram::getMain()
    {
        if(mCurrent != 0)
            return mCurrent->getMain();

        return 0;
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isValid() const
    {
        // Supported if one of the delegates is
        return mCurrent != 0;
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isVertexMatrixFusion() const
    {
        if(mCurrent != 0)
            return mCurrent->isVertexMatrixFusion();

        return false;
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isVertexInterpolation() const
    {
        if(mCurrent != 0)
            return mCurrent->isVertexInterpolation();

        return false;
    }
    //-----------------------------------------------------------------------
    NCount ProxyGpuProgram::getVertexOffsetCount() const
    {
        if(mCurrent != 0)
            return mCurrent->getVertexOffsetCount();

        return 0;
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isVertexTextureFetch() const
    {
        if(mCurrent != 0)
            return mCurrent->isVertexTextureFetch();

        return false;
    }
    //-----------------------------------------------------------------------
    GpuProgramParam * ProxyGpuProgram::createDefaultParam()
    {
        if(mCurrent != 0)
            return mCurrent->createDefaultParam();

        return 0;
    }
    //-----------------------------------------------------------------------
    GpuProgramParam * ProxyGpuProgram::getDefaultParam() const
    {
        if(mCurrent != 0)
            return mCurrent->getDefaultParam();

        return 0;
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isEnvColourEnable() const
    {
        if(mCurrent != 0)
            return mCurrent->isEnvColourEnable();
        else
            return HighLevelGpuProgram::isEnvColourEnable();
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isEnvLightEnable() const
    {
        if(mCurrent != 0)
            return mCurrent->isEnvLightEnable();
        else
            return HighLevelGpuProgram::isEnvLightEnable();
    }
    //---------------------------------------------------------------------
    bool ProxyGpuProgram::isEnvFogEnable() const
    {
        if(mCurrent != 0)
            return mCurrent->isEnvFogEnable();
        else
            return HighLevelGpuProgram::isEnvFogEnable();
    }
    //---------------------------------------------------------------------
    bool ProxyGpuProgram::isEnvSpaceEnable() const
    {
        if(mCurrent != 0)
            return mCurrent->isEnvSpaceEnable();
        else
            return HighLevelGpuProgram::isEnvSpaceEnable();
    }
    //-----------------------------------------------------------------------
    Nui32 ProxyGpuProgram::getErrorCode() const
    {
        if(mCurrent == 0)
        {
            return 0;
        }
        else
        {
            return mCurrent->getErrorCode();
        }
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::load()
    {
        if(mCurrent != 0)
            mCurrent->load();
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::reload()
    {
        if(mCurrent != 0)
            mCurrent->reload();
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isReloadable() const
    {
        if(mCurrent != 0)
            return mCurrent->isReloadable();
        else
            return true;
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::unload()
    {
        if(mCurrent != 0)
            mCurrent->unload();
    }
    //-----------------------------------------------------------------------
    bool ProxyGpuProgram::isLoading() const
    {
        if(mCurrent != 0)
            return mCurrent->isLoading();

        return false;
    }
    //-----------------------------------------------------------------------
    LoadingState ProxyGpuProgram::getState() const
    {
        if(mCurrent != 0)
            return mCurrent->getState();

        return LS_UNLOADED;
    }
    //-----------------------------------------------------------------------
    PrcID ProxyGpuProgram::getPrc() const
    {
        if(mCurrent != 0)
            return mCurrent->getPrc();

        return 0;
    }
    //-----------------------------------------------------------------------
    ResourceID ProxyGpuProgram::getOriginID() const
    {
        if(mCurrent != 0)
            return mCurrent->getOriginID();

        return 0;
    }
    //-----------------------------------------------------------------------
    NCount ProxyGpuProgram::getSize() const
    {
        if(mCurrent != 0)
            return mCurrent->getSize();

        return 0;
    }
    //-----------------------------------------------------------------------
    NCount ProxyGpuProgram::getOriginSize() const
    {
        if(mCurrent != 0)
            return mCurrent->getOriginSize();

        return 0;
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::ref(bool doload)
    {
        if(mCurrent != 0)
            mCurrent->ref();
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::unref()
    {
        if(mCurrent != 0)
            mCurrent->unref();
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::setLoadScheme(ResLoadScheme * rls)
    {
        if(mCurrent != 0)
            mCurrent->setLoadScheme(rls);
    }
    //-----------------------------------------------------------------------
    ResLoadScheme * ProxyGpuProgram::getLoadScheme() const
    {
        if(mCurrent != 0)
            return mCurrent->getLoadScheme();

        return 0;
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::setResultScheme(ResResultScheme * rrs)
    {
        if(mCurrent != 0)
            mCurrent->setResultScheme(rrs);
    }
    //-----------------------------------------------------------------------
    ResResultScheme * ProxyGpuProgram::getResultScheme() const
    {
        if(mCurrent != 0)
            return mCurrent->getResultScheme();

        return 0;
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::add(ResourceListener * lis)
    {
        if(mCurrent != 0)
            mCurrent->add(lis);
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::remove(ResourceListener * lis)
    {
        if(mCurrent != 0)
            mCurrent->remove(lis);
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::genAssemblerImpl()
    {
        N_EXCEPT(UnImpl, _I18n("This method should never get called!"));
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::unloadSysCodeImpl()
    {
        N_EXCEPT(UnImpl, _I18n("This method should never get called!"));
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::genParamDefImpl()
    {
        N_EXCEPT(UnImpl, _I18n("This method should never get called!"));
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgram::loadCodeImpl(const VString & code)
    {
        N_EXCEPT(UnImpl, _I18n("This method should never get called!"));
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ProxyGpuProgramFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ProxyGpuProgramFactory::ProxyGpuProgramFactory(HighLevelGpuProgramManager * mag):
        HighLevelGpuProgramFactory(mag){}
    //-----------------------------------------------------------------------
    ProxyGpuProgramFactory::~ProxyGpuProgramFactory(){}
    //-----------------------------------------------------------------------
    FactoryID ProxyGpuProgramFactory::getID() const
    {
        return N_FACTORY_ProxyProgram;
    }
    //-----------------------------------------------------------------------
    ShaderLanguage ProxyGpuProgramFactory::getLanguage() const
    {
        return SL_ALTER;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram * ProxyGpuProgramFactory::create(ResourceID rid, GroupID gid)
    {
        return N_new ProxyGpuProgram(rid, gid);
    }
    //-----------------------------------------------------------------------
    void ProxyGpuProgramFactory::destroy(void * prog)
    {
        ProxyGpuProgram * temp = static_cast<ProxyGpuProgram *>(prog);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
}