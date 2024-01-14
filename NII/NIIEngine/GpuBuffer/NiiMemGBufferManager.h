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

#ifndef _NII_MEM_GBUFFERMANAGER_H_
#define _NII_MEM_GBUFFERMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"

namespace NII
{
    /// SYS / IndexBuffer
    class MemIndexBuffer : public IndexBuffer
    {
    public:
        MemIndexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData);

        ~MemIndexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark lm, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };

    /// SYS / VertexBuffer
    class MemVertexBuffer : public VertexBuffer
    {
    public:
        MemVertexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData);

        ~MemVertexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /// CPU MemCounterBuffer
    class MemCounterBuffer : public CounterBuffer
    {
    public:
        MemCounterBuffer(BufferManager * mag, NCount size, BufferModeMark mode, Nui8 * initData);
        ~MemCounterBuffer();
        
        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);
                       
        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);
        
        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);
        
        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /// CPU StructBuffer
    class MemStructBuffer : public StructBuffer
    {
    public:
        MemStructBuffer(BufferManager * mag, NCount size, BufferModeMark mode, Nui8 * initData);
        ~MemStructBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /// CPU IndirectBuffer
    class MemIndirectBuffer : public IndirectBuffer
    {
    public:
        MemIndirectBuffer(BufferManager * mag, Type type, NCount size, BufferModeMark mode, Nui8 * initData);
        ~MemIndirectBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /// CPU StorageBuffer
    class MemStorageBuffer : public StorageBuffer
    {
    public:
        MemStorageBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, BufferModeMark mode, Nui8 * initData);
        ~MemStorageBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /// CPU TextureBuffer
    class MemTextureBuffer : public TextureBuffer
    {
    public:
        MemTextureBuffer(BufferManager * mag, NCount size, BufferModeMark mode, PixelFormat pf, Nui8 * initData);
        ~MemTextureBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size, NCount multiIdx = 0);

        /// @copydetails Buffer::unlock
        void unlock(UnlockType type, NCount oft, NCount size);
        
        Nui8 * getHostData() const  {return mData;}
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl(UnlockType type, NCount oft, NCount size);

        /// @copydetails Buffer::reserveImpl
        bool reserveImpl(NCount size, Nmark newMode = -1, bool oldData = true);
    protected:
        Nui8 * mData;
    };
    
    /** 系统级内存管理器
    @version NIIEngine 3.0.0
    */
    class MemGBufferManager : public GBufferManager
    {
    public:
        MemGBufferManager();
        ~MemGBufferManager();
        
        void setupVAO(GeometryRaw * obj);
        
        /// @copydetails GBufferManager::create
        void create(GpuBufferAsync *& out, GpuBuffer * creator, MappedBuffer * buf, NCount oft, NCount size );

        /// @copydetails GBufferManager::create
        void create(VertexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);

        /// @copydetails GBufferManager::create
        void create(IndexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);

        /// @copydetails GBufferManager::create
        void create(CounterBuffer *& out, NCount size, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::create
        void create(IndirectBuffer *& out, IndirectBuffer::Type type, NCount size, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::create
        void create(StructBuffer *& out, NCount size, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::create
        void create(TextureBuffer *& out, PixelFormat pf, NCount size, BufferModeMark mode, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::create
        void create(StorageBuffer *& out, NCount usize, NCount ucount, void * initData = 0, GpuGroupID pool = 0);

        /// @copydetails GBufferManager::create
        void create(DisplayBuffer *& out);
        
        /// @copydetails GBufferManager::createImpl
        void createImpl(VertexData *& out);
        
        /// @copydetails GBufferManager::destroyImpl
        void destroyImpl(VertexData * obj);
    };
}
#endif