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

#include "OgreGLES2StateCacheManager.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreRoot.h"

namespace Ogre {
    
    GLES2StateCacheManager::GLES2StateCacheManager(void)
    {
        clearCache();
    }
    
    void GLES2StateCacheManager::initializeCache()
    {
        GLCheck(glBlendEquation(GL_FUNC_ADD));
        
        GLCheck(glBlendFunc(GL_ONE, GL_ZERO));
        
        GLCheck(glCullFace(mCullFace));

        GLCheck(glDepthFunc(mDepthFunc));

        GLCheck(glDepthMask(mDepthMask));

        GLCheck(glStencilMask(mStencilMask));

        GLCheck(glClearDepthf(mClearDepth));

        GLCheck(glBindTexture(GL_TEXTURE_2D, 0));

        GLCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));

        GLCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        GLCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));

        GLCheck(glActiveTexture(GL_TEXTURE0));

        GLCheck(glClearColor(mClearColour[0], mClearColour[1], mClearColour[2], mClearColour[3]));

        GLCheck(glColorMask(mColourMask[0], mColourMask[1], mColourMask[2], mColourMask[3]));
    }
    
    void GLES2StateCacheManager::clearCache()
    {
        mDepthMask = GL_TRUE;
        mBlendEquationRGB = GL_FUNC_ADD;
        mBlendEquationAlpha = GL_FUNC_ADD;
        mCullFace = GL_BACK;
        mDepthFunc = GL_LESS;
        mStencilMask = 0xFFFFFFFF;
        mActiveTextureUnit = 0;
        mClearDepth = 1.0f;
        mLastBoundTexID = 0;
        
        // Initialize our cache variables and also the GL so that the
        // stored values match the GL state
        mBlendFuncSource = GL_ONE;
        mBlendFuncDest = GL_ZERO;
        mBlendFuncSourceAlpha = GL_ONE;
        mBlendFuncDestAlpha = GL_ZERO;
        
        mClearColour[0] = mClearColour[1] = mClearColour[2] = mClearColour[3] = 0.0f;
        mColourMask[0] = mColourMask[1] = mColourMask[2] = mColourMask[3] = GL_TRUE;

        mViewport[0] = 0.0f;
        mViewport[1] = 0.0f;
        mViewport[2] = 0.0f;
        mViewport[3] = 0.0f;
        
        mActiveVertexArray = 0;

#ifdef N_ENABLE_STATE_CACHE
        mEnableVector.reserve(25);
        mEnableVector.clear();
        mActiveBufferMap.clear();
        mTexUnitsMap.clear();

        mEnabledVertexAttribs.reserve(64);
        mEnabledVertexAttribs.clear();
#endif
    }

    void GLES2StateCacheManager::bindGLBuffer(GLenum target, GLuint buffer, bool force)
    {
#ifdef N_ENABLE_STATE_CACHE
        auto ret = mActiveBufferMap.emplace(target, buffer);
        if(ret.first->second != buffer || force) // Update the cached value if needed
        {
            ret.first->second = buffer;
            ret.second = true;
        }

        // Update GL
        if(ret.second)
#endif
        {
            if(target == GL_FRAMEBUFFER)
            {
                OgreAssert(false, "not implemented");
            }
            else if(target == GL_RENDERBUFFER)
            {
                GLCheck(glBindRenderbuffer(target, buffer));
            }
            else
            {
                GLCheck(glBindBuffer(target, buffer));
            }
        }
    }
    
    void GLES2StateCacheManager::deleteGLBuffer(GLenum target, GLuint buffer)
    {
        // Buffer name 0 is reserved and we should never try to delete it
        if(buffer == 0)
            return;
        
        if(target == GL_FRAMEBUFFER)
        {
            GLCheck(glDeleteFramebuffers(1, &buffer));
        }
        else if(target == GL_RENDERBUFFER)
        {
            GLCheck(glDeleteRenderbuffers(1, &buffer));
        }
        else
        {
            GLCheck(glDeleteBuffers(1, &buffer));
        }

#ifdef N_ENABLE_STATE_CACHE
        BindBufferMap::iterator i = mActiveBufferMap.find(target);
        
        if (i != mActiveBufferMap.end() && ((*i).second == buffer))
        {
            // Currently bound buffer is being deleted, update the cached value to 0,
            // which it likely the buffer that will be bound by the driver.
            // An update will be forced next time we try to bind on this target.
            (*i).second = 0;
        }
#endif
    }

    void GLES2StateCacheManager::bindGLVertexArray(GLuint vao)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mActiveVertexArray != vao)
        {
            mActiveVertexArray = vao;
            GLCheck(glBindVertexArrayOES(vao));
            //we also need to clear the cached GL_ELEMENT_ARRAY_BUFFER value, as it is invalidated by glBindVertexArray
            mActiveBufferMap[GL_ELEMENT_ARRAY_BUFFER] = 0;
        }
