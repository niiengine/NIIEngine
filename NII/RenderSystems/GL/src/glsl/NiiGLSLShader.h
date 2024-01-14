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

#ifndef _NII_GLSLProgram_H__
#define _NII_GLSLProgram_H__

#include "NiiGLPreInclude.h"
#include "NiiGLSLShaderBase.h"
#include "NiiGeometryRaw.h"

namespace NII 
{
namespace GLSL 
{
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
    class _EngineGLAPI GLSLShader : public GLSLShaderBase
    {
    public:
        GLSLShader(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs);
        ~GLSLShader();

        //GLhandle getGLHandle() const                                    { return mGLShaderHandle; }
        
        GLuint getGLProgramHandle();

        /// @copydetails GLSLShaderBase::compile
        bool compile(bool checkErrors = true);

        /// @copydetails GLSLShaderBase::attachToProgramObject
        void attachToProgramObject(GLuint programObject );

        /// @copydetails GLSLShaderBase::detachFromProgramObject
        void detachFromProgramObject(GLuint programObject );

        /// @copydetails GpuProgram::getLanguage
        ShaderLanguage getLanguage() const;

        /// @copydetails GpuProgram::isEnvSpaceEnable
        bool isEnvSpaceEnable() const
        {
            return true;
        }

        /// @copydetails GpuProgram::isEnvColourEnable
        bool isEnvColourEnable() const
        {
            return true;
        }

        /// @copydetails GpuProgram::isEnvLightEnable
        bool isEnvLightEnable() const
        {
            return true;
        }

        /// @copydetails GpuProgram::isEnvFogEnable
        bool isEnvFogEnable() const
        {
            return true;
        }
        
        /// @copydoc Resource::calcSize
        NCount calcSize() const;
        
        /// Bind the shader in OpenGL.
        void bind(void);
        
        /// Unbind the shader in OpenGL.
        void unbind(void);
        
        /// Execute the param binding functions for this shader.
        void bindParameters(GpuProgramParam * params, uint16 mask);
        
        /// Execute the pass iteration param binding functions for this shader.
        void bindPassIterationParameters(GpuProgramParam * params);
        
        /// Execute the shared param binding functions for this shader.
        void bindSharedParameters(GpuProgramParam * params, uint16 mask);

        /** Returns the operation type that this geometry program expects to
            receive as input
        */
        GeometryRaw::OperationType getInputOperationType(void) const    { return mInputOperationType; }

        /** Returns the operation type that this geometry program will emit
        */
        GeometryRaw::OperationType getOutputOperationType(void) const   { return mOutputOperationType; }
        
        /** Returns the maximum number of vertices that this geometry program can
            output in a single run
        */
        NIIi getMaxOutputVertices(void) const                           { return mMaxOutputVertices; }

        /** Sets the operation type that this geometry program expects to receive
        */
        void setInputOperationType(GeometryRaw::OperationType ot)       { mInputOperationType = ot; }
        
        /** Set the operation type that this geometry program will emit
        */
        void setOutputOperationType(GeometryRaw::OperationType ot)
        {
            switch (ot)
            {
            case GeometryRaw::OT_POINT_LIST:
            case GeometryRaw::OT_LINE_STRIP:
            case GeometryRaw::OT_TRIANGLE_STRIP:
                break;
            default:
                N_EXCEPT(RenderingAPI, _T("Geometry shader output operation type can only be point list, line strip or triangle strip"));
            }

            mOutputOperationType = ot;
        }
        /** Set the maximum number of vertices that a single run of this geometry program
            can emit.
        */
        void setMaxOutputVertices(NIIi maxOutputVertices)
        { 
            mMaxOutputVertices = maxOutputVertices; 
        }
        
        /// Since GLSL has no assembly, use this shader for binding.
        GpuProgram * getMain()                                          { return this; }
    protected:
        /// @copydetails GLSLShaderBase::init
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails HighLevelGpuProgram::genAssemblerImpl
        void genAssemblerImpl();

        /// @copydetails HighLevelGpuProgram::unloadSysCodeImpl
        void unloadSysCodeImpl();

        /// @copydetails HighLevelGpuProgram::configParamImpl
        void configParamImpl(GpuProgramParam * params);

        /// @copydetails HighLevelGpuProgram::genParamDefImpl
        void genParamDefImpl();
        
        /// Add boiler plate code and preprocessor extras, then
        /// submit shader source to OpenGL.
        void submitSource();
    private:
        /// GL handle for shader object
        GLuint mGLShaderHandle;
        /// GL handle for program object the shader is bound to.
        GLuint mGLProgramHandle;

        // legacy GL_EXT_geometry_shader4 functionality
        GeometryRaw::OperationType mInputOperationType;
        GeometryRaw::OperationType mOutputOperationType;
        NIIi mMaxOutputVertices;
    };
}
}
#endif