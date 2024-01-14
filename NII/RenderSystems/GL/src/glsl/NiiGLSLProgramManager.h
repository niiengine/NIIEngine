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
#ifndef _NII_GLSLLinkProgramManager_H__
#define _NII_GLSLLinkProgramManager_H__

#include "NiiGLPreInclude.h"
#include "NiiSingleton.h"
#include "NiiGLSLProgramManagerBase.h"
#include "NiiGLSLExtSupport.h"
#include "NiiGLSLProgram.h"

namespace NII 
{
namespace GLSL 
{
    class _EngineGLAPI GLSLProgramManager : public Singleton<GLSLProgramManager>, public GLSLProgramManagerBase, public GPUAlloc
    {
    public:
        GLSLProgramManager(GLRenderSystem * sys);
        ~GLSLProgramManager();

        GLSLProgram * getActiveProgram(void);

        /** Set the active fragment shader for the next rendering state.
            The active program object will be cleared.
            Normally called from the GLSLGpuProgram::bindProgram and unbindProgram methods
        */
        void setActiveFragmentShader(GLSLShader * shader);

        /** Set the active geometry shader for the next rendering state.
            The active program object will be cleared.
            Normally called from the GLSLGpuProgram::bindProgram and unbindProgram methods
        */
        void setActiveGeometryShader(GLSLShader * shader);

        /** Set the active vertex shader for the next rendering state.
            The active program object will be cleared.
            Normally called from the GLSLGpuProgram::bindProgram and unbindProgram methods
        */
        void setActiveVertexShader(GLSLShader * shader);
        
        /// @copydoc setActiveVertexShader
        void setActiveHullShader(GLSLShader * shader);
        
        /// @copydoc setActiveVertexShader
        void setActiveDomainShader(GLSLShader * shader);
        
        /// @copydoc setActiveVertexShader
        void setActiveComputeShader(GLSLShader * shader);
        
        bool isAttributeValid(VertexType type, Ni32 index);
        
        NIIi getAttributeIndex(VertexType type, Ni32 index);

        /** Populate a list of uniforms based on an OpenGL program object.
        */
        void extractUniforms(
            GLuint programObject,
            const GpuParamUnitList* (&constantDefs)[6],
            GLUniformReferenceList& uniformList,
            GLAtomicCounterReferenceList& counterList,
            GLStructBufferList& uniformBufferList,
            SharedParamsBufferMap& sharedParamsBufferMap,
            GLCounterBufferList& counterBufferList);
            
        GLStateCacheManager * getStateManager() const       { return mRenderSys->getStateManager(); }
    private:
        /// @copydetails GLSLProgramManagerBase::convertGLUniformtoOgreType
        void convertGLUniformtoOgreType(GLenum gltype, GpuParamUnit & unit);
            
        /** Find the data source definition for a given uniform name
            and reference. Return true if found and pair the reference
            with its data source. */
        static bool findUniformDataSource(const String& paramName, const GpuParamUnitList* (&constantDefs)[6], GLUniformReference& refToUpdate);

        /** Find the data source definition for a given atomic counter
            uniform name and reference. Return true if found and pair
            the reference with its data source. */
        static bool findAtomicCounterDataSource(const String& paramName, const GpuParamUnitList* (&constantDefs)[6], GLAtomicCounterReference& refToUpdate);

        /// active objects defining the active rendering gpu state
        GLSLShader * mActiveVS;
        GLSLShader * mActiveGS;
        GLSLShader * mActiveFS;
        GLSLShader * mActiveTS;
        GLSLShader * mActiveDS;
        GLSLShader * mActiveCS;
        
        GLSLProgram * mActiveProgram;
        
        GLRenderSystem * mRenderSys;
    };
}
}
#endif
