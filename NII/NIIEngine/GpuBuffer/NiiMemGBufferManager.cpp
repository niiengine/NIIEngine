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
#include "NiiMemGBufferManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemIndexBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemIndexBuffer::MemIndexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData) :
        IndexBuffer(mag, unitsize, unitcount, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = N_alloc_t(Nui8, mSize);
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemIndexBuffer::~MemIndexBuffer()
    {
        N_free(mData);
    }
    //-----------------------------------------------------------------------
    void * MemIndexBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemIndexBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    void * MemIndexBuffer::lockImpl(Nmark m, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    bool MemIndexBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void MemIndexBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemIndexBuffer::read(void * dst, NCount oft, NCount size)
    {
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(dst, mData + oft, size);
    }
    //-----------------------------------------------------------------------
    void MemIndexBuffer::write(const void * src, NCount oft, NCount size)
    {
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(mData + oft, src, size);
    }
    //-----------------------------------------------------------------------
    void MemIndexBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemIndexBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemIndexBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemVertexBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemVertexBuffer::MemVertexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData) :
        VertexBuffer(mag, unitsize, unitcount, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = (Nui8 *)(N_alloc_simd(mSize));
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemVertexBuffer::~MemVertexBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void * MemVertexBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemVertexBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    void * MemVertexBuffer::lockImpl(Nmark m, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    bool MemVertexBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void MemVertexBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemVertexBuffer::read(void * dst, NCount oft, NCount size)
    {
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(dst, mData + oft, size);
    }
    //-----------------------------------------------------------------------
    void MemVertexBuffer::write(const void * src, NCount oft, NCount size)
    {
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(mData + oft, src, size);
    }
    //-----------------------------------------------------------------------
    void MemVertexBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemVertexBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemVertexBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemCounterBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemCounterBuffer::MemCounterBuffer(BufferManager * mag, NCount size, BufferModeMark mode, Nui8 * initData): 
        CounterBuffer(mag, size, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = N_alloc_t_simd(Nui8, mSize);
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemCounterBuffer::~MemCounterBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void * MemCounterBuffer::lockImpl(Nmark lm, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemCounterBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    bool MemCounterBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void* MemCounterBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemCounterBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemCounterBuffer::read(void * dst, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(dst, mData + oft, length);
    }
    //-----------------------------------------------------------------------
    void MemCounterBuffer::write(const void * src, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(mData + oft, src, length);
    }
    //-----------------------------------------------------------------------
    void MemCounterBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemCounterBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemCounterBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemStructBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemStructBuffer::MemStructBuffer(BufferManager * mag, NCount size, BufferModeMark mode, Nui8 * initData) : 
        StructBuffer(mag, size, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = (Nui8 *)(N_alloc_simd(size));
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemStructBuffer::~MemStructBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void* MemStructBuffer::lockImpl(Nmark lm, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemStructBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    bool MemStructBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void* MemStructBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemStructBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemStructBuffer::read(void * dest, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(dest, mData + oft, length);
    }
    //-----------------------------------------------------------------------
    void MemStructBuffer::write(const void * src, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(mData + oft, src, length);
    }
    //-----------------------------------------------------------------------
    void MemStructBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemStructBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemStructBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemIndirectBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemIndirectBuffer::MemIndirectBuffer(BufferManager * mag, Type type, NCount size, BufferModeMark mode, Nui8 * initData) : 
        IndirectBuffer(mag, type, size, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = (Nui8 *)(N_alloc_simd(size));
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemIndirectBuffer::~MemIndirectBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void* MemIndirectBuffer::lockImpl(Nmark lm, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemIndirectBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    bool MemIndirectBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void* MemIndirectBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemIndirectBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemIndirectBuffer::read(void * dest, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(dest, mData + oft, length);
    }
    //-----------------------------------------------------------------------
    void MemIndirectBuffer::write(const void * src, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(mData + oft, src, length);
    }
    //-----------------------------------------------------------------------
    void MemIndirectBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemIndirectBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemIndirectBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemStorageBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemStorageBuffer::MemStorageBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData) : 
        IndirectBuffer(mag, unitsize, unitcount, mode & ~Buffer::M_HOST)
    {
        if(initData)
            mData = initData;
        else
            mData = (Nui8 *)(N_alloc_simd(size));
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemStorageBuffer::~MemStorageBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void* MemStorageBuffer::lockImpl(Nmark lm, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemStorageBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    bool MemStorageBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void* MemStorageBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemStorageBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemStorageBuffer::read(void * dest, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(dest, mData + oft, length);
    }
    //-----------------------------------------------------------------------
    void MemStorageBuffer::write(const void * src, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(mData + oft, src, length);
    }
    //-----------------------------------------------------------------------
    void MemStorageBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemStorageBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemStorageBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemTextureBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemTextureBuffer::MemTextureBuffer(BufferManager * mag, NCount size, BufferModeMark mode, PixelFormat pf, Nui8 * initData) : 
        TextureBuffer(mag, size, mode & ~Buffer::M_HOST, pf)
    {
        if(initData)
            mData = initData;
        else
            mData = (Nui8 *)(N_alloc_simd(size));
        mMark |= Buffer::M_HOST;
    }
    //-----------------------------------------------------------------------
    MemTextureBuffer::~MemTextureBuffer()
    {
        N_free_simd(mData);
    }
    //-----------------------------------------------------------------------
    void* MemTextureBuffer::lockImpl(Nmark lm, NCount oft, NCount size)
    {
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemTextureBuffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //-----------------------------------------------------------------------
    bool MemTextureBuffer::reserveImpl(NCount size, Nmark newMode, bool oldData)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void* MemTextureBuffer::lock(Nmark m, NCount oft, NCount size, NCount multiIdx)
    {
        mMapping = true;
        return mData + oft;
    }
    //-----------------------------------------------------------------------
    void MemTextureBuffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        mMapping = false;
    }
    //-----------------------------------------------------------------------
    void MemTextureBuffer::read(void * dest, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(dest, mData + oft, length);
    }
    //-----------------------------------------------------------------------
    void MemTextureBuffer::write(const void * src, NCount oft, NCount length)
    {
        assert((oft + length) <= mSize);
        ::memcpy(mData + oft, src, length);
    }
    //-----------------------------------------------------------------------
    void MemTextureBuffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        N_assert(dynamic_cast<MemTextureBuffer *>(src), "");
        N_assert((oft + size) <= mSize, "error");
        ::memcpy(static_cast<MemTextureBuffer *>(src)->mData + oft, src + srcOft, size);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MemGBufferManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MemGBufferManager::MemGBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    MemGBufferManager::~MemGBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::setupVAO(GeometryRaw * obj)
    {
        N_EXCEPT(RenderingAPI, _I18n("Cannot create DisplayBuffer in MemGBufferManager"));
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(GpuBufferAsync *& out, GpuBuffer * creator, MappedBuffer * buf, NCount oft, NCount size )
    {
        N_EXCEPT(RenderingAPI, _I18n("Cannot create DisplayBuffer in MemGBufferManager"));
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(VertexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemVertexBuffer(this, usize, ucount, mode, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(IndexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemIndexBuffer(this, usize, ucount, mode, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(CounterBuffer *& out, NCount size, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemCounterBuffer(this, size, mode, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(IndirectBuffer *& out, IndirectBuffer::Type type, NCount size, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemIndirectBuffer(this, type, size, mode);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(StructBuffer *& out, NCount size, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemStorageBuffer(this, size, mode, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(TextureBuffer *& out, PixelFormat pf, NCount size, BufferModeMark mode, void * initData, GpuGroupID)
    {
        out = N_new MemTextureBuffer(this, size, mode, pf, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(StorageBuffer *& out, NCount usize, NCount ucount, void * initData, GpuGroupID)
    {
        out = N_new MemStructBuffer(this, usize, ucount, initData);
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::create(DisplayBuffer *& out)
    {
        N_EXCEPT(RenderingAPI, _I18n("Cannot create DisplayBuffer in MemGBufferManager"));
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::createImpl(VertexData *& out
    {
        N_EXCEPT(RenderingAPI, _I18n("Cannot create DisplayBuffer in MemGBufferManager"));
    }
    //-----------------------------------------------------------------------
    void MemGBufferManager::destroyImpl(VertexData * obj)
    {
        N_EXCEPT(RenderingAPI, _I18n("Cannot create DisplayBuffer in MemGBufferManager"));
    }
    //-----------------------------------------------------------------------
}