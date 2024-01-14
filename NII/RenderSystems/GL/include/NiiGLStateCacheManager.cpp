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

#include "NiiGLStateCacheManager.h"
#include "NiiGLRenderSystem.h"
#include "NiiEngine.h"

namespace NII 
{
    //-----------------------------------------------------------------------------
    GLStateCacheManager::GLStateCacheManager(void)
    {
        clearCache();
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::initializeCache()
    {
        glBlendEquation(GL_FUNC_ADD);

        if(GLEW_VERSION_2_0)
        {
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        }
        else if(GLEW_EXT_blend_equation_separate)
        {
            glBlendEquationSeparateEXT(GL_FUNC_ADD, GL_FUNC_ADD);
        }

        glBlendFunc(GL_ONE, GL_ZERO);
        
        glCullFace(mCullFace);

        glDepthFunc(mDepthFunc);

        glDepthMask(mDepthMask);

        glStencilMask(mStencilMask);

        glClearDepth(mClearDepth);

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

        glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);

        glClearColor(mClearColour[0], mClearColour[1], mClearColour[2], mClearColour[3]);

        glColorMask(mColourMask & CM_RED, mColourMask & CM_GREEN, mColourMask & CM_BLUE, mColourMask & CM_ALPHA);

        glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
        
        glBlendEquation(GL_FUNC_ADD);

        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::clearCache()
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
        mShininess = 0.0f;
        mPolygonMode = GL_FILL;
        mShadeModel = GL_SMOOTH;

        // Initialize our cache variables and also the GL so that the
        // stored values match the GL state
        mBlendFuncSource = GL_ONE;
        mBlendFuncDest = GL_ZERO;
        mBlendFuncSourceAlpha = GL_ONE;
        mBlendFuncDestAlpha = GL_ZERO;
        
        mClearColour[0] = mClearColour[1] = mClearColour[2] = mClearColour[3] = 0.0f;
        mColourMask = CM_RED | CM_BLUE | CM_GREEN | CM_ALPHA;

#ifdef N_ENABLE_STATE_CACHE
        mEnableVector.reserve(25);
        mEnableVector.clear();
#endif
        mActiveBufferMap.clear();
        mTexUnitsMap.clear();
        mTextureCoordGen.clear();

        mViewport[0] = 0.0f;
        mViewport[1] = 0.0f;
        mViewport[2] = 0.0f;
        mViewport[3] = 0.0f;

        mAmbient[0] = 0.2f;
        mAmbient[1] = 0.2f;
        mAmbient[2] = 0.2f;
        mAmbient[3] = 1.0f;

        mDiffuse[0] = 0.8f;
        mDiffuse[1] = 0.8f;
        mDiffuse[2] = 0.8f;
        mDiffuse[3] = 1.0f;

        mSpecular[0] = 0.0f;
        mSpecular[1] = 0.0f;
        mSpecular[2] = 0.0f;
        mSpecular[3] = 1.0f;

        mEmissive[0] = 0.0f;
        mEmissive[1] = 0.0f;
        mEmissive[2] = 0.0f;
        mEmissive[3] = 1.0f;

        mLightAmbient[0] = 0.2f;
        mLightAmbient[1] = 0.2f;
        mLightAmbient[2] = 0.2f;
        mLightAmbient[3] = 1.0f;

        mPointSize = 1.0f;
        mPointSizeMin = 1.0f;
        mPointSizeMax = 1.0f;
        mPointAttenuation[0] = 1.0f;
        mPointAttenuation[1] = 0.0f;
        mPointAttenuation[2] = 0.0f;

        mActiveDrawFrameBuffer = 0;
        mActiveReadFrameBuffer = 0;

        mActiveVertexArray = 0;
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::bindGLBuffer(GLenum target, GLuint buffer, bool force)
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
                N_assert(false, "not implemented");
            }
            else if(target == GL_RENDERBUFFER)
            {
                GLCheck(glBindRenderbufferEXT(target, buffer));
            }
            else
            {
                GLCheck(glBindBuffer(target, buffer));
            }
        }

    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::deleteGLBuffer(GLenum target, GLuint buffer)
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
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::invalidateStateForTexture(GLuint texture)
    {
#ifdef N_ENABLE_STATE_CACHE
        mTexUnitsMap.erase(texture);
#endif
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setCullFace(GLenum face)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mCullFace != face)
#endif
        {
            mCullFace = face;
            
            glCullFace(face);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setBlendEquation(GLenum eqRGB, GLenum eqAlpha)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mBlendEquationRGB != eqRGB || mBlendEquationAlpha != eqAlpha)
