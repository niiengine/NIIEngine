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

#include "OgreGL3PlusPrerequisites.h"

#include "Vao/OgreGL3PlusAsyncTicket.h"
#include "Vao/OgreGL3PlusGBufferManager.h"

#include "Vao/OgreStagingBuffer.h"
#include "OgreGL3PlusTextureGpu.h"
#include "OgreGL3PlusMappings.h"
#include "Vao/OgreGL3PlusGBufferManager.h"

#include "OgreTextureBox.h"
#include "OgreTextureGpuManager.h"
#include "OgrePixelFormatGpuUtils.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    GLBufferAsync::GLBufferAsync( GpuBuffer * creator, NCount offset, NCount size ) :
        GpuBufferAsync( creator, offset, size ),
        mFenceName( 0 )
    {
        //Base constructor has already called async. We should now place a fence.
        GLCheck( mFenceName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
    }
    //-----------------------------------------------------------------------------------
    GLBufferAsync::GLBufferAsync( GpuBuffer * creator, MappedBuffer * buffer, NCount offset, NCount size ) :
        GpuBufferAsync( creator, buffer, offset, size ),
        mFenceName( 0 )
    {
        //Base constructor has already called async. We should now place a fence.
        GLCheck( mFenceName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
    }
    //-----------------------------------------------------------------------------------
    GLBufferAsync::~GLBufferAsync()
    {
        if( mFenceName )
        {
            GLCheck( glDeleteSync( mFenceName ) );
            mFenceName = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    const void * GLBufferAsync::mapImpl(void)
    {
        if( mFenceName )
            mFenceName = GLGBufferManager::waitFor( mFenceName );

        return mMapBuffer->readMap( mMapOffset, mSize );
    }
    //-----------------------------------------------------------------------------------
    OperationReason GLBufferAsync::isDataValid() const
    {
        if( mFenceName )
        {
            //Ask GL API to return immediately and tells us about the fence
            GLenum waitRet = glClientWaitSync( mFenceName, 0, 1 );
            if( waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED )
            {
                GLCheck( glDeleteSync( mFenceName ) );
                mFenceName = 0;
                return OR_OK;
            }
            else if(waitRet == GL_TIMEOUT_EXPIRED)
            {
                return OR_BLOCK;
            }
            else
            {
                return OR_ERROR;
            }
        }

        return OR_OK;
    }
    //-----------------------------------------------------------------------------------
    GLTextureAsync::GLTextureAsync( NCount width, NCount height, NCount depth, NCount array, Texture::Type type,
        PixelFormat pf, GLGBufferManager * mag, bool supportsGetTextureSubImage ) :
        TextureAsync( width, height, depth, array, type, pf),
        mVboName( 0 ),
        mTmpVboName( 0 ),
        mDownloadFrame( 0 ),
        mAccurateFence( 0 ),
        mMag( mag ),
        mSupportsGetTextureSubImage( supportsGetTextureSubImage )
    {
        mVboName = createBuffer( width, height, depth );
    }
    //-----------------------------------------------------------------------------------
    GLTextureAsync::~GLTextureAsync()
    {
        if( mBlockType == BT_Mapping )
            unmap();

        GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 ) );

        if( mVboName )
        {
            glDeleteBuffers( 1u, &mVboName );
            mVboName = 0;
        }
        if( mTmpVboName )
        {
            glDeleteBuffers( 1u, &mTmpVboName );
            mTmpVboName = 0;
        }

        if( mAccurateFence )
        {
            GLCheck( glDeleteSync( mAccurateFence ) );
            mAccurateFence = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    GLuint GLTextureAsync::createBuffer( NCount width, NCount height, NCount depth )
    {
        GLuint vboName;

        GLCheck( glGenBuffers( 1, &vboName ) );
        GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, vboName ) );

        const uint32 rowAlignment = 4u;
        const size_t sizeBytes = PixelUtil::getSize( width, height, depth, 1u, mFormat, rowAlignment );

        if(N_Engine().getFeature()->isSupport(CF_PersistentMapping) )
        {
            GLCheck( glBufferStorage( GL_PIXEL_PACK_BUFFER, sizeBytes, 0, GL_MAP_READ_BIT ) );
        }
        else
        {
            GLCheck( glBufferData( GL_PIXEL_PACK_BUFFER, sizeBytes, 0, GL_STREAM_READ ) );
        }

        return vboName;
    }
    //-----------------------------------------------------------------------------------
    void GLTextureAsync::writeImpl( TextureGpu * src, NCount mipLevel, PixelBlock * srcBox, NCount oft, bool notify )
    {
        TextureAsync::writeImpl( src, mipLevel, srcBox, oft, notify );

        mDownloadFrame = mMag->getFrameCount();

        if( mAccurateFence )
        {
            GLCheck( glDeleteSync( mAccurateFence ) );
            mAccurateFence = 0;
        }

        PixelBlock srcTextureBox;
        PixelBlock fullSrcTextureBox( src->getEmptyBox( mipLevel ) );

        if( !srcBox )
            srcTextureBox = fullSrcTextureBox;
        else
        {
            srcTextureBox = *srcBox;
            srcTextureBox.mRowSize   = fullSrcTextureBox.mRowSize;
            srcTextureBox.mPixelSize = fullSrcTextureBox.mPixelSize;
            srcTextureBox.mSliceSize = fullSrcTextureBox.mSliceSize;
        }

        if( src->isPoolSlice() )
        {
            fullSrcTextureBox.mArrayIdx= src->getPoolArray();
            fullSrcTextureBox.mArray = src->getPool()->mPool->getArray();

            srcTextureBox.mArrayIdx += src->getPoolArray();
        }

        const size_t mPixelSize = PixelUtil::getUnitSize( src->getFormat() );

        const GLint rowLength   = mPixelSize > 0 ? (srcTextureBox.mRowSize / mPixelSize) : 0;
        const GLint imageHeight = (srcTextureBox.mRowSize > 0) ? (srcTextureBox.mSliceSize / srcTextureBox.mRowSize) : 0;

        GLCheck( glPixelStorei( GL_PACK_ALIGNMENT, 4 ) );
        GLCheck( glPixelStorei( GL_PACK_ROW_LENGTH, rowLength) );
        GLCheck( glPixelStorei( GL_PACK_IMAGE_HEIGHT, imageHeight ) );

        const Texture::Type::Texture::Type textureType = src->getActualType();
        const PixelFormat pf = src->getFormat();

        N_assert( dynamic_cast<GLTexture*>( src ) );
        GLTexture *srcTextureGl = static_cast<GLTexture*>( src );

        const GLenum targetGl   = srcTextureGl->getGlTextureTarget();
        const GLuint texName    = srcTextureGl->getFinalTextureName();

        GLCheck( glBindTexture( targetGl, texName ) );
        GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, mVboName ) );

        GLint xPos          = static_cast<GLint>( srcTextureBox.x );
        GLint yPos          = static_cast<GLint>( srcTextureBox.y );
        GLint zPos          = static_cast<GLint>( srcTextureBox.z );
        GLint slicePos      = static_cast<GLint>( srcTextureBox.mArrayIdx );
        GLsizei width       = static_cast<GLsizei>( srcTextureBox.width );
        GLsizei height      = static_cast<GLsizei>( srcTextureBox.height );
        GLsizei depth= static_cast<GLsizei>( srcTextureBox.getVolume() );

        if( textureType == Texture::T_1DArray )
        {
            yPos            = slicePos;
            slicePos        = 1;
            height          = depth;
            depth   = 1u;
        }

        //We need to use glGetTextureSubImage & glGetCompressedTextureSubImage,
        //which is only available since GL4.5. If this isn't possible, then
        //we need to download the whole texture into a dummy temporary buffer.
        if( !fullSrcTextureBox.equalSize( srcTextureBox ) && !mSupportsGetTextureSubImage )
        {
            if( !mTmpVboName || !mSubregion.equalSize( srcTextureBox ) )
            {
                if( mTmpVboName )
                    glDeleteBuffers( 1u, &mTmpVboName );
                //Create temporary BO to hold the whole thing.
                //When mapping we'll use the subregion via
                //bytesPer* variables.
                mTmpVboName = createBuffer( fullSrcTextureBox.width, fullSrcTextureBox.height,
                                            fullSrcTextureBox.getVolume() );
            }

            GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, mTmpVboName ) );
            mSubregion = srcTextureBox;
            mSubregion.mPixelSize    = fullSrcTextureBox.mPixelSize;
            mSubregion.mRowSize      = fullSrcTextureBox.mRowSize;
            mSubregion.mSliceSize    = fullSrcTextureBox.mSliceSize;
        }
        else if( mTmpVboName )
        {
            glDeleteBuffers( 1u, &mTmpVboName );
            mTmpVboName = 0;
            mSubregion = PixelBlock();
        }

        if( !src->isWindow() )
        {
            if( fullSrcTextureBox.equalSize( srcTextureBox ) || !mSupportsGetTextureSubImage )
            {
                //We can use glGetTexImage & glGetCompressedTexImage (cubemaps need a special path)
                if( !PixelUtil::isCompress( pf ) )
                {
                    GLenum format, type;
                    GL3PlusMappings::getFormatAndType( pf, format, type );

                    if( textureType != Texture::T_CUBE )
                    {
                        GLCheck( glGetTexImage( targetGl, mipLevel, format, type, 0 ) );
                    }
                    else
                    {
                        for( size_t i=0; i<(size_t)depth; ++i )
                        {
                            const GLenum targetCubeGl = static_cast<GLenum>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
                            GLCheck( glGetTexImage( targetCubeGl, mipLevel, format, type,
                                                 reinterpret_cast<void*>(srcTextureBox.mSliceSize * i ) ) );
                        }
                    }
                }
                else
                {
                    if( textureType != Texture::T_CUBE )
                    {
                        GLCheck( glGetCompressedTexImage( targetGl, mipLevel, 0 ) );
                    }
                    else
                    {
                        for( size_t i=0; i<(size_t)depth; ++i )
                        {
                            const GLenum targetCubeGl = static_cast<GLenum>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
                            GLCheck( glGetCompressedTexImage( targetCubeGl, mipLevel,
                                                           reinterpret_cast<void*>(srcTextureBox.mSliceSize * i ) ) );
                        }
                    }
                }
            }
            else
            {
                //We need to use glGetTextureSubImage & glGetCompressedTextureSubImage,
                //which is only available since GL4.5. Support is here. Yay!
                if( !PixelUtil::isCompress( pf ) )
                {
                    //Use INT_MAX as buffer size, OpenGL already
                    //knows the size because the buffer is bound.
                    GLenum format, type;
                    GL3PlusMappings::getFormatAndType( pf, format, type );
                    GLCheck( glGetTextureSubImage( texName, mipLevel, xPos, yPos,
                                                std::max( zPos, slicePos ),
                                                width, height, depth, format, type,
                                                std::numeric_limits<int>::max(), 0 ) );
                }
                else
                {
                    GLCheck( glGetCompressedTextureSubImage( texName, mipLevel, xPos, yPos,
                                                          std::max( zPos, slicePos ),
                                                          width, height, depth,
                                                          std::numeric_limits<int>::max(), 0 ) );
                }
            }
        }
        else
        {
            GLenum format, type;
            GL3PlusMappings::getFormatAndType( pf, format, type );
            GLCheck( glReadPixels( xPos, yPos, width, height, format, type, 0 ) );
        }

        if( notify )
        {
            GLCheck( mAccurateFence = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
        }
    }
    //-----------------------------------------------------------------------------------
    PixelBlock GLTextureAsync::mapImpl(NCount depthIdx, NCount arrayIdx, NCount oft)
    {
        waitForDownloadToFinish();

        PixelBlock re;

        GLuint vboName = mTmpVboName ? mTmpVboName : mVboName;
        GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, vboName ) );

        size_t sizeBytes = 0;
        const uint32 rowAlignment = 4u;

        if( mTmpVboName )
        {
            re = mSubregion;
            sizeBytes = re.mSliceSize * std::max( mSubregion.getBack(), mSubregion.getMaxArray() );
        }
        else
        {
            re = PixelBlock( mWidth, mHeight, getDepth(), getArray(),
                                 PixelUtil::getUnitSize( mFormat ),
                                 getRowSize(), getSliceSize() );
            sizeBytes = PixelUtil::getSize( mWidth, mHeight, mDepth, mArray, mFormat, rowAlignment );
        }

        //if( PixelUtil::isCompress( mFormat ) )
            re.setFormat( mFormat );

        re.data = glMapBufferRange( GL_PIXEL_PACK_BUFFER, 0, sizeBytes, GL_MAP_READ_BIT );

        if( mTmpVboName )
        {
            //Offset to the beginning of the region.
            re.data = re.getOriginData( re.x, re.y, std::max( re.z, re.mArrayIdx + depthIdx));
            re.x = 0;
            re.y = 0;
            re.z = 0;
            re.mArrayIdx = 0;
        }
        else
        {
            re.data = re.getOriginData( 0, 0, depthIdx );
            re.mArray -= depthIdx;
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    void GLTextureAsync::unmapImpl(void)
    {
        GLuint vboName = mTmpVboName ? mTmpVboName : mVboName;
        GLCheck( glBindBuffer( GL_PIXEL_PACK_BUFFER, vboName ) );
        GLCheck( glUnmapBuffer( GL_PIXEL_PACK_BUFFER ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTextureAsync::waitForDownloadToFinish(void)
    {
        if( mBlockType != BT_Writting )
            return; //We're done.

        if( mAccurateFence )
        {
            mAccurateFence = GLGBufferManager::waitFor( mAccurateFence );
        }
        else
        {
            mMag->wait( mDownloadFrame );
            mQueryCount = 0;
        }

        mBlockType = BT_None;
    }
    //-----------------------------------------------------------------------------------
    bool GLTextureAsync::isDataValid(void)
    {
        if( !TextureAsync::isDataValid() )
        {
            //Early out. The texture is not even finished being ready.
            //We didn't even start the actual download.
            return false;
        }

        bool retVal = false;

        if( mBlockType != BT_Writting )
        {
            retVal = true;
        }
        else if( mAccurateFence )
        {
            //Ask GL API to return immediately and tells us about the fence
            GLenum waitRet = glClientWaitSync( mAccurateFence, 0, 0 );
            if( waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED )
            {
                GLCheck( glDeleteSync( mAccurateFence ) );
                mAccurateFence = 0;
                if( mBlockType != BT_Mapping )
                    mBlockType = BT_None;
                retVal = true;
            }
        }
        else
        {
            if( mDownloadFrame == mMag->getFrameCount() )
            {
                if( mQueryCount > 3 )
                {
                    //Use is not calling vaoManager->update(). Likely it's stuck in an
                    //infinite loop checking if we're done, but we'll always return false.
                    //If so, switch to accurate tracking.
                    GLCheck( mAccurateFence = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
                    GLCheck( glFlush() );

                    LogManager::getSingleton().logMessage(
                                "WARNING: Calling TextureAsync::isDataValid too "
                                "often with innacurate tracking in the same frame this transfer "
                                "was issued. Switching to accurate tracking. If this is an accident, "
                                "wait until you've rendered a few frames before checking if it's done. "
                                "If this is on purpose, consider calling TextureAsync::download()"
                                " with accurate tracking enabled.", LML_CRITICAL );
                }

                ++mQueryCount;
            }

            //We're downloading but have no fence. That means we don't have accurate tracking.
            retVal = mMag->isFrameFinished( mDownloadFrame );
            ++mQueryCount;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
}
