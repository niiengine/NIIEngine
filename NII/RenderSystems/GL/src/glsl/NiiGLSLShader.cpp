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

#include "NiiGpuProgram.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiGpuProgramManager.h"
#include "NiiLogManager.h"

#include "NiiGLSLExtSupport.h"
#include "NiiGLSLLinkProgramManager.h"
#include "NiiGLSLShader.h"
#include "NiiGLSLGpuProgram.h"
#include "NiiGLSLPreprocessor.h"

namespace NII 
{
namespace GLSL 
{
    //-----------------------------------------------------------------------
    static GeometryRaw::OperationType parseOperationType(const String & val)
    {
        if (val == _T("point_list"))
        {
            return GeometryRaw::OT_POINT_LIST;
        }
        else if (val == _T("line_list"))
        {
            return GeometryRaw::OT_LINE_LIST;
        }
        else if (val == _T("line_list_adj"))
        {
            return GeometryRaw::OT_LINE_LIST_ADJ;
        }
        else if (val == _T("line_strip"))
        {
            return GeometryRaw::OT_LINE_STRIP;
        }
        else if (val == _T("line_strip_adj"))
        {
            return GeometryRaw::OT_LINE_STRIP_ADJ;
        }
        else if (val == _T("triangle_strip"))
        {
            return GeometryRaw::OT_TRIANGLE_STRIP;
        }
        else if (val == _T("triangle_strip_adj"))
        {
            return GeometryRaw::OT_TRIANGLE_STRIP_ADJ;
        }
        else if (val == _T("triangle_fan"))
        {
            return GeometryRaw::OT_TRIANGLE_FAN;
        }
        else if (val == _T("triangle_list_adj"))
        {
            return GeometryRaw::OT_TRIANGLE_LIST_ADJ;
        }
        else
        {
            //Triangle list is the default fallback. Keep it this way?
            return GeometryRaw::OT_TRIANGLE_LIST;
        }
    }
    //-----------------------------------------------------------------------
    static void operationTypeToString(GeometryRaw::OperationType val, String & out)
    {
        switch (val)
        {
        case GeometryRaw::OT_POINT_LIST:
            out = _T("point_list");
            break;
        case GeometryRaw::OT_LINE_LIST:
            out = _T("line_list");
            break;
        case GeometryRaw::OT_LINE_LIST_ADJ:
            out = _T("line_list_adj");
            break;
        case GeometryRaw::OT_LINE_STRIP:
            out = _T("line_strip");
            break;
        case GeometryRaw::OT_LINE_STRIP_ADJ:
            out = _T("line_strip_adj");
            break;
        case GeometryRaw::OT_TRIANGLE_STRIP:
            out = _T("triangle_strip");
            break;
        case GeometryRaw::OT_TRIANGLE_STRIP_ADJ:
            out = _T("triangle_strip_adj");
            break;
        case GeometryRaw::OT_TRIANGLE_FAN:
            out = _T("triangle_fan");
            break;
        case GeometryRaw::OT_TRIANGLE_LIST_ADJ:
            out = _T("triangle_list_adj");
            break;
        case GeometryRaw::OT_TRIANGLE_LIST:
        default:
            out = _T("triangle_list");
            break;
        }
    }
    /// Command object for setting the input operation type (geometry shader only)
    class CmdInputOperationType : public PropertyCmd
    {
    public:
        CmdInputOperationType() : PropertyCmd(N_PropertyGpuProgramGLSL_InputOpType, _T("input_operation_type"),
            _T("The input operation type for this geometry program. Can be 'point_list', 'line_list',\
                'line_strip', 'triangle_list', 'triangle_strip' or 'triangle_fan'"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const GLSLShader* t = static_cast<const GLSLShader*>(own);
            operationTypeToString(t->getInputOperationType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            GLSLShader* t = static_cast<GLSLShader*>(own);
            t->setInputOperationType(parseOperationType(val));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const GLSLShader*>(own)->getInputOperationType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<GLSLShader*>(own)->setInputOperationType((GeometryRaw::OperationType)*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdInputOperationType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("triangle_list");
        }
    };
    /// Command object for setting the output operation type (geometry shader only)
    class CmdOutputOperationType : public PropertyCmd
    {
    public:
        CmdOutputOperationType() : PropertyCmd(N_PropertyGpuProgramGLSL_OutOpType, _T("output_operation_type"),
            _T("The input operation type for this geometry program. Can be 'point_list', 'line_strip' or 'triangle_strip'"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const GLSLShader* t = static_cast<const GLSLShader*>(own);
            operationTypeToString(t->getOutputOperationType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            GLSLShader* t = static_cast<GLSLShader*>(own);
            t->setOutputOperationType(parseOperationType(val));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const GLSLShader*>(own)->getOutputOperationType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<GLSLShader*>(own)->setOutputOperationType((GeometryRaw::OperationType)*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdOutputOperationType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("triangle_list");
        }
    };
    /// Command object for setting the maximum output vertices (geometry shader only)
    class CmdMaxOutputVertices : public PropertyCmd
    {
    public:
        CmdMaxOutputVertices() : PropertyCmd(N_PropertyParticleCtrlGeo_CtrlUp, _T("max_output_vertices"),
            _T("The maximum number of vertices a single run of this geometry program can output"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const GLSLShader* t = static_cast<const GLSLShader*>(own);
            StrConv::conv(t->getMaxOutputVertices(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            GLSLShader* t = static_cast<GLSLShader*>(own);
            NIIi temp;
            StrConv::conv(val, temp);
            t->setMaxOutputVertices(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIi * temp = (NIIi *)out;
            *temp = static_cast<const GLSLShader *>(own)->getMaxOutputVertices();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIi * temp = (NIIi *)in;
            static_cast<GLSLShader *>(own)->setMaxOutputVertices(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxOutputVertices();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = String(_T("3"));
        }
    };
    static CmdMaxOutputVertices msMaxOutputVerticesCmd;
    //-----------------------------------------------------------------------
    GLSLShader::GLSLShader(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs)
        : GLSLShaderBase(rid, gid, ls, rs)
        , mInputOperationType(GeometryRaw::OT_TRIANGLE_LIST)
        , mOutputOperationType(GeometryRaw::OT_TRIANGLE_LIST)
        , mMaxOutputVertices(3)
        , mGLShaderHandle(0)
        , mGLProgramHandle(0)
    {
        mCreator = N_OnlyPtr(HighLevelGpuProgram);
        // add parameter command "attach" to the material serializer dictionary
        setupCmd(N_CmdObj_GLSLProgram);

        mType = ST_VS; // default value, to be corrected after the constructor with GpuProgram::setType()
        // Manually assign language now since we use it immediately
        mSyntax = SL_GLSL;
        // There is nothing to load
        mProgramSrcValid = false;
    }
    //---------------------------------------------------------------------------
    GLSLShader::~GLSLShader()
    {
        // Have to call this here rather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if (getState() == LS_LOADED)
        {
            unload();
        }
        else
        {
            unloadSysCode();
        }
    }
    //---------------------------------------------------------------------------
    bool GLSLShader::compile(bool checkErrors)
    {
        // only create a shader object if glsl is supported
        if (isValid())
        {
            // Create shader object.
            GLenum GLShaderType = 0;
            
            switch (mType)
            {
            case ST_VS:
                GLShaderType = GL_VERTEX_SHADER;
            case ST_TS:
                GLShaderType = GL_TESS_CONTROL_SHADER;
            case ST_DS:
                GLShaderType = GL_TESS_EVALUATION_SHADER;
            case ST_GS:
                GLShaderType = GL_GEOMETRY_SHADER;
            case ST_FS:
                GLShaderType = GL_FRAGMENT_SHADER;
            case ST_CS:
                GLShaderType = GL_COMPUTE_SHADER;
            }
            
            GLCheck(mGLShaderHandle = glCreateShader(GLShaderType));
        }

        //TODO GL 4.3 KHR_debug

        // if (getGLSupport()->checkGpuApi("GL_KHR_debug") || gl3wIsSupported(4, 3))
        //     glObjectLabel(GL_SHADER, mGLShaderHandle, 0, mName.c_str());

        // if (N_Engine().getRender()->getFeature()->isSupport(GF_GLSL_SeparateShaderObject))
        // {
        //     GLCheck(mGLProgramHandle = glCreateProgram());
        //     if(getGLSupport()->checkGpuApi("GL_KHR_debug") || gl3wIsSupported(4, 3))
        //         glObjectLabel(GL_PROGRAM, mGLProgramHandle, 0, mName.c_str());
        // }

        submitSource();

        // Add preprocessor extras and main source
        if (!mSource.empty())
        {
            const char *source = mSource.c_str();
            glShaderSource(mGLShaderHandle, 1, &source, NULL);
        }

        GLCheck(glCompileShader(mGLShaderHandle));

        // Check for compile errors
        int compiled = 0;
        GLCheck(glGetShaderiv(mGLShaderHandle, GL_COMPILE_STATUS, &compiled));

        if(!checkErrors)
            return compiled == 1;

        String compileInfo = getObjectInfo(mGLShaderHandle);

        if (!compiled)
            N_EXCEPT(RenderingAPI, getResourceLogName() + " " + compileInfo);

        // probably we have warnings
        if (!compileInfo.empty())
            N_Only(Log).stream(LL_Info) << getResourceLogName() << " " << compileInfo;

        return compiled == 1;
    }
    //-----------------------------------------------------------------------
    void GLSLShader::genAssemblerImpl()
    {
        // mAssemblerCode = N_new GLSLGpuProgram( this );
        // Shader params need to be forwarded to low level implementation
        // mAssemblerCode->setAdjacencyPrimitive(isAdjacencyPrimitive());
        // mAssemblerCode->setComputeDim(getComputeDim());
    }
    //-----------------------------------------------------------------------
    void GLSLShader::unloadSysCodeImpl()
    {
        if (isValid())
        {
            GLCheck(glDeleteShader(mGLShaderHandle));    

            if (N_Engine().getRender()->getFeature()->isSupport(GF_GLSL_SeparateShaderObject) && mGLProgramHandle)
            {
                GLCheck(glDeleteProgram(mGLProgramHandle));
                mGLProgramHandle = 0;
            }

            // destroy all programs using this shader
            N_Only(GLSL::GLSLProgram).destroyAllByShader(this);
            
            mGLShaderHandle = 0;
            mLinked = 0;
        }
    }
    //-----------------------------------------------------------------------
    void GLSLShader::configParamImpl(GpuProgramParam * params)
    {
        getParamDef();
        params->setDefine(mDefines);
        // Don't set logical / physical maps here, as we can't access parameters by logical index in GLHL.
    }
    //-----------------------------------------------------------------------
    void GLSLShader::submitSource()
    {
        if (mSource.empty())
            return;

        const RenderFeature * rsc = N_Engine().getRender()->getFeature();

        bool clipDistBug = ((N_PLAT == N_PLAT_WIN32) || (N_PLAT == N_PLAT_WINRT)) && rsc->getVendor() == GT_Intel;

        size_t versionPos = mSource.find("#version");
        int shaderVersion = 100;
        size_t belowVersionPos = 0;

        if(versionPos != String::npos)
        {
            StrConv::conv(mSource.substr(versionPos + 9, 3), shaderVersion);
            belowVersionPos = mSource.find('\n', versionPos) + 1;
        }

        // OSX driver only supports glsl150+ in core profile
        bool shouldUpgradeToVersion150 = !rsc->isSupport("glsl130") && shaderVersion < 150;

        // Add standard shader input and output blocks, if missing.
        // Assume blocks are missing if gl_Position is missing.
        if (0 &&  rsc->isSupport(GF_GLSL_SeparateShaderObject) && (mSource.find("vec4 gl_Position") == VString::npos))
        {
            size_t mainPos = mSource.find("void main");
            // Only add blocks if shader is not a child
            // shader, i.e. has a main function.
            if (mainPos != VString::npos)
            {
                String clipDistDecl = clipDistBug ? "float gl_ClipDistance[1];" : "float gl_ClipDistance[];";
                String inBlock = "in gl_PerVertex\n{\nvec4 gl_Position;\nfloat gl_PointSize;\n"+clipDistDecl+"\n} gl_in[];\n\n";
                String outBlock = "out gl_PerVertex\n{\nvec4 gl_Position;\nfloat gl_PointSize;\n"+clipDistDecl+"\n};\n\n";

                if (shaderVersion >= 150 || shouldUpgradeToVersion150)
                {
                    switch (mType)
                    {
                    case ST_VS:
                        mSource.insert(belowVersionPos, outBlock);
                        break;
                    case ST_GS:
                        mSource.insert(belowVersionPos, outBlock);
                        mSource.insert(belowVersionPos, inBlock);
                        break;
                    case ST_DS:
                        mSource.insert(belowVersionPos, outBlock);
                        mSource.insert(belowVersionPos, inBlock);
                        break;
                    case ST_TS:
                        mSource.insert(belowVersionPos, outBlock.substr(0, outBlock.size() - 3) + " gl_out[];\n\n");
                        mSource.insert(belowVersionPos, inBlock);
                        break;
                    case ST_FS:
                    case ST_CS:
                        // Fragment and compute shaders do
                        // not have standard blocks.
                        break;
                    }
                }
                else if(mType == ST_VS && shaderVersion >= 130) // shaderVersion < 150, means we only have vertex shaders
                {
                	// TODO: can we have SSO with GLSL < 130?
                    mSource.insert(belowVersionPos, "out vec4 gl_Position;\nout float gl_PointSize;\nout "+clipDistDecl+"\n\n");
                }
            }
        }

        if(shouldUpgradeToVersion150)
        {
            if(belowVersionPos != 0)
                mSource = mSource.erase(0, belowVersionPos); // drop old definition

            // automatically upgrade to glsl150. thank you apple.
            const char* prefixFp =
                    "#version 150\n"
                    "#define varying in\n"
                    "#define texture1D texture\n"
                    "#define texture2D texture\n"
                    "#define texture3D texture\n"
                    "#define textureCube texture\n"
                    "#define texture2DLod textureLod\n"
                    "#define textureCubeLod textureLod\n"
                    "#define shadow2DProj textureProj\n"
                    "#define gl_FragColor FragColor\n"
                    "out vec4 FragColor;\n";
            const char* prefixVp =
                    "#version 150\n"
                    "#define attribute in\n"
                    "#define varying out\n";

            mSource.insert(0, mType == GPT_FRAGMENT_PROGRAM ? prefixFp : prefixVp);
        }

        // Submit shader source.
        const char *source = mSource.c_str();
        GLCheck(glShaderSource(mGLShaderHandle, 1, &source, NULL));
    }
    //-----------------------------------------------------------------------
    void GLSLShader::genParamDefImpl()
    {
        // We need an accurate list of all the uniforms in the shader, but we
        // can't get at them until we link all the shaders into a program object.


        // Therefore instead, parse the source code manually and extract the uniforms
        createParamMap();
        N_Only(GLSL::GLSLProgram).extractUniformsFromGLSL(mSource, *mDefines, mSrc);

        // Also parse any attached sources
        for (GLSLProgramContainer::const_iterator i = mAttachedGLSLPrograms.begin();
            i != mAttachedGLSLPrograms.end(); ++i)
        {
            GLSLShaderBase * childShader = *i;

            N_Only(GLSL::GLSLProgram).extractUniformsFromGLSL(
                childShader->getProgramCode(), *mDefines, childShader->getSrc());

        }
    }
    //-----------------------------------------------------------------------
    void GLSLShader::attachToProgramObject(GLuint programObject )
    {
        // attach child objects
        for (auto childShader : mAttachedGLSLPrograms)
        {
            childShader->attachToProgramObject(programObject);
        }
        GLCheck(glAttachShader( programObject, mGLShaderHandle ));
    }
    //-----------------------------------------------------------------------
    void GLSLShader::detachFromProgramObject(GLuint programObject)
    {
        GLCheck(glDetachShader(programObject, mGLShaderHandle));
        // attach child objects
        GLSLProgramContainerIterator childShader = mAttachedGLSLPrograms.begin();
        GLSLProgramContainerIterator childShaderend = mAttachedGLSLPrograms.end();

        while (childShader != childShaderend)
        {
            (*childShader)->detachFromProgramObject( programObject );
            ++childShader;
        }
    }
    //-----------------------------------------------------------------------
    GLuint GLSLShader::getGLProgramHandle() {
        //TODO This should be removed and the compile() function
        // should use glCreateShaderProgramv
        // for separable programs which includes creating a program.
        if (mGLProgramHandle == 0)
        {
            GLCheck(mGLProgramHandle = glCreateProgram());
        }
        return mGLProgramHandle;
    }
    //-----------------------------------------------------------------------
    void GLSLShader::bind(void)
    {
        // Tell the Program Manager what shader is to become active.
        switch (mType)
        {
        case GPT_VERTEX_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveVertexShader(this);
            break;
        case GPT_FRAGMENT_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveFragmentShader(this);
            break;
        case GPT_GEOMETRY_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveGeometryShader(this);
            break;
        case GPT_HULL_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveHullShader(this);
            break;
        case GPT_DOMAIN_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveDomainShader(this);
            break;
        case GPT_COMPUTE_PROGRAM:
            N_Only(GLSL::GLSLProgram).setActiveComputeShader(this);
            break;
        }
    }
    //-----------------------------------------------------------------------
    void GLSLShader::unbind(void)
    {
        // Tell the Link Program Manager what shader is to become inactive.
        if (mType == GPT_VERTEX_PROGRAM)
        {
            N_Only(GLSL::GLSLProgram).setActiveVertexShader(NULL);
        }
        else if (mType == GPT_GEOMETRY_PROGRAM)
        {
            N_Only(GLSL::GLSLProgram).setActiveGeometryShader(NULL);
        }
        else if (mType == GPT_HULL_PROGRAM)
        {
            N_Only(GLSL::GLSLProgram).setActiveHullShader(NULL);
        }
        else if (mType == GPT_DOMAIN_PROGRAM)
        {
            N_Only(GLSL::GLSLProgram).setActiveDomainShader(NULL);
        }
        else if (mType == GPT_COMPUTE_PROGRAM)
        {
            N_Only(GLSL::GLSLProgram).setActiveComputeShader(NULL);
        }
        else // It's a fragment shader
        {
            N_Only(GLSL::GLSLProgram).setActiveFragmentShader(NULL);
        }
    }
    //-----------------------------------------------------------------------
    void GLSLShader::bindParameters(GpuProgramParam * params, uint16 mask)
    {
        // Link can throw exceptions, ignore them at this point.
        try
        {
            // Activate the program pipeline object.
            GLSLProgram* program = N_Only(GLSL::GLSLProgram).getActiveProgram();
            // Pass on parameters from params to program object uniforms.
            program->updateUniforms(params, mask, mType);
            program->updateAtomicCounters(params, mask, mType);
        }
        catch (Exception&) {}
    }
    //-----------------------------------------------------------------------
    void GLSLShader::bindPassIterationParameters(GpuProgramParam * params)
    {
        // Activate the link program object.
        GLSLProgram * program = N_Only(GLSL::GLSLProgram).getActiveProgram();
        // Pass on parameters from params to program object uniforms.
        program->updatePassIterationUniforms(params);
    }
    //-----------------------------------------------------------------------
    void GLSLShader::bindSharedParameters(GpuProgramParam * params, uint16 mask)
    {
        // Link can throw exceptions, ignore them at this point.
        try
        {
            // Activate the program pipeline object.
            GLSLProgram * program = N_Only(GLSL::GLSLProgram).getActiveProgram();
            // Pass on parameters from params to program object uniforms.
            program->updateUniformBlocks(params, mask, mType);
            // program->updateShaderStorageBlock(params, mask, mType);

        }
        catch (InvalidParametersException & e)
        {
            N_Only(Log).error(0, "binding shared parameters failed: " +  e.getDescription());
        }
        catch (Exception&) {}
    }
    //-----------------------------------------------------------------------
    NCount GLSLShader::calcSize() const
    {
        NCount memSize = 0;

        // Delegate names.
        memSize += sizeof(GLuint);
        memSize += sizeof(GLenum);
        memSize += GpuProgram::calcSize();

        return memSize;
    }
    //-----------------------------------------------------------------------
    bool GLSLShader::initCmd(PropertyCmdSet * dest)
    {
        GLSLShaderBase::initCmd(dest);
        dest->add(N_new CmdInputOperationType());
        dest->add(N_new CmdOutputOperationType());
        dest->add(N_new CmdMaxOutputVertices());
        return true;
    }
    //-----------------------------------------------------------------------
    ShaderLanguage GLSLShader::getLanguage() const
    {
        return SL_GLSL;
    }
    //-----------------------------------------------------------------------
}
}