#endif
        {
            mBlendEquationRGB = eqRGB;
            mBlendEquationAlpha = eqAlpha;

            if(GLEW_VERSION_2_0)
            {
                GLCheck(glBlendEquationSeparate(eqRGB, eqAlpha));
            }
            else if(GLEW_EXT_blend_equation_separate)
            {
                GLCheck(glBlendEquationSeparateEXT(eqRGB, eqAlpha));
            }
            else
            {
                GLCheck(glBlendEquation(eqRGB));
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setMaterialDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mDiffuse[0] != r) ||
           (mDiffuse[1] != g) ||
           (mDiffuse[2] != b) ||
           (mDiffuse[3] != a))
#endif
        {
            mDiffuse[0] = r;
            mDiffuse[1] = g;
            mDiffuse[2] = b;
            mDiffuse[3] = a;

            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &mDiffuse[0]);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setMaterialAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mAmbient[0] != r) ||
           (mAmbient[1] != g) ||
           (mAmbient[2] != b) ||
           (mAmbient[3] != a))
#endif
        {
            mAmbient[0] = r;
            mAmbient[1] = g;
            mAmbient[2] = b;
            mAmbient[3] = a;

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &mAmbient[0]);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setMaterialEmissive(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mEmissive[0] != r) ||
           (mEmissive[1] != g) ||
           (mEmissive[2] != b) ||
           (mEmissive[3] != a))
#endif
        {
            mEmissive[0] = r;
            mEmissive[1] = g;
            mEmissive[2] = b;
            mEmissive[3] = a;

            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &mEmissive[0]);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setMaterialSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mSpecular[0] != r) ||
           (mSpecular[1] != g) ||
           (mSpecular[2] != b) ||
           (mSpecular[3] != a))
#endif
        {
            mSpecular[0] = r;
            mSpecular[1] = g;
            mSpecular[2] = b;
            mSpecular[3] = a;

            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &mSpecular[0]);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setMaterialShininess(GLfloat shininess)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mShininess != shininess)
