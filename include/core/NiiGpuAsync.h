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

#ifndef _NII_AyncTicket_H_
#define _NII_AyncTicket_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"
#include "NiiTexture.h"
#include "NiiPixelFormat.h"

namespace NII
{
    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI GpuBufferAsync : public BufferAlloc
    {
    public:
        GpuBufferAsync(GpuBuffer * src, NCount oft, NCount count);
        GpuBufferAsync(GpuBuffer * src, MappedBuffer * mapbuf, NCount oft, NCount count);
        virtual ~GpuBufferAsync();

        /** 
        @version NIIEngine 5.0.0
        */
        const void * map();

        /**
        @version NIIEngine 5.0.0
        */
        void unmap(void);

        /**
        @version NIIEngine 5.0.0
        */
        virtual OperationReason isDataValid()   { return OR_OK; }
    protected:
        virtual const void * mapImpl(void) = 0;
    private:
        GpuBuffer * mSrc;
        MappedBuffer * mMapBuffer;

        NCount mMapOffset;
        NCount mOffset;
        NCount mSize;
        bool mMapping;
    };
    
    /** 
    @version NIIEngine 5.0.0
    */
    class _EngineAPI TextureAsync : public PassAlloc, public Texture::Process
    {
    public:
        enum BlockType
        {
            BT_None,
            BT_Mapping,
            BT_Reading,
            BT_Writting
        };
        struct Operation
        {
            Operation() : mOp(0), mMipMap(0), mOpSrc(0), mOffset(0), mNotify(false){}

            Operation(Texture * op, NCount opmipmap, PixelBlock * opBox, NCount oft = 0, bool notify = false) :
                mOp(op),
                mMipMap(opmipmap),
                mOpSrc(opBox ? *opBox : PixelBlock()),
                mOffset(oft),
                mNotify(notify)
            {
            }
            
            Texture * mOp;
            PixelBlock mOpSrc;
            BlockType mType;
            NCount mMipMap;
            NCount mOffset;
            bool mNotify;
        };
        typedef vector<Operation>::type OperationList;
    public:
        TextureAsync(NCount width, NCount height, NCount depth, NCount array, Texture::Type type, PixelFormat pf);
        virtual ~TextureAsync();

        /** 
        @version NIIEngine 5.0.0
        */
        void write(Texture * src, NCount srcmipmap, PixelBlock * srcBox, NCount oft = 0, bool notify = false);

        /**
        @version NIIEngine 5.0.0
        */
        void read(Texture * dst, NCount dstmipmap, PixelBlock * dstBox, NCount oft = 0, bool notify= false);

        /** 
        @version NIIEngine 5.0.0
        */
        PixelBlock map(NCount depthIdx, NCount arrayIdx = 0, NCount oft = 0);

        /**
        @version NIIEngine 5.0.0
        */
        void unmap();
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getWidth() const                         { return mWidth; }
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getHeight() const                        { return mHeight; }
    
        /**
        @version NIIEngine 5.0.0
        */
        NCount getDepth() const                         { return mDepth; }
        
        /**
        @version NIIEngine 5.0.0
        */
        virtual NCount getArray() const                 { return mArray;}
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getVolume() const                        { return mDepth * mArray; }
        
        /**
        @version NIIEngine 5.0.0
        */
        PixelFormat getFormat() const                   { return mFormat;}

        /**
        @version NIIEngine 5.0.0
        */
        NCount getRowSize() const                       { return PixelUtil::getSize( mWidth, 1u, 1u, 1u, mFormat);}
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getSliceSize() const                     { return PixelUtil::getSize( mWidth, mHeight, 1u, 1u, mFormat);}
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getArraySize() const                     { return PixelUtil::getSize( mWidth, mHeight, mDepth, 1u, mFormat);}
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getSize() const                          { return PixelUtil::getSize( mWidth, mHeight, mDepth, mArray, mFormat);}

        /**
        @version NIIEngine 5.0.0
        */
        virtual bool isDataValid(NCount oft = 0);
        
        /**
        @version NIIEngine 5.0.0
        */
        virtual void process(const Item & item );
    protected:
        /**
        @version NIIEngine 5.0.0
        */
        virtual PixelBlock mapImpl(NCount depthIdx, NCount arrayIdx = 0, NCount oft = 0) = 0;

        /**
        @version NIIEngine 5.0.0
        */
        virtual void unmapImpl() = 0;

        /**
        @version NIIEngine 5.0.0
        */
        virtual void writeImpl(Texture * src, NCount srcmipmap, PixelBlock * srcBox, NCount oft = 0, bool notify = false);
        
        /**
        @version NIIEngine 5.0.0
        */
        virtual void readImpl(Texture * dst, NCount dstmipmap, PixelBlock * dstBox, NCount oft = 0, bool notify = false);
    protected:
        OperationList mOpList;
        BlockType mBlockType;
        NCount mWidth;
        NCount mHeight;
        NCount mDepth;
        NCount mArray;
        Texture::Type mTextureType;
        PixelFormat mFormat;
        NCount mQueryCount;
    };
}

#endif
