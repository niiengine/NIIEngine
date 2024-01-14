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

#ifndef _NII_GLStateCacheManager_H__
#define _NII_GLStateCacheManager_H__

#include "NiiGLPreInclude.h"
#include "NiiGLStateCacheManagerBase.h"

namespace NII
{
    class _EngineGLAPI GLStateCacheManager : public GLStateCacheManagerBase
    {
    public:
        GLStateCacheManager(void);
        
        void initializeCache();

        /** Clears all cached values
        */
        void clearCache();
        
        /** Bind an OpenGL buffer of any type.
         @param target The buffer target.
         @param buffer The buffer ID.
         @param force Optional parameter to force an update.
         */
        void bindGLBuffer(GLenum target, GLuint buffer, bool force = false);

        /** Delete an OpenGL buffer of any type.
         @param target The buffer target.
         @param buffer The buffer ID.
         */
        void deleteGLBuffer(GLenum target, GLuint buffer);

        /** Bind an OpenGL texture of any type.
         @param target The texture target.
         @param texture The texture ID.
         */
        void bindGLTexture(GLenum target, GLuint texture);

        /** Invalidates the state associated with a particular texture ID.
         @param texture The texture ID.
         */
        void invalidateStateForTexture(GLuint texture);

        /** Sets an integer parameter value per texture target.
         @param target The texture target.
         @param pname The parameter name.
         @param param The parameter value.
         */
        void setTexParameteri(GLenum target, GLenum pname, GLint param);
        
        /** Sets an float parameter value per texture target.
         @param target The texture target.
         @param pname The parameter name.
         @param param The parameter value.
         */
        void setTexParameterf(GLenum target, GLenum pname, GLfloat param);

        /** Activate an OpenGL texture unit.
         @param unit The texture unit to activate.
         @return Whether or not the texture unit was successfully activated.
         */
        bool activateGLTextureUnit(size_t unit);

        /// Set the blend equation for RGB and alpha separately.
        void setBlendEquation(GLenum eqRGB, GLenum eqA);

        /// Set the blend function for RGB and alpha separately.
        void setBlendFunc(GLenum source, GLenum dest, GLenum sourceA, GLenum destA);

        void setShadeModel(GLenum model);

        void setLightAmbient(GLfloat r, GLfloat g, GLfloat b);

        /** Sets the current depth mask setting.
         @param mask The depth mask to use.
         */
        void setDepthMask(GLboolean mask);

        /** Gets the current depth test function.
         @return The current depth test function.
         */
        GLenum getDepthFunc(void) const { return mDepthFunc; }

        /** Sets the current depth test function.
         @param func The depth test function to use.
         */
        void setDepthFunc(GLenum func);

        /** Gets the clear depth in the range from [0..1].
         @return The current clearing depth.
         */
        GLclampf getClearDepth(void) const { return mClearDepth; }

        /** Sets the clear depth in the range from [0..1].
         @param depth The clear depth to use.
         */
        void setClearDepth(GLclampf depth);

        /** Sets the color to clear to.
         @param red The red component.
         @param green The green component.
         @param blue The blue component.
         @param alpha The alpha component.
         */
        void setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

        /** Sets the current colour mask.
         @param red The red component.
         @param green The green component.
         @param blue The blue component.
         @param alpha The alpha component.
         */
        void setColourMask(Nmark);

        /** Sets the stencil mask.
         @param mask The stencil mask to use
         */
        void setStencilMask(GLuint mask);

        /** Enables a piece of OpenGL functionality.
         @param flag The function to enable.
         */
        void setEnabled(GLenum flag);
        
        void setDisabled(GLenum flag)

        /** Gets the current polygon rendering mode, fill, wireframe, points, etc.
         @return The current polygon rendering mode.
         */
        GLenum getPolygonMode(void) const { return mPolygonMode; }

        /** Sets the current polygon rendering mode.
         @param mode The polygon mode to use.
         */
        void setPolygonMode(GLenum mode);

        /** Sets the face culling mode.
         @return The current face culling mode
         */
        GLenum getCullFace(void) const { return mCullFace; }

        /** Sets the face culling setting.
         @param face The face culling mode to use.
         */
        void setCullFace(GLenum face);

        /// Enable the specified texture coordinate generation option for the currently active texture unit
        void enableTextureCoordGen(GLenum type);

        /// Disable the specified texture coordinate generation option for the currently active texture unit
        void disableTextureCoordGen(GLenum type);

        // Set material lighting parameters
        void setMaterialAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void setMaterialDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void setMaterialEmissive(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void setMaterialSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void setMaterialShininess(GLfloat shininess);
        void setPointSize(GLfloat size);
        void setPointParameters(const GLfloat * attenuation, float minSize = -1, float maxSize = -1);

        void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

        /** Bind an OpenGL frame buffer.
         @param target The buffer target.
         @param buffer The buffer ID.
         @param force Optional parameter to force an update.
         */
        void bindGLFrameBuffer(GLenum target,GLuint buffer, bool force = false);

        /** Bind an OpenGL frame buffer.
         @param buffer The buffer ID.
         @param force Optional parameter to force an update.
         */
        void bindGLRenderBuffer(GLuint buffer, bool force = false);

        /** Delete an OpenGL frame buffer.
         @param target The buffer target.
         @param buffer The buffer ID.
         */
        void deleteGLFrameBuffer(GLenum target, GLuint buffer);

        /** Delete an OpenGL render buffer.
         @param buffer The buffer ID.
         */
        void deleteGLRenderBuffer(GLuint buffer);

        /** Bind an OpenGL Vertex array object.
         @param vao The vertex array object ID.
         */
        void bindGLVertexArray(GLuint vao);

         /** Bind an OpenGL separable program pipeline
         @param handle The handle to the program pipeline
         */
        void bindGLProgramPipeline(GLuint handle);
    protected:
        struct TextureUnitParams
        {
            TexParameteriMap mTexParameteriMap;
            TexParameterfMap mTexParameterfMap;
        };

        typedef std::unordered_map<GLuint, TextureUnitParams> TexUnitsMap;
        
        struct TexGenParams
        {
            std::set<GLenum> mEnabled;
        };
        /// Stores the currently enabled texcoord generation types per texture unit
        std::unordered_map <GLenum, TexGenParams> mTextureCoordGen;

        /* These variables are used for caching OpenGL state.
         They are cached because state changes can be quite expensive,
         which is especially important on mobile or embedded systems.
         */

         /// Stores textures currently bound to each texture stage
        std::unordered_map <GLenum, GLuint> mBoundTextures;
        
        /// Stores the currently bound draw frame buffer value
        GLuint mActiveDrawFrameBuffer;
        /// Stores the currently bound read frame buffer value
        GLuint mActiveReadFrameBuffer;
        /// Stores the currently bound vertex array object
        GLuint mActiveVertexArray;
        /// Stores the currently bound separable program pipeline
        GLuint mActiveProgramPipeline;

        /// A map of texture parameters for each texture unit
        TexUnitsMap mTexUnitsMap;
        /// Stores the current polygon rendering mode
        GLenum mPolygonMode;
        /// Stores the last bound texture id
        GLuint mLastBoundTexID;

        GLenum mShadeModel;

        GLfloat mAmbient[4];
        GLfloat mDiffuse[4];
        GLfloat mSpecular[4];
        GLfloat mEmissive[4];
        GLfloat mLightAmbient[4];
        GLfloat mShininess;

        GLfloat mPointAttenuation[3];
        GLfloat mPointSize;
        GLfloat mPointSizeMin;
        GLfloat mPointSizeMax;
    };
}

#endif