#endif
        {
            mShininess = shininess;
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setShadeModel(GLenum model)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mShadeModel != model)
#endif
        {
            mShadeModel = model;
            glShadeModel(model);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setLightAmbient(GLfloat r, GLfloat g, GLfloat b)
    {
#ifdef N_ENABLE_STATE_CACHE
        if((mLightAmbient[0] != r) ||
           (mLightAmbient[1] != g) ||
           (mLightAmbient[2] != b))
#endif
        {
            mLightAmbient[0] = r;
            mLightAmbient[1] = g;
            mLightAmbient[2] = b;

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mLightAmbient[0]);
        }
    }
    //-----------------------------------------------------------------------------
    void GLStateCacheManager::setPointParameters(const GLfloat *attenuation, float minSize, float maxSize)
    {
        if(minSize > -1)
#ifdef N_ENABLE_STATE_CACHE
        if (minSize != mPointSizeMin)
#endif
        {
            mPointSizeMin = minSize;
            const RenderFeature* caps = dynamic_cast<GLRenderSystem*>(N_Engine().getRender())->getFeature();
            if (caps->isSupport(GF_Point_Param))
                glPointParameterf(GL_POINT_SIZE_MIN, mPointSizeMin);
        }

        if(maxSize > -1)
#ifdef N_ENABLE_STATE_CACHE
        if (maxSize != mPointSizeMax)
#endif
        {
            mPointSizeMax = maxSize;
            const RenderFeature* caps = dynamic_cast<GLRenderSystem*>(N_Engine().getRender())->getFeature();
            if (caps->isSupport(GF_Point_Param))
                glPointParameterf(GL_POINT_SIZE_MAX, mPointSizeMax);
        }

        if(attenuation)
#ifdef N_ENABLE_STATE_CACHE
        if (attenuation[0] != mPointAttenuation[0] || attenuation[1] != mPointAttenuation[1] || attenuation[2] != mPointAttenuation[2])
#endif
        {
            mPointAttenuation[0] = attenuation[0];
            mPointAttenuation[1] = attenuation[1];
            mPointAttenuation[2] = attenuation[2];
            const RenderFeature* caps = dynamic_cast<GLRenderSystem*>(N_Engine().getRender())->getFeature();
            if (caps->isSupport(GF_Point_Param))
                glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, &mPointAttenuation[0]);
        }
    }
    //-----------------------------------------------------------------------------

    void GLStateCacheManager::bindGLFrameBuffer(GLenum target,GLuint buffer, bool force)
    {
#ifdef N_ENABLE_STATE_CACHE
        bool update = false;
       
        //GL_FRAMEBUFFER sets both GL_DRAW_FRAMEBUFFER and GL_READ_FRAMEBUFFER
        if(target==GL_FRAMEBUFFER)
        {
            if( buffer != mActiveDrawFrameBuffer
             || buffer != mActiveReadFrameBuffer)
            {
                update = true;
                mActiveReadFrameBuffer=buffer;
                mActiveDrawFrameBuffer=buffer;
            }
        }
        else if( target == GL_DRAW_FRAMEBUFFER)
        {
            if(buffer != mActiveDrawFrameBuffer)
            {
                update = true;
                mActiveDrawFrameBuffer=buffer;
            }
        }
        else if( target == GL_READ_FRAMEBUFFER)
        {
            if(buffer != mActiveReadFrameBuffer)
            {
                update = true;
                mActiveReadFrameBuffer=buffer;
            }
        }

        // Update GL
        if(update)
#endif
        {
            GLCheck(glBindFramebuffer(target, buffer));
        }
    }
    void GLStateCacheManager::bindGLRenderBuffer(GLuint buffer, bool force)
    {
#ifdef N_ENABLE_STATE_CACHE
        auto ret = mActiveBufferMap.emplace(GL_RENDERBUFFER, buffer);
        if(ret.first->second != buffer || force) // Update the cached value if needed
        {
            ret.first->second = buffer;
            ret.second = true;
        }

        // Update GL
        if(ret.second)
#endif
        {
            GLCheck(glBindRenderbuffer(GL_RENDERBUFFER, buffer));
        }
    }

    void GLStateCacheManager::deleteGLFrameBuffer(GLenum target, GLuint buffer)
    {   
        // Buffer name 0 is reserved and we should never try to delete it
        if(buffer == 0)
            return;
        
        //always delete the buffer, even if not currently bound
        GLCheck(glDeleteFramebuffers(1, &buffer));

#ifdef N_ENABLE_STATE_CACHE
        if (buffer == mActiveDrawFrameBuffer )
        {
            // Currently bound read frame buffer is being deleted, update the cached values to 0,
            mActiveDrawFrameBuffer = 0;
        }
        if ( buffer == mActiveReadFrameBuffer)
        {
            // Currently bound read frame buffer is being deleted, update the cached values to 0,
            mActiveReadFrameBuffer = 0;
        }
#endif
    }
    void GLStateCacheManager::deleteGLRenderBuffer(GLuint buffer)
    {
        // Buffer name 0 is reserved and we should never try to delete it
        if(buffer == 0)
            return;

        //always delete the buffer, even if not currently bound
        GLCheck(glDeleteRenderbuffers(1, &buffer));
        
#ifdef N_ENABLE_STATE_CACHE
        BindBufferMap::iterator i = mActiveBufferMap.find(GL_RENDERBUFFER);
        if (i != mActiveBufferMap.end() && ((*i).second == buffer))
        {
            // Currently bound render buffer is being deleted, update the cached value to 0,
            // which it likely the buffer that will be bound by the driver.
            // An update will be forced next time we try to bind on this target.
            (*i).second = 0;
        }
#endif
    }

    void GLStateCacheManager::bindGLVertexArray(GLuint vao)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mActiveVertexArray != vao)
        {
            mActiveVertexArray = vao;
            GLCheck(glBindVertexArray(vao));
            //we also need to clear the cached GL_ELEMENT_ARRAY_BUFFER value, as it is invalidated by glBindVertexArray
            mActiveBufferMap[GL_ELEMENT_ARRAY_BUFFER] = 0;
        }
#else
        GLCheck(glBindVertexArray(vao));
#endif
    }

    // TODO: Store as high/low bits of a GLuint, use vector instead of map for TexParameteriMap
    void GLStateCacheManager::setTexParameteri(GLenum target, GLenum pname, GLint param)
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
    
    void GLStateCacheManager::setTexParameterf(GLenum target, GLenum pname, GLfloat param)
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
    
    void GLStateCacheManager::bindGLTexture(GLenum target, GLuint texture)
    {
#ifdef N_ENABLE_STATE_CACHE
        mLastBoundTexID = texture;
#endif
        // Update GL
        GLCheck(glBindTexture(target, texture));
    }
    
    bool GLStateCacheManager::activateGLTextureUnit(size_t unit)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mActiveTextureUnit == unit)
            return true;
