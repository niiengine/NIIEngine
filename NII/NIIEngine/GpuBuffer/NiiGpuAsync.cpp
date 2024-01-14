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

#include "NiiPreProcess.h"

namespace NII
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // GpuBufferAsync
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GpuBufferAsync::GpuBufferAsync(GpuBuffer * src, NCount oft, NCount size) :
        mMapping(false),
        mSrc(src),
        mOffset(oft),
        mSize(size)
    {
        mMag->request(mMapBuffer, mSize, M_DEVWRITE);
        
        mMapOffset = mMapBuffer->asyncWrite(src, mOffset, mSize);
    }
    //-----------------------------------------------------------------------------------
    GpuBufferAsync::GpuBufferAsync(GpuBuffer * src, MappedBuffer * buf, NCount oft, NCount size) :
        mMapping( false ),
        mSrc( src ),
        mMapBuffer( buf ),
        mOffset( oft ),
        mSize( size )
    {
        mMapOffset = mMapBuffer->asyncWrite(src, mOffset, mSize);
    }
    //-----------------------------------------------------------------------------------
    GpuBufferAsync::~GpuBufferAsync()
    {
        if(mMapBuffer->isMapping())
        {
            mMapBuffer->unmap();
        }

        mMapBuffer->removeRef();
        mMapBuffer = 0;
    }
    //-----------------------------------------------------------------------------------
    const void * GpuBufferAsync::map()
    {
        assert( !mMapping );
        mMapping = true;
        return mapImpl();
    }
    //-----------------------------------------------------------------------------------
    void GpuBufferAsync::unmap()
    {
        mMapBuffer->unmap();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TextureAsync
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    TextureAsync::TextureAsync(NCount width, NCount height, NCount depth, NCount array, Texture::Type type, PixelFormat pf) :
        mBlockType( BT_None),
        mWidth(width),
        mHeight(height),
        mDepth(depth),
        mArray(array),
        mTextureType(type),
        mFormat(pf),
        mQueryCount(0)
    {
    }
    //-----------------------------------------------------------------------------------
    TextureAsync::~TextureAsync()
    {
        if(mBlockType == BT_Mapping)
            unmap();

        OperationList::const_iterator i, iend = mOpList.end();
        for(i = mOpList.begin(); i != iend; ++i)
        {
            (*i).mOp->removeListener(this);
        }
        mOpList.clear();
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::writeImpl(Texture * src, NCount mipmap, PixelBlock * srcBox, NCount oft, bool notify)
    {
        PixelBlock srcTextureBox;
        const PixelBlock fullSrcTextureBox(src->getEmptyBox(mipmap));

        if( srcBox )
        {
            srcTextureBox = *srcBox;
            srcTextureBox.mRowSize   = fullSrcTextureBox.mRowSize;
            srcTextureBox.mUnitSize = fullSrcTextureBox.mUnitSize;
            srcTextureBox.mSliceSize = fullSrcTextureBox.mSliceSize;
        }
        else
        {
            srcTextureBox = fullSrcTextureBox;
        }


        assert( mipmap < src->getMipmapCount() );
        assert( mFormat == PixelUtil::getFamily( src->getFormat() ) );
        assert( fullSrcTextureBox.contains( srcTextureBox ) );
        assert( srcTextureBox.width == mWidth );
        assert( srcTextureBox.height == mHeight );
        assert( srcTextureBox.getVolume() == mDepth * mArray );
        assert( (!src->isMultiSample() || !src->isManualMSAA() || src->isOpenGLRenderWindow()) &&
                "Cannot download from an explicitly resolved MSAA texture!" );
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::readImpl(Texture * dst, NCount dstmipmap, PixelBlock *dstBox, bool notify)
    {
        
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::process( const Item & item )
    {
        OperationList::const_iterator i, iend = mOpList.end();
        for(i = mOpList.begin(); i != iend; ++i)
        {
            const Operation & temp = *i;
            if(temp.mOp == item.texture)
            {
                if( itemp.reason == O_Valid )
                {
                    temp.mOp->removeListener( this );
                    writeImpl( temp.mOp, temp.mMipMap, temp.mOpSrc.isEmpty() ? 0 : &temp.mOpSrc, temp.mOffset, temp.mNotify);
                    mOpList.erase(i);
                    break;
                }
                else if( item.reason == O_OutGpu )
                {
                    temp.mOp->removeListener( this );
                    LogManager::getSingleton().logMessage(
                                        "WARNING: TextureAsync was waiting on texture to become "
                                        "ready to download its contents, but Texture '" + texture->getName() +
                                        "' lost residency!", LML_CRITICAL );
                    mOpList.erase(i);
                    if(mOpList.empty())
                        mBlockType = BT_None;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::write( Texture * src, NCount mipmap, PixelBlock * srcBox, NCount oft, bool notify)
    {
        OperationList::const_iterator i, iend = mOpList.end();
        for(i = mOpList.begin(); i != iend; ++i)
        {
            const Operation & temp = *i;
            if( oft == temp.mOffset)
            {
                temp.mOp->removeListener( this );
                mOpList.erase(i);
                break;
            }
        }

        if( mBlockType == BT_Mapping )
        {
            N_EXCEPT( Exception::ERR_INVALID_STATE, "Cannot download to mapped texture. You must call unmap first!" );
        }

        if( ((!src->isValidImpl() && src->getCurrentStorage() == StorageType::ST_Gpu)) ||
            (!src->isValid() && src->getStorage() == StorageType::ST_Gpu &&
            src->getTaskCount() <= 1u) )
        {
            //Texture is not resident but soon will be, or is resident but not yet ready.
            //Register ourselves to listen for when that happens, we'll download then.
            src->addListener( this );
            mOpList.empalce_back(Operation( src, mipmap, notify, srcBox ));
        }
        else if(src->getCurrentStorage() != StorageType::ST_Gpu)
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                         "Only Resident textures can be downloaded via TextureAsync. "
                         "Trying to download texture: '" + src->getName() + "'" );
        }
        else
        {
            writeImpl(src, mipmap, srcBox, oft, notify);
        }

        mQueryCount = 0;

        mBlockType = BT_Writting;
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::read(Texture * dst, NCount dstmipmap, PixelBlock * dstBox, NCount oft, bool notify)
    {
        
    }
    //-----------------------------------------------------------------------------------
    PixelBlock TextureAsync::map(NCount depthIdx, NCount arrayIdx, NCount oft)
    {
        assert( mBlockType != BT_Mapping );
        assert( depthIdx < getDepth() );
        assert( arrayIdx < getArray() );
        
        OperationList::const_iterator i, iend = mOpList.end();
        for(i = mOpList.begin(); i != iend; ++i)
        {
            const Operation & temp = *i;
            if(temp.mOffset == oft)
                temp.mOp->wait(false);
        }

        PixelBlock re = mapImpl(depthIdx, arrayIdx, oft);
        mBlockType = BT_Mapping;

        re.setFormat(mFormat);

        return re;
    }
    //-----------------------------------------------------------------------------------
    void TextureAsync::unmap()
    {
        assert(mBlockType == BT_Mapping);
        unmapImpl();
        mBlockType = BT_None;
    }
    //-----------------------------------------------------------------------------------
    bool TextureAsync::isDataValid(NCount oft)
    {
        OperationList::const_iterator i, iend = mOpList.end();
        for(i = mOpList.begin(); i != iend; ++i)
        {
            if((*i).mOffset == oft)
                return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------------------
}
