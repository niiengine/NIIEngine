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

#ifndef NII_GLSLPROGRAMBASE_H_
#define NII_GLSLPROGRAMBASE_H_

#include "NiiGLPreInclude.h"
#include "NiiGpuBuffer.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiGpuProgram.h"
#include "NiiGeometryRaw.h"
#include "NiiGLUniformCache.h"

/// A value to define the case we didn't look for the attributes since the contractor
#define NULL_CUSTOM_ATTRIBUTES_INDEX -2

/// A value to define the attribute has not been found (this is also the result when glGetAttribLocation fails)
#define NOT_FOUND_CUSTOM_ATTRIBUTES_INDEX -1
        
namespace NII
{
    /// Structure used to keep track of named uniforms in the linked program object
    struct GLUniformReference
    {
        /// GL location handle
        GLint mLocation;
        /// Which type of program params will this value come from?
        GpuProgram::ShaderType mSourceProgType;
        /// The constant definition it relates to
        const GpuParamUnit * mConstantDef;
    };
    typedef vector<GLUniformReference>::type GLUniformReferenceList;
    typedef GLUniformReferenceList::iterator GLUniformReferenceIterator;

    typedef vector<StructBuffer *>::type GLStructBufferList;
    typedef GLStructBufferList::iterator GLStructBufferIterator;

    class GLSLProgramBase
    {
    public:
        GLSLProgramBase(GLSLShaderBase * shader);
        virtual ~GLSLProgramBase() {}

        void extractLayoutQualifiers(void);

        /** Sets whether the linked program includes the required instructions
            to perform skeletal animation.
            @remarks
            If this is set to true, OGRE will not blend the geometry according to
            skeletal animation, it will expect the vertex program to do it.
        */
        void setSkeletalAnimationIncluded(bool set)         { mSkeletalAnimation = set; }

        /** Returns whether the linked program includes the required instructions
            to perform skeletal animation.
            @remarks
             If this returns true, OGRE will not blend the geometry according to
             skeletal animation, it will expect the vertex program to do it.
                                                                             */
        bool isSkeletalAnimationIncluded() const            { return mSkeletalAnimation; }

        /// Get the GL Handle for the program object
        GLuint getGLProgramHandle() const                   { return mGLProgramHandle; }
        
        /// Get the index of a non-standard attribute bound in the linked code
        virtual GLint getAttributeIndex(VertexType vtype, NIIi index);
        
        bool isAttributeValid(VertexType semantic, NIIi index);

        /** Makes a program object active by making sure it is linked and then putting it in use.
         */
        virtual void activate() = 0;
        
        virtual void inactivate() = 0;

        /// query if the program is using the given shader
        virtual bool isUsingShader(GLSLShaderBase * shader) const = 0;

        /** Updates program object uniforms using data from GpuProgramParam.
            Normally called by GLSLShader::bindParameters() just before rendering occurs.
        */
        virtual void updateUniforms(GpuProgramParam * params, Nui16 mask, GpuProgram::ShaderType stype) = 0;

        /** Updates program object uniform blocks using data from GpuProgramParam.
            Normally called by GLSLShader::bindParameters() just before rendering occurs.
        */
        virtual void updateUniformBlocks(GpuProgramParam * params, Nui16 mask, GpuProgram::ShaderType stype) = 0;

        /** Updates program object uniforms using data from pass iteration GpuProgramParameters.
            Normally called by GLSLShader::bindMultiPassParameters() just before multi pass rendering occurs.
        */
        virtual void updatePassIterationUniforms(GpuProgramParam * params) = 0;

        /** Get the fixed attribute bindings normally used by GL for a vtype. */
        static Ni32 getFixedAttributeIndex(VertexType vtype, NIIi index);

        /**
         * use alternate vertex attribute layout using only 8 vertex attributes
         *
         * For "Vivante GC1000" and "VideoCore IV" (notably in Raspberry Pi) on GLES2
         */
        static void setTightAttLayout();
        
        /// Name / attribute list
        struct CustomAttribute
        {
            const char * name;
            Ni32 attrib;
            VertexType semantic;
        };
        
        static CustomAttribute DefaultAttributeLayout[16];
    protected:
        /// Compiles and links the vertex and fragment programs
        virtual void compileAndLink(void) = 0;
        
        void bindFixedAttributes( GLuint programName );

        void setupBaseInstance( GLuint programName );

