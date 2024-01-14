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

#ifndef _NII_GLRenderSystemBase_H__
#define _NII_GLRenderSystemBase_H__

#include "NiiGLPreInclude.h"
#include "NiiRenderSys.h"
#include "NiiViewWindow.h"

namespace NII 
{
    class GLContext;
    class GLSupport;
    class GLSLProgramBase;
    class GLRTTManager;

    class _EngineGLAPI GLRenderSystemBase : public RenderSys
    {
    public:
        virtual ~GLRenderSystemBase() {}

        virtual void _copyContentsToMemory(Viewport * vp, const Box & src, PixelBlock & dst, ViewWindow::FaceType buffer) = 0;

        inline GLContext * _getMainContext() const      { return mMainContext; }

        inline GLContext * _getCurrentContext() const   { return mCurrentContext; }
        
        inline GLSupport * getGLSupport() const         { return mGLSupport; }

        /**
        * Check if GL Version is supported
        */
        bool hasMinGLVersion(int major, int minor) const;

        /**
        * Check if an extension is available
        */
        bool checkGpuApi(const VString & ext) const;

        /** Unregister a render target->context mapping. If the context of target
            is the current context, change the context to the main context so it
            can be destroyed safely.

            @note This is automatically called by the destructor of
            GLContext.
        */
        virtual void _unregisterContext(GLContext *context) = 0;

        virtual void bindVertexElementToGpu(const VertexUnit & elem, const VertexBuffer * vbuffer, NCount vertexStart) = 0;

        /// @copydetails RenderSys::checkConfig
        String checkConfig() { return _T(""); }

        /// @copydetails RenderSys::setConfig
        bool setConfig(const String & name, const String & value);

        /// @copydetails RenderSys::getTexelXOffset
        NIIf getTexelXOffset()                          { return 0.0; }         // No offset in GL

        /// @copydetails RenderSys::getTexelYOffset
        NIIf getTexelYOffset()                          { return 0.0; }         // No offset in GL

        /// @copydetails RenderSys::getMinDepth
        NIIf getMinDepth()                              { return -1.0f; }       // Range [-1.0f, 1.0f]

        /// @copydetails RenderSys::getMaxDepth
        NIIf getMaxDepth()                              { return 1.0f; }        // Range [-1.0f, 1.0f]

        /// @copydetails RenderSys::getColourType
        DataType getColourType() const
        {
            return DT_Colour_ABGR;
        }

        /// @copydetails RenderSys::reinit
        void reinit()
        {
            this->shutdown();
            this->init();
        }

        /// @copydetails RenderSys::getSysProj
        void getSysProj(const Matrix4f & matrix, Matrix4f & dest, bool)
        {
            // no conversion request for OpenGL
            dest = matrix;
        }

        /// Mimics D3D9RenderSystem::_getDepthStencilFormatFor, if no FBO RTT manager, outputs GL_NONE
        void _getDepthStencilFormatFor(PixelFormat interpf, Nui32 * depth, Nui32* stencil);

        /** Create VAO on current context */
        virtual Nui32 _createVao() { return 0; }
        /** Bind VAO, context should be equal to current context, as VAOs are not shared  */
        virtual void _bindVao(GLContext* context, Nui32 vao) {}
        /** Destroy VAO immediately or defer if it was created on other context */
        virtual void _destroyVao(GLContext* context, Nui32 vao) {}
        /** Destroy FBO immediately or defer if it was created on other context */
        virtual void _destroyFbo(GLContext* context, Nui32 fbo) {}
        /** Complete destruction of VAOs and FBOs deferred while creator context was not current */
        void _completeDeferredVaoFboDestruction();

        virtual void resetRenderer(ViewWindow *) {}
        virtual void notifyOnContextLost() {}
    protected:
        void setupConfig();
        void refreshConfig();
    protected:
        GLContext * mMainContext;           ///< 主GL设备器 - 仅主线程
        GLContext * mCurrentContext;        ///< 当前GL设备器 - 仅主线程
        GLSupport * mGLSupport;
        String mVendor;
        GLRTTManager * mRTTManager;
        std::set<VString> mExtensionList;
    };
}

#endif
