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
#ifndef __GLSLLinkProgram_H__
#define __GLSLLinkProgram_H__

#include "NiiGLPreInclude.h"
#include "NiiGpuProgram.h"
#include "NiiGpuBuffer.h"
#include "NiiGLUniformCache.h"
#include "NiiGLSLProgramBase.h"

namespace NII 
{
namespace GLSL 
{
    /** Structure used to keep track of named atomic counter uniforms
        in the linked program object.  Same as GLUniformReference, but
        contains an additional offset parameter which currently only
        atomic counters feature.
    */
    struct GLAtomicCounterReference
    {
        /// GL binding handle (similar to location)
        GLint mBinding;
        /// GL offset (only used for atomic counters)
        GLint mOffset;
        /// Which type of program params will this value come from?
        GpuProgramType mSourceProgType;
        /// The constant definition it relates to
        const GpuParamUnit * mConstantDef;
    };

    typedef vector<GLAtomicCounterReference>::type GLAtomicCounterReferenceList;
    typedef GLAtomicCounterReferenceList::iterator GLAtomicCounterReferenceIterator;
    typedef map<GpuParamValue *, StructBuffer *>::type SharedParamsBufferMap;
    typedef vector<StructBuffer *>::type GLUniformBufferList;
    typedef GLUniformBufferList::iterator GLUniformBufferIterator;
    typedef vector<CounterBuffer *>::type GLCounterBufferList;
    typedef GLCounterBufferList::iterator GLCounterBufferIterator;

    /** C++ encapsulation of GLSL Program Object

    */
    class _EngineGLAPI GLSLProgram : public GLSLProgramBase
    {
    public:
        /// Constructor should only be used by GLSLProgramManager
        GLSLProgram(GLSLShader* vp, GLSLShader * ts, GLSLShader * ds, GLSLShader* gp, GLSLShader* fp, GLSLShader * cs);

        ~GLSLProgram(void);

        virtual void updateAtomicCounters(GpuProgramParam * params, uint16 mask, GpuProgramType fromProgType) = 0;

        void setTransformFeedbackVaryings(const std::vector<String> & nameStrings);
        
        GLSLShader * getVertexShader() const           { return static_cast<GLSLShader *>(mVertexShader); }
        GLSLShader * getHullShader() const             { return mHullShader; }
        GLSLShader * getDomainShader() const           { return mDomainShader; }
        GLSLShader * getGeometryShader() const         { return mGeometryShader; }
        GLSLShader * getFragmentShader() const         { return mFragmentShader; }
        GLSLShader * getComputeShader() const          { return mComputeShader; }

        /// Get the GL Handle for the program object
        //GLhandleARB getGLHandle() const                 { return mGLProgramHandle; }

        /// @copydetails GLSLProgramBase::isUsingShader
        bool isUsingShader(GLSLShaderBase * shader) const
        {
            return mVertexShader == shader || (GLSLShaderBase*)mGeometryShader == shader ||
                   (GLSLShaderBase*)mFragmentShader == shader ||
                   (GLSLShaderBase*)mHullShader == shader ||
                   (GLSLShaderBase*)mDomainShader == shader ||
                   (GLSLShaderBase*)mComputeShader == shader;
        }
    protected:
        Nui32 getCombinedHash() const;
        
        String getCombinedName() const;
        
        String getCombinedSource() const;
    private:
        /// Container of atomic counter uniform references that are active in the program object
        GLAtomicCounterReferenceList mGLAtomicCounterReferences;

        /// Map of shared parameter blocks to uniform buffer references
        SharedParamsBufferMap mSharedParamsBufferMap;
        
        /// Container of uniform buffer references that are active in the program object
        GLUniformBufferList mGLUniformBufferReferences;

        /// Container of counter buffer references that are active in the program object
        GLCounterBufferList mGLCounterBufferReferences;

        GLSLShader * mHullShader;
        GLSLShader * mDomainShader;
        GLSLShader * mGeometryShader;
        GLSLShader * mFragmentShader;
        GLSLShader * mComputeShader;
    };
}
}
#endif