#else
        GLCheck(glBindVertexArrayOES(vao));
#endif
    }

    void GLES2StateCacheManager::invalidateStateForTexture(GLuint texture)
    {
#ifdef N_ENABLE_STATE_CACHE
        mTexUnitsMap.erase(texture);
#endif
    }

    // TODO: Store as high/low bits of a GLuint, use vector instead of map for TexParameteriMap
    void GLES2StateCacheManager::setTexParameteri(GLenum target, GLenum pname, GLint param)
    {
#ifdef N_ENABLE_STATE_CACHE
        // Check if we have a map entry for this texture id. If not, create a blank one and insert it.
        TexUnitsMap::iterator it = mTexUnitsMap.find(mLastBoundTexID);
        if (it == mTexUnitsMap.end())
        {
            TextureUnitParams unit;
            mTexUnitsMap[mLastBoundTexID] = unit;
            
            // Update the iterator
            it = mTexUnitsMap.find(mLastBoundTexID);
        }
        
        // Get a local copy of the parameter map and search for this parameter
        TexParameteriMap &myMap = (*it).second.mTexParameteriMap;
        auto ret = myMap.emplace(pname, param);
        TexParameteriMap::iterator i = ret.first;

        // Update the cached value if needed
        if((*i).second != param || ret.second)
        {
            (*i).second = param;
            
            // Update GL
            GLCheck(glTexParameteri(target, pname, param));
        }
#else
        GLCheck(glTexParameteri(target, pname, param));
#endif
    }

    void GLES2StateCacheManager::setTexParameterf(GLenum target, GLenum pname, GLfloat param)
    {
#ifdef N_ENABLE_STATE_CACHE
        // Check if we have a map entry for this texture id. If not, create a blank one and insert it.
        TexUnitsMap::iterator it = mTexUnitsMap.find(mLastBoundTexID);
        if (it == mTexUnitsMap.end())
        {
            TextureUnitParams unit;
            mTexUnitsMap[mLastBoundTexID] = unit;

            // Update the iterator
            it = mTexUnitsMap.find(mLastBoundTexID);
        }

        // Get a local copy of the parameter map and search for this parameter
        TexParameterfMap &myMap = (*it).second.mTexParameterfMap;
        auto ret = myMap.emplace(pname, param);
        TexParameterfMap::iterator i = ret.first;

        // Update the cached value if needed
        if((*i).second != param || ret.second)
        {
            (*i).second = param;

            // Update GL
            GLCheck(glTexParameterf(target, pname, param));
        }
#else
        GLCheck(glTexParameterf(target, pname, param));
#endif
    }

    void GLES2StateCacheManager::bindGLTexture(GLenum target, GLuint texture)
    {
        mLastBoundTexID = texture;
        
        // Update GL
        GLCheck(glBindTexture(target, texture));
    }
    
    bool GLES2StateCacheManager::activateGLTextureUnit(size_t unit)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mActiveTextureUnit == unit)
            return true;
#endif

        if (unit >= Root::getSingleton().getRenderSystem()->getCapabilities()->getMaxTexUnit())
            return false;

        GLCheck(glActiveTexture(GL_TEXTURE0 + unit));
        mActiveTextureUnit = unit;
        return true;
    }

    void GLES2StateCacheManager::setBlendFunc(GLenum source, GLenum dest, GLenum sourceA, GLenum destA)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mBlendFuncSource != source || mBlendFuncDest != dest || sourceA != mBlendFuncSourceAlpha || destA != mBlendFuncDestAlpha )
