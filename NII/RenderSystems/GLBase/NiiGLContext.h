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

#ifndef _NII_GLContext_H_
#define _NII_GLContext_H_

#include "NiiGLPreInclude.h"
#include "NiiGLStateCacheManagerBase.h"
#include "NiiSharedPtr.h"

namespace NII
{
    class _EngineGLAPI GLContext
    {
    public:
        GLContext() : initialized(false)
        {
        }
        virtual ~GLContext()
        {
        }

        virtual void setCurrent() = 0;

        virtual void endCurrent() {};

        bool getInitialized()           { return initialized; };
        
        void setInitialized()           { initialized = true; };
        
		virtual void releaseContext() {}

		virtual GLContext * clone() const = 0;

        /**
        * Get the state cache manager, creating it on demand
        */
        template<class SCM> SCM * getState()
        {
            if (!mState) 
            {
                SCM * stateMag = N_new SCM;
                stateMag->initializeCache();
                mState = SharedPtr<GLStateCacheManagerBase>(stateMag);
            }
            return static_cast<SCM *>(mState.get());
        }

        /// VAOs deferred for destruction in proper GL context
        std::vector<Nui32>& _getVaoDeferredForDestruction() { return mVaoDeferredForDestruction; }
        /// FBOs deferred for destruction in proper GL context
        std::vector<Nui32>& _getFboDeferredForDestruction() { return mFboDeferredForDestruction; }
    protected:
        bool initialized;
        SharedPtr<GLStateCacheManagerBase> mState;
        std::vector<Nui32> mVaoDeferredForDestruction;
        std::vector<Nui32> mFboDeferredForDestruction;
    };
}
#endif