        static VertexType getAttributeSemanticEnum(const VString & type);

        static const char * getAttributeSemanticString(VertexType vtype);

        /// Container of uniform references that are active in the program object
        GLUniformReferenceList mGLUniformReferences;

        /// Container of uniform buffer references that are active in the program object
        GLStructBufferList mGLStructBufferReferences;

        /// Linked vertex shader.
        GLSLShaderBase* mVertexShader;
        GLint mBaseInstanceLocation;
        /// GL handle for the program object
        GLuint mGLProgramHandle;
        /// Flag indicating that the program or pipeline object has been successfully linked
        int mLinked;
        /// An array to hold the attributes indexes
        int mCustomAttributesIndexes[VT_Count][NII_MAX_TextureCoord];
        
        /// Flag indicating skeletal animation is being performed
        bool mSkeletalAnimation;
        /// Flag to indicate that uniform references have already been built
        bool mUniformRefsBuilt;
    };
    
    /** Specialisation of HighLevelGpuProgram to provide support for OpenGL
        Shader Language (GLSL).
    @remarks
        GLSL has no target assembler or entry point specification like DirectX 9 HLSL.
        Vertex and Fragment shaders only have one entry point called "main".  
        When a shader is compiled, microcode is generated but can not be accessed by
        the application.
        GLSL also does not provide assembler low level output after compiling.  The GL Render
        system assumes that the Gpu program is a GL Gpu program so GLSLShader will create a 
        GLSLGpuProgram that is subclassed from GLGpuProgram for the low level implementation.
        The GLSLShader class will create a shader object and compile the source but will
        not create a program object.  It's up to GLSLGpuProgram class to request a program object
        to link the shader object to.

    @note
        GLSL supports multiple modular shader objects that can be attached to one program
        object to form a single shader.  This is supported through the "attach" material script
        command.  All the modules to be attached are listed on the same line as the attach command
        separated by white space.
        
    */
    class GLSLShaderBase : public HighLevelGpuProgram
    {
    public:
        GLSLShaderBase(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs);

        /// Get the OGRE assigned shader ID.
        NIIi getShaderID(void) const                        { return mShaderID; }

        const String & getAttachedShaderNames() const       { return mAttachedShaderNames; }

        /** Return the shader link status.
            Only used for separable programs.
        */
        NIIi isLinked(void) const                           { return mLinked; }

        /** Set the shader link status.
            Only used for separable programs.
        */
        void setLinked(NIIi flag)                           { mLinked = flag; }

        virtual bool compile(bool checkErrors = false) = 0;

        virtual void attachToProgramObject(GLuint programObject) = 0;

        virtual void detachFromProgramObject(GLuint programObject) = 0;

        /** Attach another GLSL Shader to this one. */
        void attachChildShader(ResourceID name);

        /** Sets whether matrix packing in column-major order. */ 
        void setColumnMajorMatrices(bool columnMajor)       { mColumnMajorMatrices = columnMajor; }

        /** Gets whether matrix packed in column-major order. */
        bool getColumnMajorMatrices() const                 { return mColumnMajorMatrices; }

        /// Get the uniform cache for this shader
        GLUniformCache * getUniformCache()                  { return &mUniformCache; }
    protected:
        /// @copydetails HighLevelGpuProgram::init
        bool initCmd(PropertyCmdSet * dest);

        String getResourceLogName() const;

        /// @copydetails GpuProgram::loadCodeImpl
        void loadCodeImpl(const VString & code);

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails HighLevelGpuProgram::configParamImpl
        void configParamImpl(GpuProgramParam * params);
    protected:
        /// Container of attached programs
        typedef std::vector< GLSLShaderBase* > GLSLProgramContainer;
        typedef GLSLProgramContainer::iterator GLSLProgramContainerIterator;

        /// Attached Shader names
        String mAttachedShaderNames;
        GLSLProgramContainer mAttachedGLSLPrograms;

        /** Flag indicating that the shader has been successfully
            linked.
            Only used for separable programs. */
        NIIi mLinked;

        /// OGRE assigned shader ID.
        NIIi mShaderID;

        /// Pointer to the uniform cache for this shader
        GLUniformCache mUniformCache;
        
        /// Matrix in column major pack format?
        bool mColumnMajorMatrices;

        /// Keep track of the number of shaders created.
        static NIIi mShaderCount;
    };
}
#endif