#endif
        {
            mBlendFuncSource = source;
            mBlendFuncDest = dest;
            mBlendFuncSourceAlpha = sourceA;
            mBlendFuncDestAlpha = destA;
            
            GLCheck(glBlendFuncSeparate(source, dest, sourceA, destA));
        }
    }

    void GLES2StateCacheManager::setBlendEquation(GLenum eqRGB, GLenum eqAlpha)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mBlendEquationRGB != eqRGB || mBlendEquationAlpha != eqAlpha)
#endif
        {
            mBlendEquationRGB = eqRGB;
            mBlendEquationAlpha = eqAlpha;

            GLCheck(glBlendEquationSeparate(eqRGB, eqAlpha));
        }
    }
    
    void GLES2StateCacheManager::setDepthMask(GLboolean mask)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mDepthMask != mask)
#endif
        {
            mDepthMask = mask;
            
            GLCheck(glDepthMask(mask));
        }
    }
    
    void GLES2StateCacheManager::setDepthFunc(GLenum func)
    {
        if(mDepthFunc != func)
        {
            mDepthFunc = func;
            
            GLCheck(glDepthFunc(func));
        }
    }
    
    void GLES2StateCacheManager::setClearDepth(GLclampf depth)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mClearDepth != depth)
#endif
        {
            mClearDepth = depth;
            
            GLCheck(glClearDepthf(depth));
        }
    }
    
    void GLES2StateCacheManager::setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mClearColour[0] != red) ||
           (mClearColour[1] != green) ||
           (mClearColour[2] != blue) ||
           (mClearColour[3] != alpha))
#endif
        {
            mClearColour[0] = red;
            mClearColour[1] = green;
            mClearColour[2] = blue;
            mClearColour[3] = alpha;
            
            GLCheck(glClearColor(mClearColour[0], mClearColour[1], mClearColour[2], mClearColour[3]));
        }
    }
    
    void GLES2StateCacheManager::setColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mColourMask[0] != red) ||
           (mColourMask[1] != green) ||
           (mColourMask[2] != blue) ||
           (mColourMask[3] != alpha))
#endif
        {
            mColourMask[0] = red;
            mColourMask[1] = green;
            mColourMask[2] = blue;
            mColourMask[3] = alpha;
            
            GLCheck(glColorMask(mColourMask[0], mColourMask[1], mColourMask[2], mColourMask[3]));
        }
    }
    
    void GLES2StateCacheManager::setStencilMask(GLuint mask)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mStencilMask != mask)
#endif
        {
            mStencilMask = mask;
            
            GLCheck(glStencilMask(mask));
        }
    }
    
    void GLES2StateCacheManager::setEnabled(GLenum flag)
    {
#ifdef N_ENABLE_STATE_CACHE
        bool found = std::find(mEnableVector.begin(), mEnableVector.end(), flag) != mEnableVector.end();
        if(!found)
        {
            mEnableVector.push_back(flag);
            
            GLCheck(glEnable(flag));
        }
#else
        GLCheck(glEnable(flag));
#endif
    }
    
    void GLES2StateCacheManager::setDisabled(GLenum flag)
    {
#ifdef N_ENABLE_STATE_CACHE
        auto iter = std::find(mEnableVector.begin(), mEnableVector.end(), flag);
        if(iter != mEnableVector.end())
        {
            mEnableVector.erase(iter);

            GLCheck(glDisable(flag));
        }
#else
        GLCheck(glDisable(flag));
#endif
    }

    void GLES2StateCacheManager::setCullFace(GLenum face)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mCullFace != face)
#endif
        {
            mCullFace = face;
            
            GLCheck(glCullFace(face));
        }
    }

    void GLES2StateCacheManager::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mViewport[0] != x) ||
           (mViewport[1] != y) ||
           (mViewport[2] != width) ||
           (mViewport[3] != height))
#endif
        {
            mViewport[0] = x;
            mViewport[1] = y;
            mViewport[2] = width;
            mViewport[3] = height;
            GLCheck(glViewport(x, y, width, height));
        }
    }

}
