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

#ifndef _OgreGL3PlusRenderPassDescriptor_H_
#define _OgreGL3PlusRenderPassDescriptor_H_

#include "OgreGL3PlusPrerequisites.h"
#include "OgreRenderPassDescriptor.h"

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Resources
    *  @{
    */

    struct GL3PlusFrameBufferDescValue
    {
        GLuint  fboName;
        uint16  refCount;
        GL3PlusFrameBufferDescValue();
    };

    typedef map<FboKey, GL3PlusFrameBufferDescValue>::type GL3PlusFrameBufferDescMap;

    /** GL3+ will share FBO handles between all GLFrameBufferObject that share the
        same FBO setup. This doesn't mean these FrameBufferObject are exactly the
        same, as they may have different clear, loadAction or storeAction values.
    */
    class _OgreGL3PlusExport GLFrameBufferObject : public FrameBufferObject
    {
    protected:
        GLuint  mFboName;
        GLuint  mFboMsaaResolve;
        bool    mAllClearColoursSetAndIdentical;
        bool    mAnyColourLoadActionsSetToClear;
        bool    mHasRenderWindow;

        GL3PlusFrameBufferDescMap::iterator mSharedFboItor;

        GL3PlusRenderSystem *mRenderSys;

        void checkRenderWindowStatus(void);
        void switchToRenderWindow(void);
        void switchToFBO(void);
        /// Sets mAllClearColoursSetAndIdentical & mAnyColourLoadActionsSetToClear
        /// which can be used for quickly taking fast paths during rendering.
        void analyzeClearColour(void);

        void updateColourFbo( uint8 lastNumColourEntries );
        void updateDepthFbo(void);
        void updateStencilFbo(void);

        /// Returns a mask of FrameBufferObject::TargetType bits set that indicates
        /// if 'other' wants to perform clears on colour, depth and/or stencil values.
        /// If using MRT, each colour is evaluated independently (only the ones marked
        /// as clear will be cleared).
        uint32 checkForClearActions( GLFrameBufferObject *other ) const;

    public:
        GLFrameBufferObject( GL3PlusRenderSystem *renderSystem );
        virtual ~GLFrameBufferObject();

        GLuint getFboName(void) const       { return mFboName; }

        virtual void syncType( uint32 entryTypes );

        virtual void setClearColour( uint8 idx, const Colour &clearColour );
        virtual void setClearColour( const Colour &clearColour );

        uint32 willSwitchTo( GLFrameBufferObject *newDesc, bool warnIfRtvWasFlushed ) const;

        void performLoadActions( uint8 blendChannelMask, bool depthWrite, uint32 stencilWriteMask, uint32 entriesToFlush );
        void performStoreActions( bool hasArbInvalidateSubdata, uint32 entriesToFlush );
    };

    /** @} */
    /** @} */
}

#include "OgreHeaderSuffix.h"

#endif
