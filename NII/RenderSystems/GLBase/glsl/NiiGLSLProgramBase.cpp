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

#include "NiiGLSLProgramBase.h"
#include "NiiStrConv.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiGpuProgram.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiGpuProgramManager.h"
#include "NiiLogManager.h"

#include "NiiGLSLShaderBase.h"
#include "NiiGLSLPreprocessor.h"

namespace NII
{
    //-----------------------------------------------------------------------
    // Switching attribute bindings requires re-creating VAOs. So avoid!
    // Fixed builtins (from ARB_vertex_program Table X.2) are:

    //  a  builtin              custom attrib name
    // ----------------------------------------------
    //  0  gl_Vertex            vertex/ position
    //  1  n/a                  blendWeights
    //  2  gl_Normal            normal
    //  3  gl_Color             colour
    //  4  gl_SecondaryColor    secondary_colour
    //  5  gl_FogCoord          n/a
    //  6  n/a                  n/a
    //  7  n/a                  blendIndices
    //  8  gl_MultiTexCoord0    uv0
    //  9  gl_MultiTexCoord1    uv1
    //  10 gl_MultiTexCoord2    uv2
    //  11 gl_MultiTexCoord3    uv3
    //  12 gl_MultiTexCoord4    uv4
    //  13 gl_MultiTexCoord5    uv5
    //  14 gl_MultiTexCoord6    uv6, tangent
    //  15 gl_MultiTexCoord7    uv7, binormal
    GLSLProgramBase::CustomAttribute GLSLProgramBase::DefaultAttributeLayout[16] = {
        {"vertex", getFixedAttributeIndex(VT_Pos, 0), VT_Pos},
        {"blendWeights", getFixedAttributeIndex(VT_Matrix_Weight, 0), VT_Matrix_Weight},
        {"normal", getFixedAttributeIndex(VT_Normals, 0), VT_Normals},
        {"colour", getFixedAttributeIndex(VT_Diffuse, 0), VT_Diffuse},
        {"secondary_colour", getFixedAttributeIndex(VT_Specular, 0), VT_Specular},
        {"blendIndices", getFixedAttributeIndex(VT_Matrix_Index, 0), VT_Matrix_Index},
        {"uv0", getFixedAttributeIndex(VT_Tex_Coord, 0), VT_Tex_Coord},
        {"uv1", getFixedAttributeIndex(VT_Tex_Coord, 1), VT_Tex_Coord},
        {"uv2", getFixedAttributeIndex(VT_Tex_Coord, 2), VT_Tex_Coord},
        {"uv3", getFixedAttributeIndex(VT_Tex_Coord, 3), VT_Tex_Coord},
        {"uv4", getFixedAttributeIndex(VT_Tex_Coord, 4), VT_Tex_Coord},
        {"uv5", getFixedAttributeIndex(VT_Tex_Coord, 5), VT_Tex_Coord},
        {"uv6", getFixedAttributeIndex(VT_Tex_Coord, 6), VT_Tex_Coord},
        {"tangent", getFixedAttributeIndex(VT_Tangent, 0), VT_Tangent},
        {"binormal", getFixedAttributeIndex(VT_Binormal, 0), VT_Binormal},
        {"space", getFixedAttributeIndex(VT_Tex_Coord, 7), VT_Space}
    };
    //-----------------------------------------------------------------------
    static Ni32 CoustomAttributeLayout[VT_Count] = {
            0, // VT_Pos
            1, // VT_Matrix_Weight
            5, // VT_Matrix_Index
            2, // VT_Normals
            3, // VT_Diffuse
            4, // VT_Specular
            15,// VT_Space
            13, // VT_Tangent
            14,// VT_Binormal
            6, // VT_Tex_Coord
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLSLProgramBase
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLSLProgramBase::GLSLProgramBase(GLSLShaderBase * vs) : 
        mVertexShader(vs),
        mBaseInstanceLocation( GL_INVALID_INDEX ),
        mUniformRefsBuilt(false),
        mGLProgramHandle(0),
        mLinked(false),
        mSkeletalAnimation(false)
    {
        // init mCustomAttributesIndexes
        for (size_t i = 0; i < VT_Custom8; i++)
        {
            for (size_t j = 0; j < NII_MAX_TextureCoord; j++)
            {
                mCustomAttributesIndexes[i][j] = NULL_CUSTOM_ATTRIBUTES_INDEX;
            }
        }
    }
    //-----------------------------------------------------------------------
    const char* GLSLProgramBase::getAttributeSemanticString(VertexType vtype)
    {
        if(vtype == VT_Tex_Coord)
            return "uv";

        size_t numAttribs = sizeof(DefaultAttributeLayout)/sizeof(CustomAttribute);
        for (size_t i = 0; i < numAttribs; ++i)
        {
            if (DefaultAttributeLayout[i].semantic == vtype)
                return DefaultAttributeLayout[i].name;
        }

        N_assert(false, "Missing attribute!");
        return 0;
    }
    //-----------------------------------------------------------------------
    void GLSLProgramBase::extractLayoutQualifiers(void)
    {
        // Format is:
        //      layout(location = 0) attribute vec4 vertex;

        if (!mVertexShader)
        {
            return;
        }

        const VString & shaderSource = mVertexShader->getProgramCode();
        VString::size_type currPos = shaderSource.find("layout");
        while (currPos != String::npos)
        {
            VertexType vtype;
            int index = 0;

            VString::size_type endPos = shaderSource.find(';', currPos);
            if (endPos == String::npos)
            {
                // Problem, missing semicolon, abort.
                break;
            }

            VString line = shaderSource.substr(currPos, endPos - currPos);

            // Skip over 'layout'.
            currPos += 6;

            // Skip until '='.
            VString::size_type eqPos = line.find('=');
            VString::size_type parenPos = line.find(')');

            // Skip past '=' up to a ')' which contains an integer(the position).
            // TODO This could be a definition, does the preprocessor do replacement?
            VString attrLocation = line.substr(eqPos + 1, parenPos - eqPos - 1);
            StrUtil::trim(attrLocation);
            int attrib;
            StrConv::conv(attrLocation, attrib);

            // The rest of the line is a standard attribute definition.
            // Erase up to it then split the remainder by spaces.
            line.erase(0, parenPos + 1);
            StrUtil::trim(line);
            VStringList parts;
            StrUtil::split(line, parts, " ");

            if (parts.size() < 3)
            {
                // This is a malformed attribute.
                // It should contain 3 parts, i.e. "attribute vec4 vertex".
                break;
            }
            size_t attrStart = 0;
            if (parts.size() == 4)
            {
                if( parts[0] == "flat" || parts[0] == "smooth"|| parts[0] == "perspective")
                {
                    // Skip the interpolation qualifier
                    attrStart = 1;
                }
            }

            // Skip output attribute
            if (parts[attrStart] != "out")
            {
                VString attrName = parts[attrStart + 2];
                VString::size_type uvPos = attrName.find("uv");

                if(uvPos == 0)
                    vtype = getAttributeSemanticEnum("uv0"); // treat "uvXY" as "uv0"
                else if(attrName == "position")
                    vtype = getAttributeSemanticEnum("vertex");
                else
                    vtype = getAttributeSemanticEnum(attrName);

                // Find the texture unit index.
                if (uvPos == 0)
                {
                    VString uvIndex = attrName.substr(uvPos + 2, attrName.length() - 2);
                    StrConv::conv(uvIndex, index);
                }

                mCustomAttributesIndexes[vtype][index] = attrib;
            }

            currPos = shaderSource.find("layout", currPos);
        }
    }
    //-----------------------------------------------------------------------
    GLint GLSLProgramBase::getAttributeIndex(VertexType vtype, NIIi index)
    {
        GLint res = mCustomAttributesIndexes[vtype][index];
        if (res == NULL_CUSTOM_ATTRIBUTES_INDEX)
        {
            const char * attString = getAttributeSemanticString(vtype);
            GLint attrib;
            GLCheck(attrib = glGetAttribLocation(mGLProgramHandle, attString));

            // Sadly position is a special case.
            if (attrib == NOT_FOUND_CUSTOM_ATTRIBUTES_INDEX && vtype == VES_POSITION)
            {
                GLCheck(attrib = glGetAttribLocation(mGLProgramHandle, "position"));
            }

            // For uv and other case the index is a part of the name.
            if (attrib == NOT_FOUND_CUSTOM_ATTRIBUTES_INDEX)
            {
                String type = VString(attString) + N_convV(index);
                GLCheck(attrib = glGetAttribLocation(mGLProgramHandle, type.c_str()));
            }

            // Update mCustomAttributesIndexes with the index we found (or didn't find).
            mCustomAttributesIndexes[vtype][index] = attrib;
            res = attrib;
        }
        return res;
    }
    //-----------------------------------------------------------------------
    bool GLSLProgramBase::isAttributeValid(VertexType semantic, NIIi index)
    {
        return getAttributeIndex(semantic, index) != NOT_FOUND_CUSTOM_ATTRIBUTES_INDEX;
    }
    //-----------------------------------------------------------------------
    void GLSLProgramBase::setTightAttLayout() {
        //  a  builtin              custom attrib name
        // ----------------------------------------------
        //  0  gl_Vertex            vertex/ position
        //  1  gl_Normal            normal
        //  2  gl_Color             colour
        //  3  gl_MultiTexCoord0    uv0
        //  4  gl_MultiTexCoord1    uv1, blendWeights
        //  5  gl_MultiTexCoord2    uv2, blendIndices
        //  6  gl_MultiTexCoord3    uv3, tangent
        //  7  gl_MultiTexCoord4    uv4, binormal

        size_t numAttribs = sizeof(DefaultAttributeLayout) / sizeof(CustomAttribute);
        for (size_t i = 0; i < numAttribs; ++i)
        {
            CustomAttribute & a = DefaultAttributeLayout[i];
            a.attrib -= CoustomAttributeLayout[a.semantic]; // only keep index (for uvXY)
        }

        CoustomAttributeLayout[VT_Normals] = 1;
        CoustomAttributeLayout[VT_Diffuse] = 2;
        CoustomAttributeLayout[VT_Tex_Coord] = 3;
        CoustomAttributeLayout[VT_Matrix_Weight] = 4;
        CoustomAttributeLayout[VT_Matrix_Index] = 5;
        CoustomAttributeLayout[VT_Tangent] = 6;
        CoustomAttributeLayout[VT_Binormal] = 7;

        for (size_t i = 0; i < numAttribs; ++i)
        {
            CustomAttribute& a = DefaultAttributeLayout[i];
            a.attrib += CoustomAttributeLayout[a.semantic];
        }
    }
    //-----------------------------------------------------------------------
    void GLSLProgramBase::bindFixedAttributes( GLuint programName )
    {
        struct SemanticNameTable
        {
            const char *semanticName;
            VertexType semantic;
        };

#define N_NUM_VertexType 12
        static const SemanticNameTable attributesTable[N_NUM_VertexType] =
        {
            { "vertex",             VT_Pos },
            { "blendWeights",       VT_Matrix_Weight },
            { "blendIndices",       VT_Matrix_Index },
            { "normal",             VT_Normals },
            { "colour",             VT_Diffuse },
            { "secondary_colour",   VT_Specular },
            { "tangent",            VT_Tangent },
            { "binormal",           VT_Binormal },
            { "qtangent",           VT_Normals },
            { "blendWeights2",      VT_Custom6 },
            { "blendIndices2",      VT_Custom7 },
            { "uv",                 VT_Tex_Coord },
        };
        const NCount maxVertexAttribs = N_Only(GBuffer).getMaxVertexAttribs();

        for( size_t i = 0; i < N_NUM_VertexType - 1; ++i )
        {
            const SemanticNameTable &entry = attributesTable[i];
            GLint attrIdx = DefaultAttributeLayout[entry.semantic];
            if( attrIdx < maxVertexAttribs )
            {
                GLCheck( glBindAttribLocation( programName, attrIdx, entry.semanticName ) );
            }
        }

        for( size_t i = 0; i < 8; ++i )
        {
            //UVs are a special case.
            GLCheck( glBindAttribLocation( programName, DefaultAttributeLayout[VT_Tex_Coord] + i, ("uv" + N_convV( i )).c_str() ) );
        }

        if( N_Engine().getRender()->getFeature()->isSupport(CF_BaseInstance) )
            GLCheck( glBindAttribLocation( programName, 15, "drawId" ) );
    }
    //-----------------------------------------------------------------------
    void GLSLProgramBase::setupBaseInstance( GLuint programName )
    {
        if( !N_Engine().getRender()->getFeature()->isSupport(CF_BaseInstance) )
            mBaseInstanceLocation = glGetUniformLocation( programName, "baseInstance" );
    }
    //-----------------------------------------------------------------------
    Ni32 GLSLProgramBase::getFixedAttributeIndex(VertexType vtype, NIIi index)
    {
        N_assert(vtype > 0 && vtype <= VT_Count, "Missing attribute!");

        if(vtype == VT_Tex_Coord)
            return CoustomAttributeLayout[vtype] + index;

        return CoustomAttributeLayout[vtype];
    }
    //-----------------------------------------------------------------------
    VertexType GLSLProgramBase::getAttributeSemanticEnum(const VString & type)
    {
        size_t numAttribs = sizeof(DefaultAttributeLayout)/sizeof(CustomAttribute);
        for (size_t i = 0; i < numAttribs; ++i)
        {
            if (DefaultAttributeLayout[i].name == type)
                return DefaultAttributeLayout[i].semantic;
        }

        N_assert(false, "Missing attribute!");
        return VertexType(0);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //GLSLShaderBase
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    NIIi GLSLShaderBase::mShaderCount = 0;
    //-----------------------------------------------------------------------
    class CmdAttach : public PropertyCmd
    {
    public:
        CmdAttach() : PropertyCmd(N_PropertyGpuProgramGLSL_Attach, _T("attach"),
            _T("name of another GLSL program needed by this program"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = (static_cast<const GLSLShaderBase *>(own))->getAttachedShaderNames();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            //get all the shader program names: there could be more than one
            StringList vecShaderNames;
            StrUtil::split(val, vecShaderNames, _T(" \t"), 0);
            size_t programNameCount = vecShaderNames.size();
            for (size_t i = 0; i < programNameCount; ++i)
            {
                Nui32 temp;
                StrConv::conv(vecShaderNames[i], temp);
                static_cast<GLSLShaderBase *>(own)->attachChildShader(temp);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {

        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {

        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAttach();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class CmdColumnMajorMatrices : public PropertyCmd
    {
    public:
        CmdColumnMajorMatrices() : PropertyCmd(N_PropertyGpuProgramGLSL_ColumnMajor, _T("column_major_matrices"),
            _T("Whether matrix packing in column-major order."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const GLSLShaderBase*>(own)->getColumnMajorMatrices();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool tmp;
            StrConv::conv(val, tmp);
            static_cast<GLSLShaderBase*>(own)->setColumnMajorMatrices(tmp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const GLSLShaderBase *>(own)->getColumnMajorMatrices();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<GLSLShaderBase *>(own)->setColumnMajorMatrices(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColumnMajorMatrices();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("true");
        }
    };

    class CmdPreprocessorDefines : public PropertyCmd
    {
    public:

        CmdPreprocessorDefines() : PropertyCmd(N_PropertyGpuProgramGLSL_PreDefines, _T("preprocessor_defines"),
            _T("Preprocessor defines use to compile the program."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const GLSLShaderBase *>(own)->getPreprocessorDefines(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            VString temp;
            StrConv::conv(val, temp);
            static_cast<GLSLShaderBase *>(own)->setPreprocessorDefines(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPreprocessorDefines();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //-----------------------------------------------------------------------
    String GLSLShaderBase::getResourceLogName() const
    {
        if(mProgramSrcValid)
            return _T("'") + mFile + _T("'");
        return _T("'") + mSrc + _T("'");
    }
    //-----------------------------------------------------------------------
    void GLSLShaderBase::loadCodeImpl(const VString & code)
    {
        // Preprocess the GLSL shader in order to get a clean source
        CPreprocessor cpp;

        // Define "predefined" macros.
        // TODO: decide, should we define __VERSION__, and with what value.
        if(getLanguage() == SL_GLSLES)
            cpp.Define("GL_ES", 5, 1);

        VString defines = mPreprocessorDefines;
        for(const auto& def : parseDefines(defines))
        {
            cpp.Define(def.first, strlen(def.first), def.second, strlen(def.second));
        }

		// deal with includes
		mSource = genRef(mSource, this, mFile);

        size_t out_size = 0;
        const char *src = mSource.c_str ();
        size_t src_len = mSource.size ();
        char *out = cpp.Parse (src, src_len, out_size);
        if (!out || !out_size)
            // Failed to preprocess, break out
            N_EXCEPT(RenderingAPI, _T("Failed to preprocess shader ") + mSrc);

        mSource = VString (out, out_size);
        if (out < src || out > src + src_len)
            free (out);

        compile(true);
    }
    //---------------------------------------------------------------------------
    void GLSLShaderBase::unloadImpl()
    {
        // We didn't create mAssemblerProgram through a manager, so override this
        // implementation so that we don't try to remove it from one. Since getCreator()
        // is used, it might target a different matching handle!
        if (mAssemblerCode)
        {
            N_delete mAssemblerCode;
            mAssemblerCode = 0;
        }

        unloadSysCode();
    }
    //-----------------------------------------------------------------------
    void GLSLShaderBase::configParamImpl(GpuProgramParam * params)
    {
        getParamDef();
        params->setDefine(mDefines);
        // Don't set logical / physical maps here, as we can't access parameters by logical index in GLHL.
    }
    //-----------------------------------------------------------------------
    GLSLShaderBase::GLSLShaderBase(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
        HighLevelGpuProgram(rid, gid, ls, rs), 
        mColumnMajorMatrices(true), 
        mLinked(0), 
        mShaderID(++mShaderCount) // Increase shader counter and use as ID
    {
    }
    //-----------------------------------------------------------------------
    void GLSLShaderBase::attachChildShader(ResourceID name)
    {
        // is the name valid and already loaded?
        // check with the high level program manager to see if it was loaded
        HighLevelGpuProgram * prog = static_cast<HighLevelGpuProgram *>(N_Only(HighLevelGpuProgram).get(name, GroupUnknow));
        if (prog && prog->getSyntaxType() == SL_GLSL)
        {
            // make sure attached program source gets loaded and compiled
            // don't need a low level implementation for attached shader objects
            // loadHighLevel will only load the source and compile once
            // so don't worry about calling it several times
            GLSLShaderBase* childShader = static_cast<GLSLShaderBase *>(prog);
            // load the source and attach the child shader only if supported
            if (isValid())
            {
                childShader->loadSysCode();
                // add to the container
                mAttachedGLSLPrograms.push_back( childShader );
                mAttachedShaderNames += name + _T(" ");
            }
        }
    }
    //-----------------------------------------------------------------------
    bool GLSLShaderBase::initCmd(PropertyCmdSet * dest)
    {
        HighLevelGpuProgram::initCmd(dest);
        dest->add(N_new CmdAttach());
        dest->add(N_new CmdColumnMajorMatrices());
        dest->add(N_new CmdPreprocessorDefines());
        return true;
    }
    //-----------------------------------------------------------------------
    
    
}
