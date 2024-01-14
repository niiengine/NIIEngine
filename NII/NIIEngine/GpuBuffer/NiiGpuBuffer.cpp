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
#include "NiiGpuBuffer.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiMemGBufferManager.h"
#include "NiiGBufferManager.h"
#include "NiiEngine.h"
#include "NiiImage.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IndexBuffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    IndexBuffer::IndexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, unitsize, unitcount, multi, mode, initData, gid, goft, padSize)
    {
        if (mMark & M_HostCache)
        {
            mShadow = N_new MemIndexBuffer(mag, unitsize, unitcount, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //------------------------------------------------------------------------
    IndexBuffer::~IndexBuffer()
    {
        if (mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //------------------------------------------------------------------------
    Buffer * IndexBuffer::clone(BufferModeMark m) const
    {
        IndexBuffer * re;
        
        if(m == -1)
            m = mMark;
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
                
        return re;
    }
    //------------------------------------------------------------------------
    Nui8 * IndexBuffer::getHostData() const
    {
        if(mShadow)
            return static_cast<MemIndexBuffer *>(mShadow)->getHostData();
        
        return 0;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VertexBuffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VertexBuffer::VertexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, unitsize, unitcount, multi, mode, initData, gid, goft, padSize)
    {
        mMark &= ~M_INSTANCE;
        if(mMark & M_HostCache)
        {
            mShadow = N_new MemVertexBuffer(mag, unitsize, unitcount, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //------------------------------------------------------------------------
    VertexBuffer::~VertexBuffer()
    {
        if(mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //------------------------------------------------------------------------
    Buffer * VertexBuffer::clone(BufferModeMark m) const
    {
        VertexBuffer * re;
        
        if(m == -1)
            m = mMark;
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        
        return re;
    }
    //------------------------------------------------------------------------
    Nui8 * VertexBuffer::getHostData() const
    {
        if(mShadow)
            return static_cast<MemVertexBuffer *>(mShadow)->getHostData();
        
        return 0;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CounterBuffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CounterBuffer::CounterBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, 1, size, multi, mode, initData, gid, goft, padSize)
    {
        if (mMark & M_HostCache)
        {
            mShadow = N_new MemCounterBuffer(mag, size, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //-----------------------------------------------------------------------------
    CounterBuffer::~CounterBuffer()
    {
        if(mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //-----------------------------------------------------------------------------
    Buffer * CounterBuffer::clone(BufferModeMark m) const
    {
        CounterBuffer * re;
        
        if(m == -1)
            m = mMark;
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        
        return re;
    }
    //-----------------------------------------------------------------------------
    Nui8 * CounterBuffer::getHostData() const
    {
        if(mShaodw)
            return static_cast<MemCounterBuffer *>(mShadow)->getHostData();
        
        return 0;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // StructBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    StructBuffer::StructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) : 
        GpuBuffer(mag, 1, size, multi, mode, initData, gid, goft, padSize)
    {
        if (mMark & M_HostCache)
        {
            mShadow = N_new MemStructBuffer(mag, size, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //-----------------------------------------------------------------------------
    StructBuffer::~StructBuffer()
    {
        if (mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //-----------------------------------------------------------------------------
    Buffer * StructBuffer::clone(BufferModeMark m) const
    {
        StructBuffer * re;
        if(m == -1)
            m = mMark;
        
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        
        return re;
    }
    //-----------------------------------------------------------------------------
    Nui8 * StructBuffer::getHostData() const
    {
        if(mShadow)
            return static_cast<MemStructBuffer *>(mShadow)->getHostData();
        return 0;
    }
    //-----------------------------------------------------------------------------
    /*
    bool StructBuffer::writeParams(GpuProgramParam * params)
    {
        // Lock buffer
        void* mappedData = this->lock(Buffer::HBL_DISCARD);
        if (!mappedData)
        {
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "Cannot update uniform buffer\nError description: error locking uniform buffer",
                "StructBuffer::writeParams");
        }

        // Store temporary data address
        void* srcData = 0;

        // Iterate through variables
        ShaderVariableIterator it = mShaderVars.begin();
        ShaderVariableIterator end = mShaderVars.end();
        while(it != end)
        {
            String varName = it->name;

            // hack for cg parameter with strange prefix
            if (varName.size() > 0 && varName[0] == '_')
            {
                varName.erase(0,1);
            }

            const GpuParamUnit& def = params->getConstantDefinition(varName);
            if (def.isFloat())
            {
                srcData = (void *)&(*(params->getFloatConstantList().begin() + def.physicalIndex));
            }
            else
            {
                srcData = (void *)&(*(params->getIntConstantList().begin() + def.physicalIndex));
            }

            memcpy( &(((char *)(mappedData))[it->startOffset]), srcData , it->size);
        }

        // Unlock buffer
        this->unlock();

        return true;
    }
    //-----------------------------------------------------------------------------
    bool StructBuffer::writeSharedParams(GpuSharedParametersPtr sharedParams)
    {
        // Lock buffer
        void* mappedData = this->lock(Buffer::HBL_DISCARD);
        if (!mappedData)
        {
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "Cannot update uniform buffer\nError description: error locking uniform buffer",
                "StructBuffer::writeParams");
        }

        // Store temporary data address
        void* srcData = 0;

        // Iterate through variables
        ShaderVariableIterator it = mShaderVars.begin();
        ShaderVariableIterator end = mShaderVars.end();
        while(it != end)
        {
            String varName = it->name;

            // hack for cg parameter with strange prefix
            if (varName.size() > 0 && varName[0] == '_')
            {
                varName.erase(0,1);
            }

            const GpuParamUnit& def = sharedParams->getConstantDefinition(varName);
            if (def.isFloat())
            {
                srcData = (void *)&(*(sharedParams->getFloatConstantList().begin() + def.physicalIndex));
            }
            else
            {
                srcData = (void *)&(*(sharedParams->getIntConstantList().begin() + def.physicalIndex));
            }

            memcpy( &(((char *)(mappedData))[it->startOffset]), srcData , it->size);
        }

        // Unlock buffer
        this->unlock();

        return true;
    }
    */
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // FrameBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    FrameBuffer::FrameBuffer(BufferManager * mag, NCount w, NCount h, NCount d,
        PixelFormat pf, NCount multi, BufferModeMark mode, GroupID gid, NCount goft, NCount padSize):
            Buffer(mag, PixelUtil::getUnitSize(mFormat), mHeight * mWidth, multi, mode, gid, goft, padSize),
            mWidth(w),
            mHeight(h),
            mDepth(d),
            mFormat(pf)
    {
        mRowSize = mWidth;
        mSliceSize = mUnitCount;
    }
    //-----------------------------------------------------------------------------
    FrameBuffer::~FrameBuffer()
    {
    }
    //-----------------------------------------------------------------------------
    const PixelBlock & FrameBuffer::lock(const Box & lockBox, MuteMark m)
    {
        if(mMark & M_HostCache && mShadow)
        {
            if(m & MM_WRITE)
            {
                mShadowRefresh = true;
            }
            mMutexData = static_cast<FrameBuffer *>(mShadow)->lock(lockBox, m);
        }
        else
        {
            mMutexData = lockImpl(m, lockBox);
            mMapping = true;
        }
        return mMutexData;
    }
    //-----------------------------------------------------------------------------
    void * FrameBuffer::lock(MuteMark m, NCount oft, NCount size)
    {
        N_assert(!isMapping(), "Cannot lock this buffer, it is already locked!");
        N_assert(oft == 0 && size == mSize, "Cannot lock memory region, most lock box or entire buffer");

        Box myBox(0, 0, 0, mWidth, mHeight, mDepth);
        //调用像素缓存的锁定
        const PixelBlock & rv = lock(myBox, m);
        return rv.mData;
    }
    //-----------------------------------------------------------------------------
    const PixelBlock & FrameBuffer::getMutexData() const
    {
        N_assert(isMapping(), "Cannot get current lock: buffer not locked");

        return mMutexData;
    }
    //-----------------------------------------------------------------------------
    void * FrameBuffer::lockImpl(MuteMark m, NCount oft, NCount size)
    {
        N_EXCEPT(Internal, _I18n("lockImpl(oft,size) is not valid for FrameBuffers and should never be called"));
    }
    //-----------------------------------------------------------------------------
    void FrameBuffer::write(const FrameBuffer * src, const Box & srcBox, const Box & dstBox)
    {
        if(isMapping() || src->isMapping())
        {
            N_EXCEPT(Internal, _I18n("Source and destination buffer may not be locked!"));
        }
        if(src == this)
        {
            N_EXCEPT( InvalidParam, _I18n("Source must not be the same object"));
        }


        const PixelBlock & srclock = const_cast<FrameBuffer *>(src)->lock(srcBox, MM_READ);
        MuteMark method = MM_READ | MM_WRITE;
        // 比较是否对整个进行操作
        if(dstBox.mLeft == 0 && dstBox.mTop == 0 && dstBox.mFront == 0 &&
                dstBox.mRight == mWidth && dstBox.mBottom == mHeight && dstBox.mBack == mDepth)
            method = MM_WHOLE;
        const PixelBlock & dstlock = lock(dstBox, method);

        // 比较是否复制区于被复制区大小是否相同
        if(dstlock.getWidth() != srclock.getWidth() ||
            dstlock.getHeight() != srclock.getHeight() ||
                dstlock.getDepth() != srclock.getDepth())
        {
            PixelBlock::scale(srclock, dstlock);
        }
        else
        {
            PixelUtil::conv(srclock, dstlock);
        }

        unlock();
        const_cast<FrameBuffer *>(src)->unlock();
    }
    //-----------------------------------------------------------------------------
    void FrameBuffer::write(const FrameBuffer * src)
    {
        write(src, Box(0, 0, 0, src->getWidth(), src->getHeight(), src->getDepth()),
            Box(0, 0, 0, mWidth, mHeight, mDepth));
    }
    //-----------------------------------------------------------------------------
    void FrameBuffer::read(void * dest, NCount oft, NCount size)
    {
        N_EXCEPT(UnImpl, _I18n("Reading a byte range is not implemented. Use read."));
    }
    //-----------------------------------------------------------------------------
    void FrameBuffer::write(const void * src, NCount oft, NCount size)
    {
        N_EXCEPT(UnImpl, _I18n("Writing a byte range is not implemented. Use write."));
    }
    //-----------------------------------------------------------------------------
    TextureFrame * FrameBuffer::getBuffer(Nidx)
    {
        N_EXCEPT(UnImpl, _I18n("Not yet implemented for this rendersystem."));
    }
    //----------------------------------------------------------------------------
    void FrameBuffer::destroyImpl(TextureFrame *, Nidx)
    {
    }
    //------------------------------------------------------------------------------
    Nui8 * FrameBuffer::getHostData() const
    {
        return static_cast<FrameBuffer *>(mShadow)->getHostData();
    }
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    // GpuBuffer
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    GpuBuffer::GpuBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        Buffer(mag, unitsize, unitcount, multi, mode, initData, gid, goft, padSize),
        mIdx(-1)
    {

    }
    //------------------------------------------------------------------------------
    void GpuBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {

    }
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    // IndirectBuffer
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    IndirectBuffer::IndirectBuffer(BufferManager * mag, Type type, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, 1, size, multi, mode, initData, gid, goft, padSize),
        mType(type)
    {
        if(mMark & M_HostCache)
        {
            mShadow = N_new MemIndirectBuffer(mMag, type, size, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //-----------------------------------------------------------------------------
    IndirectBuffer::~IndirectBuffer()
    {
        if(mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //-----------------------------------------------------------------------------
    Buffer * IndirectBuffer::clone(BufferModeMark m) const
    {
        IndirectBuffer * re;
        if(m == -1)
            m = mMark;
        
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mType, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mType, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        return re;
    }
    //------------------------------------------------------------------------------
    Nui8 * IndirectBuffer::getHostData() const
    {
        return static_cast<MemIndirectBuffer *>(mShadow)->getHostData();
    }
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    // TextureBuffer
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    TextureBuffer::TextureBuffer(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, 1, size, multi, mode, initData, gid, goft, padSize),
        mPixelFormat(pf)
    {
        if(mMark & M_HostCache)
        {
            mShadow = N_new MemTextureBuffer(mag, size, pf, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //-----------------------------------------------------------------------------
    TextureBuffer::~TextureBuffer()
    {
        if(mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //------------------------------------------------------------------------------
    Buffer * TextureBuffer::clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const
    {
        TextureBuffer * re;
        if(m == -1)
            m = mMark;
        
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mPixelFormat, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitCount, mPixelFormat, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        return re;
    }
    //------------------------------------------------------------------------------
    Nui8 * TextureBuffer::getHostData() const
    {
        return static_cast<MemTextureBuffer *>(mShadow)->getHostData();
    }
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    // StorageBuffer
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    StorageBuffer::StorageBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        GpuBuffer(mag, unitsize, unitcount, multi, mode, initData, gid, goft, padSize)
    {
        if(mMark & M_HostCache)
        {
            mShadow = N_new MemStorageBuffer(mag, unitsize, unitcount, Buffer::M_HOSTREAD | Buffer::M_HOSTWRITE, initData);
        }
    }
    //-----------------------------------------------------------------------------------
    StorageBuffer::~StorageBuffer()
    {
        destroyAllView();
        if(mMag)
        {
            static_cast<GBufferManager *>(mMag)->onDestroy(this);
        }
    }
    //-----------------------------------------------------------------------------------
    IndexBuffer* StorageBuffer::createIndexView( NCount usize, NCount oft, NCount count )
    {
        N_assert1( mBindFlags & BB_FLAG_TEX && "Buffer must've been created with BB_FLAG_TEX" );

        IndexBufferList::const_iterator itor = mIndexBuffer.begin();
        IndexBufferList::const_iterator end  = mIndexBuffer.end();
        
        while( itor != end)
        {
            if( (*itor)->getUnitSize() == usize )
                return *itor;
            ++itor;
        }

        return createViewImpl( pf );
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyIndexView( NCount usize, NCount oft, NCount count )
    {
        IndexBufferList::const_iterator itor = mIndexBuffer.begin();
        IndexBufferList::const_iterator end  = mIndexBuffer.end();

        while( itor != end && (*itor)->getUnitSize() != usize )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyAllIndexView()
    {
        IndexBufferList::const_iterator itor = mIndexBuffer.begin();
        IndexBufferList::const_iterator end  = mIndexBuffer.end();

        while( itor != end )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    IndexBuffer* StorageBuffer::createIndirectView( IndirectBuffer::Type type, NCount oft, NCount count )
    {
        N_assert1( mBindFlags & BB_FLAG_TEX && "Buffer must've been created with BB_FLAG_TEX" );

        IndirectBufferList::const_iterator itor = mIndirectList.begin();
        IndirectBufferList::const_iterator end  = mIndirectList.end();
        
        while( itor != end)
        {
            if( (*itor)->getType() == type )
                return *itor;
            ++itor;
        }

        return createViewImpl( pf );
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyIndirectView( IndirectBuffer::Type type, NCount oft, NCount count )
    {
        IndirectBufferList::const_iterator itor = mIndirectList.begin();
        IndirectBufferList::const_iterator end  = mIndirectList.end();

        while( itor != end && (*itor)->getType() != type )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyAllIndirectView()
    {
        IndirectBufferList::const_iterator itor = mIndirectList.begin();
        IndirectBufferList::const_iterator end  = mIndirectList.end();

        while( itor != end )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* StorageBuffer::createTextureView( PixelFormat pf, NCount oft, NCount count)
    {
        N_assert1( mBindFlags & BB_FLAG_TEX);

        TextureBufferList::const_iterator itor = mTextureList.begin();
        TextureBufferList::const_iterator end  = mTextureList.end();
        while( itor != end)
        {
            if( (*itor)->getFormat() == pf )
            {
                return *itor;
            }
            ++itor;
        }

        return createViewImpl( pf );
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyTextureView( PixelFormat pf, NCount oft, NCount count)
    {
        TextureBufferList::const_iterator itor = mTextureList.begin();
        TextureBufferList::const_iterator end  = mTextureList.end();

        while( itor != end && (*itor)->getFormat() != pf )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyAllTextureView( PixelFormat pf )
    {
        TextureBufferList::const_iterator itor = mTextureList.begin();
        TextureBufferList::const_iterator end  = mTextureList.end();

        while( itor != end )
            ++itor;

        if( itor != end )
        {
            delete *itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void StorageBuffer::destroyAllView(void)
    {
        {
            IndirectBufferList::const_iterator itor = mIndirectList.begin();
            IndirectBufferList::const_iterator end  = mIndirectList.end();

            while( itor != end )
            {
                delete *itor;
                ++itor;
            }
        }
        {
            IndexBufferList::const_iterator itor = mIndexBuffer.begin();
            IndexBufferList::const_iterator end  = mIndexBuffer.end();

            while( itor != end )
            {
                delete *itor;
                ++itor;
            }
        }
        {
            TextureBufferList::const_iterator itor = mTextureList.begin();
            TextureBufferList::const_iterator end  = mTextureList.end();

            while( itor != end )
            {
                delete *itor;
                ++itor;
            }
        }
    }
    //------------------------------------------------------------------------------
    Buffer * StorageBuffer::clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const
    {
        StorageBuffer * re;
        if(m == -1)
            m = mMark;
        
        if(getHostData())
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, getHostData(), mGroupID);
        }
        else
        {
            static_cast<GBufferManager *>(mMag)->create(re, mUnitSize, mUnitCount, mMultiCount, m, 0, mGroupID);
            re->write(this, 0, 0, getSize());
        }
        return re;
    }
    //------------------------------------------------------------------------------
    Nui8 * StorageBuffer::getHostData() const
    {
        return static_cast<MemStorageBuffer *>(mShadow)->getHostData();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MappedBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    void mergeContiguousBlocks(BlockList::iterator blockToMerge, BlockList & blocks)
    {
        BlockList::iterator itor = blocks.begin();
        BlockList::iterator end = blocks.end();

        while (itor != end)
        {
            if (itor->end == blockToMerge->start)
            {
                itor->end = blockToMerge->end;
                NCount idx = itor - blocks.begin();

                //When blockToMerge is the last one, its index won't be the same
                //after removing the other iterator, they will swap.
                if (idx == blocks.size() - 1)
                    idx = blockToMerge - blocks.begin();

                remove(blocks, blockToMerge);

                blockToMerge = blocks.begin() + idx;
                itor = blocks.begin();
                end = blocks.end();
            }
            else if (blockToMerge->end == itor->start)
            {
                blockToMerge->end = itor->end;
                NCount idx = blockToMerge - blocks.begin();

                //When blockToMerge is the last one, its index won't be the same
                //after removing the other iterator, they will swap.
                if (idx == blocks.size() - 1)
                    idx = itor - blocks.begin();

                remove(blocks, itor);

                blockToMerge = blocks.begin() + idx;
                itor = blocks.begin();
                end = blocks.end();
            }
            else
            {
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    MappedBuffer::MappedBuffer(GBufferManager * mag, BufferModeMark mode, NCount offset, NCount size) :
        mOffset(offset),
        mSizeBytes(size),
        mLastOp(BOT_Free),
        mMag(mag),
        mMark(mode),
        mMapping(false),
        mMappingStart(0),
        mLastUploadOffset(0),
        mMappingCount(0),
        mRefCount(1),
        //mUnfenceTimeThreshold(mag->getDefaultStagingBufferUnfencedTime()),
        //mLifetimeThreshold(mag->getDefaultStagingBufferLifetime()),
        mLastTime(mag->getTimer()->getMilliseconds())
    {
        //if (!writeOnly)
            mFreeBlockList.push_back(Block(0, mSizeBytes));
    }
    //-----------------------------------------------------------------------------------
    MappedBuffer::~MappedBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    BlockType MappedBuffer::request(NCount size, bool upload)
    {
        if (upload)
        {
            return BT_Part;
        }
        else
        {
            BlockList::const_iterator itor = mFreeBlockList.begin();
            BlockList::const_iterator end = mFreeBlockList.end();

            while (itor != end && size > itor->size())
                ++itor;

            return itor != end ? BT_None : BT_All;
        }
        return BT_All;
    }
    //-----------------------------------------------------------------------------------
    void* MappedBuffer::writeMap(NCount size)
    {
        N_assert1(mLastOp == BOT_Free);
        mLastOp = BOT_Write;
        if (mMapping)
        {
            N_EXCEPT(InvalidState, "Buffer already mapped!");
        }

        if (size > mSizeBytes || !size)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "MappedBuffer (" + StrConv::conv(mSizeBytes) +
                " bytes) is smaller than the mapping request (" + StrConv::conv(size) + ")");
        }

        mMapping = true;
        return writeMapImpl(size);
    }
    //-----------------------------------------------------------------------------------
    void MappedBuffer::unmap()
    {
        if(!mMapping)
        {
            N_EXCEPT(InvalidState, "Unmapping an unmapped buffer!");
        }
        
        mLastUploadOffset = mMappingStart;

        unmapImpl();

        mMapping = false;
        mMappingStart += mMappingCount;

        if(mMappingStart >= mSizeBytes)
            mMappingStart = 0;
        
        mLastOp = BOT_Free;
    }
    //-----------------------------------------------------------------------------------
    void MappedBuffer::ref()
    {
        ++mRefCount;

        if (mRefCount == 1)
            mMag->onRef(this);
    }
    //-----------------------------------------------------------------------------------
    void MappedBuffer::unref()
    {
        Timer * timer = mMag->getTimer();
        --mRefCount;

        N_assert1(mRefCount >= 0);
        mLastTime = timer->getMilliseconds();

        if (mRefCount == 0)
            mMag->onUnRef(this);
    }
    //-----------------------------------------------------------------------------------
    void MappedBuffer::recover(NCount offset, NCount size)
    {
        //Put the mapped region back to our records as "available" for subsequent async
        Block mappedArea(offset, offset + size);
#if N_DEBUG
        BlockList::const_iterator itor = mFreeBlockList.begin();
        BlockList::const_iterator end = mFreeBlockList.end();

        while (itor != end)
        {
            N_assert1(!itor->overlaps(mappedArea) && "Already called readMapImpl on this area (or part of it!) before!");
            ++itor;
        }
#endif

        mFreeBlockList.push_back(mappedArea);

        mergeContiguousBlocks(mFreeBlockList.end() - 1, mFreeBlockList);
    }
    //-----------------------------------------------------------------------------------
    const void * MappedBuffer::readMap(NCount offset, NCount size)
    {
        N_assert1(mLastOp == BOT_Free);
        mLastOp = BOT_Read;
        if (mMapping)
        {
            N_EXCEPT(InvalidState, "Buffer already mapped!");
        }

        mMapping = true;

        return readMapImpl(offset, size);
    }
    //-----------------------------------------------------------------------------------
    NCount MappedBuffer::requestImpl(NCount length, bool upload)
    {
        //Grab the smallest region that fits the request.
        NCount lowestLength = std::numeric_limits<NCount>::max();
        BlockList::iterator itor = mFreeBlockList.begin();
        BlockList::iterator end = mFreeBlockList.end();

        BlockList::iterator itLowest = end;

        while (itor != end)
        {
            NCount freeRegionLength = itor->length();
            if (length <= freeRegionLength && freeRegionLength < lowestLength)
            {
                itLowest = itor;
                lowestLength = freeRegionLength;
            }

            ++itor;
        }

        NCount retVal = -1;

        if (itLowest != end)
        {
            //Got a region! Shrink our records
            retVal = itLowest->start;
            itLowest->start += length;

            //This region is empty. Remove it.
            if (itLowest->start == itLowest->end)
                remove(mFreeBlockList, itLowest);
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MappedTexture
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MappedTexture::MappedTexture(GBufferManager * mag, PixelFormat pf, NCount size, NCount groupOft, NCount groupIdx) :
        mMag(mag),
        mLastFrame(mag->getFrameCount() - mag->getDynamicBufferMultiplier() ),
        mFormat(pf),
        mOffset(groupOft),
        mMappedPtr(0),
        mCurrentOffset(0),
        mSize(size),
        mGroupIndex(groupIdx)
        mMapping(false)
  //#if OGRE_DEBUG_MODE
  //      ,mUserQueriedIfUploadWillStall( false )
  //#endif
    {
    }
    //-----------------------------------------------------------------------------------
    MappedTexture::~MappedTexture()
    {
    }
    //-----------------------------------------------------------------------------------
    bool MappedTexture::isBlock()
    {
//#if OGRE_DEBUG_MODE
//        mUserQueriedIfUploadWillStall = true;
//#endif
        return !mMag->isFrameFinished( mLastFrame );
    }
    //-----------------------------------------------------------------------------------
    PixelBlock MappedTexture::map( const PixelVolume & vol )
    {
        N_assert1( request(vol) );
        if( (vol.mDepth > 1u || vol.mArray > 1u) && (vol.mWidth > 2048 || vol.mHeight > 2048) )
        {
            N_EXCEPT(Internal, "error" );
        }

        PixelBlock re = mapImpl( vol );
        re.setFormat(vol.mFormat);

        return re;
    }
    //-----------------------------------------------------------------------------------
    void MappedTexture::unmap()
    {
        N_assert1( mMapping && "error" );
        mMapping = false;
    }
    //-----------------------------------------------------------------------------------
    void MappedTexture::read(Texture * dst, NCount dstmipmap, const PixelBlock *dstBox, const PixelBlock &srcBox, bool host)
    {
        N_assert1( !mMapping && "You must call unmap before you can upload!" );
//#if OGRE_DEBUG_MODE
//        mUserQueriedIfUploadWillStall = false;
//#endif
        const PixelBlock fullDstTextureBox( std::max( 1u, dst->getWidth() >> dstmipmap ),
                                            std::max( 1u, dst->getHeight() >> dstmipmap ),
                                            std::max( 1u, dst->getDepth() >> dstmipmap ),
                                            dst->getArray(),
                                            PixelUtil::getUnitSize(dst->getFormat()),
                                            dst->getRowSize( dstmipmap ),
                                            dst->getSliceSize( dstmipmap ) );

        N_assert1( !dstBox || srcBox.equalVolume( *dstBox ) && "Src & Dst must be equal" );
        N_assert1( !dstBox || fullDstTextureBox.contains( *dstBox ) );
        N_assert1( dstmipmap < dst->getMipmapCount() );
        N_assert1(( !srcBox.mRowSize || (srcBox.mSliceSize % srcBox.mRowSize) == 0) && "srcBox.mSliceSize must be a multiple of srcBox.mRowSize!" );

        if( dst->isMultiSample() )
        {
            N_EXCEPT(Internal, "Cannot upload to texture '" + dst->getName() + "' because it's using MSAA");
        }

        if( dst->getCurrentStorage() == StorageType::ST_Disk )
        {
            N_EXCEPT(Internal, "Cannot upload to texture '" + dst->getName() + "' which is in StorageType::ST_Disk mode" );
        }

        if(host )
        {
            if( dst->getCurrentStorage() == StorageType::ST_Cpu || dst->getRecoverType() == Texture::RT_CpuGpu )
            {
                N_EXCEPT(Internal, "Cannot upload to texture '" + dst->getName() + "'. The parameter cpuSrcBox must not be null" );
            }

            Nui8 * sysptr = dst->getData( dstmipmap );
            if(sysptr)
            {
                PixelBlock sysbox;

                if(dstBox)
                {
                    sysbox = *dstBox;
                    sysbox.data = sysptr;
                    sysbox.mUnitSize = PixelUtil::getUnitSize( dst->getFormat() );
                    sysbox.mRowSize = dst->getRowSize( dstmipmap );
                    sysbox.mSliceSize = dst->getSliceSize( dstmipmap ); 
                }
                else
                {
                    sysbox = dst->getDataBlock( dstmipmap );
                }

                sysbox.write(srcBox);
            }
        }

        mLastFrame = mMag->getFrameCount();
    }
    //-----------------------------------------------------------------------------------
    void MappedTexture::write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host )
    {
        
    }
    //-----------------------------------------------------------------------------------
    bool MappedTexture::request( const PixelVolume & vol ) const
    {
        return PixelUtil::getSize(vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat) <= mSize;
    }
    //-----------------------------------------------------------------------------------
}