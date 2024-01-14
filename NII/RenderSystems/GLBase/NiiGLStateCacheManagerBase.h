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

#ifndef NII_GLSTATECACHEMANAGERBASE_H_
#define NII_GLSTATECACHEMANAGERBASE_H_

#include "NiiPreInclude.h"
#include "NiiGLSupport.h"

namespace NII
{
    /** An in memory cache of the OpenGL state.

     State changes can be particularly expensive time wise. This is because
     a change requires OpenGL to re-evaluate and update the state machine.
     Because of the general purpose nature of OGRE we often set the state for
     a specific texture, material, buffer, etc. But this may be the same as the
     current status of the state machine and is therefore redundant and causes
     unnecessary work to be performed by OpenGL.

     Instead we are caching the state so that we can check whether it actually
     does need to be updated. This leads to improved performance all around and 
     can be somewhat dramatic in some cases.

     @warning caching does not work with multiple windows, sharing the same context.
     They will erroneously get different state caches.
     */
    class _EngineGLAPI GLStateCacheManagerBase : public SysAlloc
    {
    public:
        virtual ~GLStateCacheManagerBase() {}

        inline void getViewport(NIIi * out)
        {
            out[0] = mViewport[0];
            out[1] = mViewport[1];
            out[2] = mViewport[2];
            out[3] = mViewport[3];
        }

        /// Gets the current colour mask setting RGBA order.
        inline Nui32 getColourMask() const { return mColourMask; }

        /// Gets the current depth mask setting.
        inline Nui8 getDepthMask() const { return mDepthMask; }

        /// Gets the current stencil mask.
        inline Nui32 getStencilMask() const { return mStencilMask; }
    protected:
        typedef std::unordered_map<Nui32, Nui32> BindBufferMap;
        typedef std::unordered_map<Nui32, NIIi> TexParameteriMap;
        typedef std::unordered_map<Nui32, NIIf> TexParameterfMap;

        /* These variables are used for caching OpenGL state.
         They are cached because state changes can be quite expensive,
         which is especially important on mobile or embedded systems.
         */

         /// Array of each OpenGL feature that is enabled i.e. blending, depth test, etc.
        std::vector<Nui32> mEnableVector;
        /// Stores the current clear colour
        NIIf mClearColour[4];
        /// Stores the current depth clearing colour
        NIIf mClearDepth;
        /// Stores the current colour write mask
        Nui32 mColourMask;
        /// Stores the current depth write mask
        Nui8 mDepthMask;
        /// Stores the current stencil mask
        Nui32 mStencilMask;
        /// Viewport origin and size
        NIIi mViewport[4];
        /// A map of different buffer types and the currently bound buffer for each type
        BindBufferMap mActiveBufferMap;
        /// Stores the current face culling setting
        Nui32 mCullFace;
        /// Stores the current depth test function
        Nui32 mDepthFunc;
        /// Stores the current blend equation
        Nui32 mBlendEquationRGB;
        Nui32 mBlendEquationAlpha;
        Nui32 mBlendFuncSource;
        Nui32 mBlendFuncDest;
        Nui32 mBlendFuncSourceAlpha;
        Nui32 mBlendFuncDestAlpha;
        /// Stores the currently active texture unit
        Nui32 mActiveTextureUnit;
    };
}

#endif
