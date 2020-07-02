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
    class _EngineAPI MemIndexBuffer : public IndexBuffer
    {
    public:
        MemIndexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);

        ~MemIndexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };

    /// SYS / VertexBuffer
    class _EngineAPI MemVertexBuffer : public VertexBuffer
    {
    public:
        MemVertexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);

        ~MemVertexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size);

        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };
    
    /// CPU MemCounterBuffer
    class _EngineAPI MemCounterBuffer : public CounterBuffer
    {
    public:
        MemCounterBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~MemCounterBuffer();
        
        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);
        
        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);
                       
        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size);
        
        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);
        
        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };
    
    /// CPU UniformBuffer
    class _EngineAPI MemUniformBuffer : public UniformBuffer
    {
    public:
        MemUniformBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~MemUniformBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size);

        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };
    
    /** 系统级内存管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemGBufferManager : public GBufferManager
    {
    public:
        MemGBufferManager();
        ~MemGBufferManager();

        /// @copydetails GBufferManager::create
        void create(VertexBuffer *& out, NCount size, NCount count, Nmark mode);

        /// @copydetails GBufferManager::create
        void create(IndexBuffer *& out, NCount size, NCount count, Nmark mode);

        /// @copydetails GBufferManager::create
        void create(CounterBuffer *& out, NCount size, Nmark mode);
        
        /// @copydetails GBufferManager::create
        void create(UniformBuffer *& out, NCount size, Nmark mode);

        /// @copydetails GBufferManager::create
        void create(DisplayBuffer *& out);
    };
}
#endif