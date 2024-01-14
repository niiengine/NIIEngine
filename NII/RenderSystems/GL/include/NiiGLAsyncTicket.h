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

#ifndef _Ogre_GL3PlusAsyncTicket_H_
#define _Ogre_GL3PlusAsyncTicket_H_

#include "OgreGL3PlusPrerequisites.h"

#include "NiiGpuAsync.h"
#include "NiiPixelFormat.h"

namespace Ogre
{
    class _EngineGLAPI GLBufferAsync : public GpuBufferAsync
    {
    public:
        GLBufferAsync( GpuBuffer *creator, NCount offset, NCount count );
        GLBufferAsync( GpuBuffer *creator, MappedBuffer * buffer, NCount offset, NCount count );
        virtual ~GLBufferAsync();

        virtual OperationReason isDataValid() const;
    protected:
        GLsync  mFenceName;

        virtual const void* mapImpl(void);
    };
    
    /** See TextureAsync
    */
    class _EngineGLAPI GLTextureAsync : public TextureAsync
    {
    public:
        GLTextureAsync( NCount width, NCount height, NCount depth, NCount array, Texture::Type type, PixelFormat pf, 
            GLGBufferManager *vaoManager, bool supportsGetTextureSubImage );
        virtual ~GLTextureAsync();

        virtual bool isDataValid(void);
    protected:
        GLuint createBuffer( NCount width, NCount height, NCount depth );

        virtual PixelBlock mapImpl(NCount depthIdx, NCount arrayIdx = 0, NCount oft = 0);
        
        virtual void unmapImpl();

        void waitForDownloadToFinish(void);

        virtual void writeImpl( Texture * src, NCount srcmipmap, PixelBlock * srcBox, NCount oft = 0, bool notify = false );
    protected:
        GLuint mVboName;
        /// In case GL_ARB_get_texture_sub_image / GL 4.5 is not available and
        /// user requested to only download a subregion of the texture.
        GLuint mTmpVboName;
        PixelBlock mSubregion;

        uint32 mDownloadFrame;
        GLsync mAccurateFence;
        GLGBufferManager * mMag;
        bool mSupportsGetTextureSubImage;
    };
}

#endif