#endif

        if (unit >= N_Engine().getRender()->getFeature()->getMaxTexUnit())
            return false;

        GLCheck(glActiveTexture(GL_TEXTURE0 + unit));
        mActiveTextureUnit = unit;
        return true;
    }

    void GLStateCacheManager::setBlendFunc(GLenum source, GLenum dest, GLenum sourceA, GLenum destA)
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

    void GLStateCacheManager::setDepthMask(GLboolean mask)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mDepthMask != mask)
#endif
        {
            mDepthMask = mask;
            
            GLCheck(glDepthMask(mask));
        }
    }
    
    void GLStateCacheManager::setDepthFunc(GLenum func)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mDepthFunc != func)
#endif
        {
            mDepthFunc = func;
            
            GLCheck(glDepthFunc(func));
        }
    }
    
    void GLStateCacheManager::setClearDepth(GLclampf depth)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mClearDepth != depth)
#endif
        {
            mClearDepth = depth;
            
            GLCheck(glClearDepth(depth));
        }
    }
    
    void GLStateCacheManager::setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
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
    
    void GLStateCacheManager::setColourMask(Nmark mark)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mColourMask != mark)
#endif
        {
            mColourMask = mark;
            
            GLCheck(glColorMask(mColourMask & CM_RED, mColourMask & CM_GREEN, mColourMask & CM_BLUE, mColourMask & CM_ALPHA));
        }
    }
    
    void GLStateCacheManager::setStencilMask(GLuint mask)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mStencilMask != mask)
#endif
        {
            mStencilMask = mask;
            
            GLCheck(glStencilMask(mask));
        }
    }
    
    void GLStateCacheManager::setEnabled(GLenum flag)
    {
#ifdef N_ENABLE_STATE_CACHE
        auto iter = std::find(mEnableVector.begin(), mEnableVector.end(), flag);
        bool was_enabled = iter != mEnableVector.end();

        if(was_enabled == true)
            return; // no change

        mEnableVector.push_back(flag);
#endif
        GLCheck(glEnable(flag));
    }
    
    void GLStateCacheManager::setDisabled(GLenum flag)
    {
#ifdef N_ENABLE_STATE_CACHE
        auto iter = std::find(mEnableVector.begin(), mEnableVector.end(), flag);
        bool was_enabled = iter != mEnableVector.end();

        if(was_enabled == false)
            return; // no change

        mEnableVector.erase(iter);
#endif
        GLCheck(glDisable(flag));
    }

    void GLStateCacheManager::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
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

    void GLStateCacheManager::setPolygonMode(GLenum mode)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mPolygonMode != mode)
#endif
        {
            mPolygonMode = mode;
            GLCheck(glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode));
        }
    }


    void GLStateCacheManager::setPointSize(GLfloat size)
    {
#ifdef N_ENABLE_STATE_CACHE
        if (mPointSize != size)
#endif
        {
            mPointSize = size;
            GLCheck(glPointSize(mPointSize));
        }
    }
    
    void GLStateCacheManager::enableTextureCoordGen(GLenum type)
    {
#ifdef N_ENABLE_STATE_CACHE
        std::unordered_map<GLenum, TexGenParams>::iterator it = mTextureCoordGen.find(mActiveTextureUnit);
        if (it == mTextureCoordGen.end())
        {
            GLCheck(glEnable(type));
            mTextureCoordGen[mActiveTextureUnit].mEnabled.insert(type);
        }
        else
        {
            if (it->second.mEnabled.find(type) == it->second.mEnabled.end())
            {
                GLCheck(glEnable(type));
                it->second.mEnabled.insert(type);
            }
        }
#else
        GLCheck(glEnable(type));
#endif
    }

    void GLStateCacheManager::disableTextureCoordGen(GLenum type)
    {
#ifdef N_ENABLE_STATE_CACHE
        std::unordered_map<GLenum, TexGenParams>::iterator it = mTextureCoordGen.find(mActiveTextureUnit);
        if (it != mTextureCoordGen.end())
        {
            std::set<GLenum>::iterator found = it->second.mEnabled.find(type);
            if (found != it->second.mEnabled.end())
            {
                GLCheck(glDisable(type));
                it->second.mEnabled.erase(found);
            }
        }
#else
        GLCheck(glDisable(type));
#endif
    }
    void GLStateCacheManager::bindGLProgramPipeline(GLuint handle)
    {
#ifdef N_ENABLE_STATE_CACHE
        if(mActiveProgramPipeline != handle)
#endif
        {
            mActiveProgramPipeline = handle;
            GLCheck(glBindProgramPipeline(mActiveProgramPipeline));
        }
    }
}
