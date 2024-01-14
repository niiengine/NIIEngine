/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef __GLMULTIRENDERTARGET_H__
#define __GLMULTIRENDERTARGET_H__

#include "NiiGLFrameBufferObject.h"
#include "NiiMultiTextureFrame.h"

namespace NII 
{
    /** MultiRenderTarget for GL. Requires the FBO extension.
    */
    class _EngineGLAPI GLFBOMultiRenderTarget : public MultiTextureFrame, public GLRenderTarget
    {
    public:
        GLFBOMultiRenderTarget(GLFBOManager * manager, const String &name);
        ~GLFBOMultiRenderTarget();

        ///@copydetails FrameObj::getPlatformData
        bool getPlatformData(const String & name, void * pData) const;

        ///@copydetails FrameObj::isFlipping
        bool isFlipping() const { return true; }

        ///@copydetails FrameObj::attachExt
        bool attachExt(ExtFrameBuffer * ebuf);

        ///@copydetails FrameObj::detachExt
        void detachExt();

        ///@copydetails GLRenderTarget::getContext
        GLContext * getContext() const { return fbo.getContext(); }

        ///@copydetails GLRenderTarget::getFBO
        GLFrameBufferObjectBase * getFBO() const { return &fbo; }
    protected:
        ///@copydetails MultiTextureFrame::attachImpl
        void attachImpl(Nidx idx, TextureFrame * obj);
        ///@copydetails MultiTextureFrame::detachImpl
        void detachImpl(Nidx idx);
    private:
        mutable GLFrameBufferObject fbo;
    };
}

#endif
