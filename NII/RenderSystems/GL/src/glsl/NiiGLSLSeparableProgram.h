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
#ifndef __GLSLSeparableProgram_H__
#define __GLSLSeparableProgram_H__

#include "NiiGLPreInclude.h"

namespace Ogre
{
    //TODO Need to check that the shaders are being reused rather than
    // destroyed and then recompiled.

    /** Specialisation of GLSLShader to provide support for separable
        programs via the OpenGL program pipeline.

        Separable programs consist of shader objects which have been
        individually linked. This allows for invidual shaders in a
        program pipeline to be swapped without recompiling the program
        object, as would be necessary with monolithic programs. This
        is especially useful in the common case of only entities which
        change only a few of the shaders in the pipeline while leaving
        the rest the same.

        @remarks
        GLSL has no target assembler or entry point specification like
        DirectX 9 HLSL.  Vertex and Fragment shaders only have one
        entry point called "main".  When a shader is compiled,
        microcode is generated but can not be accessed by the
        application.

        GLSL also does not provide assembler low level output after
        compiling.  The GL Render system assumes that the Gpu program
        is a GL Gpu program so GLSLSeparableProgram will create a
        GL3PlusShader for the low level implementation.  The GLProgram
        class will create a shader and program object and compile the
        source but will not create a pipeline object.  It's up to
        GLGpuProgram class to request a program pipeline object to
        link the program object to.

        @note
        GLSL supportsn multiple modular shader objects that can be
        attached to one program object to form a single shader.  This
        is supported through the "attach" material script command.
        All the modules to be attached are listed on the same line as
        the attach command separated by white space.
    */
    class _EngineGLAPI GLSLSeparableProgram : public GLSLProgram
    {
    public:
        /// Constructor should only be used by GLSLSeparableProgramManager.
        GLSLSeparableProgram(GLSLShader* vs, GLSLShader* ts, GLSLShader* ds, GLSLShader* gs, GLSLShader* fs, GLSLShader* cs);
        ~GLSLSeparableProgram();
        
        /** Makes a program pipeline object active by making sure it
            is linked and then putting it in use.
        */
        void activate(void);
        
        void inactivate();

        /// GL Program Pipeline Handle
        GLuint getGLProgramPipelineHandle() const { return mGLProgramPipelineHandle; }

        /** Updates program pipeline object uniforms using named and
            indexed parameter data from GpuProgramParameters.
            normally called by GLSLShader::bindProgramParameters()
            just before rendering occurs.
        */
        void updateUniforms(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype);
        
        /** Updates program object atomic counter buffers using data
            from GpuProgramParameters.  Normally called by
            GLSLShader::bindProgramAtomicCounterParameters() just
            before rendering occurs.
        */
        void updateAtomicCounters(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype);
        
        /** Updates program object uniform blocks using shared
            parameter data from GpuProgramParameters.  Normally called
            by GLSLShader::bindProgramSharedParameters() just before
            rendering occurs.
        */
        void updateUniformBlocks(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype);
        
        /** Updates program pipeline object uniforms using data from
            pass iteration GpuProgramParameters.  Normally called by
            GLSLShader::bindProgramPassIterationParameters() just
            before multi pass rendering occurs.
        */
        void updatePassIterationUniforms(GpuProgramParam * params);
    protected:
        /// Compiles and links the separate programs.
        void compileAndLink(void);

        void loadIndividualProgram(GLSLShader *program);

        /// Build uniform references from active named uniforms.
        void buildGLUniformReferences(void);
        
        /// GL handle for pipeline object.
        GLuint mGLProgramPipelineHandle;
    };
}

#endif
