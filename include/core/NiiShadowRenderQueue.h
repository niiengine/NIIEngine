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

#ifndef _NII_SHADOW_RENDER_QUEUE_H_
#define _NII_SHADOW_RENDER_QUEUE_H_

#include "NiiPreInclude.h"
#include "NiiRenderQueue.h"

namespace NII
{
    class _EngineInner ShadowRenderQueue : public RenderQueue
    {
    public:
        ShadowRenderQueue();
        ~ShadowRenderQueue();

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitNoShadowPasses(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        bool getSplitNoShadowPasses() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitPassesByLightingType(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        bool getSplitPassesByLightingType() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setShadowCastersCannotBeReceivers(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        bool getShadowCastersCannotBeReceivers() const;
    protected:
        /// @copydetails::createImpl
        void createImpl(RenderGroup *& out);

        /// @copydetails::createMainGroup
        void createMainGroup();
    protected:
        bool mSplitPassesByLightingType;
        bool mSplitNoShadowPasses;
        bool mShadowCastersCannotBeReceivers;
    };
}
#endif