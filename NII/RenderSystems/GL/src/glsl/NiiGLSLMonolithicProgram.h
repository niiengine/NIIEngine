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
#ifndef __GLSLMonolithicProgram_H__
#define __GLSLMonolithicProgram_H__

#include "NiiGLPreInclude.h"
#include "NiiGpuProgram.h"

namespace NII {

    class GLSLShader;

    /** Model of OpenGL program object created using the glLinkProgram
        method of linking.
        
        Linking using glLinkProgram is supported by OpenGL 2.0 and up,
        but does not allow hot-swapping shaders without recompiling
        the program object like GLSLSeparableProgram can. Hence the name
        'monolithic'.
    */
    class _EngineGLAPI GLSLMonolithicProgram : public GLSLProgram
    {
    public:
        /// Constructor should only be used by GLSLMonolithicProgramManager
        GLSLMonolithicProgram(GLSLShader* vs, GLSLShader* ts, GLSLShader* ds, GLSLShader* gs, GLSLShader* fs, GLSLShader* cs);
        ~GLSLMonolithicProgram(void);

        /** Makes a program object active by making sure it is linked
            and then putting it in use.
        */
        void activate(void);
        
        void inactivate();

        /** Updates program object uniforms using data from
            GpuProgramParameters.  normally called by
            GLSLShader::bindParameters() just before rendering
            occurs.
        */
        void updateUniforms(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype);
        
        /** Updates program object uniform blocks using data from
            GpuProgramParameters.  normally called by
            GLSLShader::bindParameters() just before rendering
            occurs.
        */
        void updateUniformBlocks(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype);
        
        /** Updates program object uniforms using data from pass
            iteration GpuProgramParameters.  normally called by
            GLSLShader::bindMultiPassParameters() just before multi
            pass rendering occurs.
        */
        void updatePassIterationUniforms(GpuProgramParam * params);

        void updateAtomicCounters(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype) {}
    protected:
        /// Get the the binary data of a program from the microcode cache
        void getCache(Nui32 id);
        
        /// Compiles and links the vertex and fragment programs
        void compileAndLink(void);

        void buildGLUniformReferences(void);
    private:
        GLUniformCache * mUniformCache;
    };
}
#endif // __GLSLMonolithicProgram_H__
