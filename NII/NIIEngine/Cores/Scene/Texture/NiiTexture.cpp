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
#include "NiiTexture.h"
#include "NiiLogManager.h"
#include "NiiGpuBuffer.h"
#include "NiiImageCodec.h"
#include "NiiException.h"
#include "NiiTextureManager.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"
#include "NiiBitwise.h"
#include "NiiStrConv.h"

namespace NII
{
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // SampleType
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void SampleType::_set( Nui8 msaa, Nui8 csaa, CoordType type )
    {
        mMultiSampling = msaa;
        mCoverageSampling = csaa;
        mType = type;
    }
    //-----------------------------------------------------------------------------------
    void SampleType::setMsaa( Nui8 msaa, CoordType pattern )
    {
        mMultiSampling = msaa;
        mCoverageSampling = 0u;
        mType = pattern;
    }
    //-----------------------------------------------------------------------------------
    bool SampleType::isMsaa() const
    {
        return mCoverageSampling == 0u;
    }
    //-----------------------------------------------------------------------------------
    void SampleType::setCsaa( Nui8 csaa, bool quality )
    {
        if( csaa != 8u && csaa != 16u )
        {
            N_EXCEPT( InvalidParam, "CSAA setting must be 8x or 16x");
        }

        mMultiSampling = quality ? 8u : 4u;
        mCoverageSampling = csaa;
        mType = CT_Unknow;
    }
    //-----------------------------------------------------------------------------------
    void SampleType::setEqaa( Nui8 msaa, Nui8 csaa )
    {
        mMultiSampling = msaa;
        mCoverageSampling = csaa;
        mType = CT_Unknow;
    }
    //-----------------------------------------------------------------------------------
    void SampleType::parseString( const String & hint )
    {
        Nui8 samples; 
        StrConv::conv(hint, samples);
        samples = std::max(1u, samples);
        const bool csaa = hint.find( "CSAA" ) != String::npos;
        const bool eqaa = hint.find( "EQAA" ) != String::npos;

        if( csaa )
        {
            // "8x CSAA", "8xQ CSAA", "16x CSAA", "16xQ CSAA", also "8 [Quality]", "16 [Quality]"
            bool qualityHint = samples >= 8u && ( hint.find( "Quality" ) != String::npos ||
                                                  hint.find( "xQ CSAA" ) != String::npos );
            setCsaa( samples, qualityHint );
        }
        else if( eqaa )
        {
            // "2f4x EQAA", "4f8x EQAA", "4f16x EQAA", "8f16x EQAA", but not Dx9 only "2f8x EQAA"
            if( hint.find("4f16x") != String::npos )
                setEqaa( 4, 16 );
            else
                setEqaa( samples, static_cast<Nui8>( samples << 1u ) );
        }
        else
            setMsaa( samples ); // "Nx MSAA", also "Nx", "N"
    }
    //-----------------------------------------------------------------------------------
    /*bool SampleType::isCsaa( void ) const
    {
        // {4,8} {8,8} {4,16} {8,16} 
        return ( mCoverageSampling == 8u || mCoverageSampling == 16u ) && ( mMultiSampling == 4u || mMultiSampling == 8u );
    }
    //-----------------------------------------------------------------------------------
    bool SampleType::isCsaaQuality( void ) const
    {
        // {8,8} {8,16}, but not the {4,8} {4,16} 
        return ( mCoverageSampling == 8u || mCoverageSampling == 16u ) && mMultiSampling == 8u;
    }*/
    //-----------------------------------------------------------------------------------
    void SampleType::getString( String & out ) const
    {
        if( mCoverageSampling == 0 )
            out += N_conv(mMultiSampling) + _T("x MSAA");
        else if( ( mCoverageSampling == 8u || mCoverageSampling == 16u ) && mMultiSampling == 8u )
            out += N_conv(mCoverageSampling) + _T("xQ CSAA");
        else if( ( mCoverageSampling == 8u || mCoverageSampling == 16u ) && ( mMultiSampling == 4u || mMultiSampling == 8u ) )
            out += N_conv(mCoverageSampling) + _T("x CSAA");
        else
            out += N_conv(mMultiSampling) + _T("f") + N_conv(mCoverageSampling) + _T("x EQAA");
    
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // Process
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Texture::Process::~Process() {}
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // Texture
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const IdString Texture::msFinalTextureBuffer = IdString( "msFinalTextureBuffer" );
    const IdString Texture::msMsaaTextureBuffer  = IdString( "msMsaaTextureBuffer" );
    PixelFormat Texture::DepthFormat = PF_Depth24UN_Stencil8ui;
    //--------------------------------------------------------------------------
    Texture::Texture(ResourceID rid, GroupID gid, LangID lid) :
        Resource(N_CmdObj_Texture, rid, gid, 0, 0, lid),
        mHeight(512),
        mSrcHeight(0),
        mWidth(512),
        mSrcWidth(0),
        mDepth(1),
        mSrcDepth(0),
        mArray(0),
        mFormat(PF_UNKNOWN),
        mSrcFormat(PF_UNKNOWN),
        mMipmapCount(0),
        mSrcMipmapCount(0),
        mGamma(1.0f),
        mFSAA(0),
        mTextureType(Texture::T_2D),
        mInnerValid(false),
        mAutoData(true)
    {
        if(setupCmd(N_CmdObj_Texture))
        {
            //定义参数描述通用的资源加载,实际上这里没有,因为预声明了,你使用的纹理文件包含了所有需要的信息
        }
        mMark = MM_MIPMAP_AUTO | Buffer::M_NORMAL  | MM_COLOUR_RGB | MM_COLOUR_ALPHA;

        if(TextureManager::getOnlyPtr())
        {
            const TextureManager & tmgr = N_Only(Texture);
            setMipmapCount(tmgr.getMipMapCount());

            if(tmgr.getBitDepth() == 16)
            {
                addModMark(MM_DEPTH_BIT_16);
                removeModMark(MM_DEPTH_BIT_32);
            }
            else
            {
                addModMark(MM_DEPTH_BIT_32);
                removeModMark(MM_DEPTH_BIT_16);
            }
        }
        mCreator = N_OnlyPtr(Texture);
    }
    //--------------------------------------------------------------------------
    Texture::Texture(ResourceID rid, const StringList & filename, GroupID gid, LangID lid) :
        Resource(N_CmdObj_Texture, rid, gid, 0, 0, lid),
        mHeight(512),
        mSrcHeight(0),
        mWidth(512),
        mSrcWidth(0),
        mDepth(1),
        mSrcDepth(0),
        mArray(0),
        mFormat(PF_UNKNOWN),
        mSrcFormat(PF_UNKNOWN),
        mMipmapCount(0),
        mSrcMipmapCount(0),
        mGamma(1.0f),
        mFSAA(0),
        mTextureType(Texture::T_2D),
        mInnerValid(false),
        mAutoData(true)
    {
        if (filename.size() == 1)
        {
            mSrc = filename[0];
        }
        else
        {
            mOriginList = filename;
        }
        if(setupCmd(N_CmdObj_Texture))
        {
            //定义参数描述通用的资源加载,实际上这里没有,因为预声明了,你使用的纹理文件包含了所有需要的信息
        }

        mMark = MM_MIPMAP_AUTO | Buffer::M_NORMAL  | MM_COLOUR_RGB | MM_COLOUR_ALPHA;

        if(TextureManager::getOnlyPtr())
        {
            const TextureManager & tmgr = N_Only(Texture);
            setMipmapCount(tmgr.getMipMapCount());

            if(tmgr.getBitDepth() == 16)
            {
                addModMark(MM_DEPTH_BIT_16);
                removeModMark(MM_DEPTH_BIT_32);
            }
            else
            {
                addModMark(MM_DEPTH_BIT_32);
                removeModMark(MM_DEPTH_BIT_16);
            }
        }
        mCreator = N_OnlyPtr(Texture);
        if(filename.size() == 1)
            load(mSrc, gid);
        else
            load(N_StrBlank, gid);
    }
    //--------------------------------------------------------------------------
    Texture::Texture(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, LangID lid):
        Resource(N_CmdObj_Texture, rid, gid, ls, rs, lid),
        mHeight(512),
        mSrcHeight(0),
        mWidth(512),
        mSrcWidth(0),
        mDepth(1),
        mSrcDepth(0),
        mArray(0),
        mFormat(PF_UNKNOWN),
        mSrcFormat(PF_UNKNOWN),
        mMipmapCount(0),
        mSrcMipmapCount(0),
        mGamma(1.0f),
        mFSAA(0),
        mTextureType(Texture::T_2D),
        mInnerValid(false),
        mAutoData(true)
    {
        if(setupCmd(N_CmdObj_Texture))
        {
            //定义参数描述通用的资源加载,实际上这里没有,因为预声明了,你使用的纹理文件包含了所有需要的信息
        }

        mMark = MM_MIPMAP_AUTO | Buffer::M_NORMAL  | MM_COLOUR_RGB | MM_COLOUR_ALPHA;

        if(TextureManager::getOnlyPtr())
        {
            const TextureManager & tmgr = N_Only(Texture);
            setMipmapCount(tmgr.getMipMapCount());

            if(tmgr.getBitDepth() == 16)
            {
                addModMark(MM_DEPTH_BIT_16);
                removeModMark(MM_DEPTH_BIT_32);
            }
            else
            {
                addModMark(MM_DEPTH_BIT_32);
                removeModMark(MM_DEPTH_BIT_16);
            }
        }
        mCreator = N_OnlyPtr(Texture);
    }
    //-----------------------------------------------------------------------------------
    Texture::Texture(ResourceID name, Type type, Nmark usage, RecoverType rtype) :
        mCurrentStorage(StorageType::ST_Disk),
        mNextStorage(StorageType::ST_Disk),
        mRecoverType(rtype),
        mTaskCount(0),
        mLastFrameUsed(N_Only(GBuffer).getFrameCount()),
        mName(name),
        mPoolArray(0),
        mTextureType(type),
        mMark(usage),
        mPoolId(0),
        mData(0),
        mPool(0),
        mAutoData(true)
    {
        N_assert( !isPoolSlice() || (isPoolSlice() && isTexture() && !isFrame() && !isBufferStroge()) );
        N_assert( (!isAutoGenMipmap() || isAutoMipmap()) && "MM_MIPMAP_CHECK requires MM_MIPMAP_AUTO" );
    }
    //-----------------------------------------------------------------------------------
    Texture::~Texture()
    {
        ImageList::iterator i, iend = mLoadedImages.end();
        for (i = mLoadedImages.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mLoadedImages.clear();

        FrameBufferList::iterator j, jend = mSurfaceList.end();
        for (j = mSurfaceList.begin(); j != jend; ++j)
        {
            N_delete *j;
        }
        mSurfaceList.clear();
        
        N_assert( mProcessList.empty() &&
                "There are listeners out there for this Texture! "
                "process( Process::O_Delete ) wasn't called!"
                "This could leave dangling pointers. Ensure you've cleaned up correctly. "
                "Most likely there are Materials/Datablocks still using this texture" );

        if( mData )
        {
            N_free_simd( mData, MEMCATEGORY_RESOURCE );
            mData = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void Texture::schedule(const Task & task)
    {
        if(mNextStorage == task.mType && task.mCheck)
            return;
        ++mTaskCount;

        if(isManual())
        {
            mNextStorage = task.mType;
            N_assert(!task.mImage, "Image pointer must null for manual textures!");
            load(task.mType, (Nui8 *)0);
        }
        else if(task.mType == ST_Reload)
        {
            load(StorageType::ST_Disk, 0);
            N_Only(Texture)._removeMetadata(mTexture);
            Task temp = task;
            temp.mType = StorageType::ST_Gpu;
            schedule(temp);
        }
        else
        {
            mNextStorage = task.mType;
            N_Only(Texture).schedule(task);
        }
    }
    //-----------------------------------------------------------------------------------
    void Texture::setType(Texture::Type type)
    {
        N_assert( mCurrentStorage == StorageType::ST_Disk );
        mTextureType = type;

        if( mTextureType == Texture::T_CUBE )
        {
            mArray = 6u;
            mDepth = 1u;
        }
        else if(mTextureType == Texture::T_3D)
        {
            mArray = 1u;
        }
        else
            mDepth = 1u;
    }
    //-----------------------------------------------------------------------------------
    void Texture::setFormat(PixelFormat pf)
    {
        N_assert( mCurrentStorage == StorageType::ST_Disk );

        if( isPreferSRGB() )
            pf = PixelUtil::getSRGB( pf );

        mFormat = pf;
    }
    //-----------------------------------------------------------------------------------
    void Texture::setSample( SampleType type)
    {
        N_assert(mCurrentStorage == StorageType::ST_Disk);
        N_assert(type.getMultiSampling() > 0u);
        mSample = type;
    }
    //-----------------------------------------------------------------------------------
    void Texture::setOriginSample(SampleType type)
    {
        N_assert(mCurrentStorage == StorageType::ST_Disk);
        N_assert(type.getMultiSampling() > 0u);
        mSrcSample = type;
    }
    //-----------------------------------------------------------------------------------
    void Texture::copyModel(const Texture & o)
    {
        N_assert(mCurrentStorage == StorageType::ST_Disk);

        mWidth = o.mWidth;
        mHeight = o.mHeight;
        mDepthOrSlices = o.mDepthOrSlices;
        mMipmapCount = o.mMipmapCount;
        mFormat = o.mFormat;
        mSrcSample = src.mSrcSample;
        mSample= src.mSample;
        mTextureType = src.mTextureType;
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isModelEqual(const Texture & o) const
    {
        return mWidth == o.mWidth &&
               mHeight == o.mHeight &&
               mDepthOrSlices == o.mDepthOrSlices &&
               mMipmapCount == o.mMipmapCount &&
               mFormat == o.mFormat &&
               mSrcSample == o.mSrcSample &&
               mTextureType == o.mTextureType;
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isSupport(CoordType pattern)
    {
        return pattern == CT_Unknow;
    }
    //-----------------------------------------------------------------------------------
    void Texture::checkValidSettings()
    {
        //Make sure depth buffers/textures always have MM_ManualMSAA set (with or without MSAA).
        if(PixelUtil::isDepth(mFormat))
            mMark |= Texture::MM_ManualMSAA;
        if(mFormat == PFG_NULL && isTexture())
            mMark |= Texture::MM_FRAME;

        if( mSrcSample.isMultiSample() )
        {
            if( (mMipmapCount > 0) || (!isFrame() && !isBufferStroge()) )
            {
                N_EXCEPT( InvalidParam, "Texture '" + getName() + "': "
                    "MSAA Textures cannot have mipmaps (use explict resolves for that), and must be either MM_FRAME or MM_BufferStroge");
            }

            if( mTextureType == Texture::T_2DArray && !isManualMSAA() )
            {
                N_EXCEPT( InvalidParam, "Texture '" + getName() + "': Only explicit resolves support Type2DArray" );
            }

            if( mTextureType != Texture::T_2D && mTextureType != Texture::T_2DArray )
            {
                N_EXCEPT( InvalidParam, "Texture '" + getName() + "': MSAA can only be used with Type2D or Type2DArray" );
            }

            if( isPoolSlice() )
            {
                N_EXCEPT( InvalidParam, "Texture '" + getName() + "': MSAA textures cannot use MM_POOLSLICE" );
            }
        }

        if( mTextureType == Texture::T_Unknow )
        {
            N_EXCEPT( InvalidParam, "Texture '" + getName() + "': TextureType cannot be Texture::T_Unknow" );
        }

        if( mRecoverType == Texture::RT_CpuGpu && (isFrame() || isBufferStroge()) )
        {
            N_EXCEPT( InvalidParam, "Cannot use CPUGPU with MM_FRAME or MM_BufferStroge" );
        }

        if( isPoolSlice() && (mTextureType != Texture::T_2D || isFrame() || isBufferStroge()) )
        {
            N_EXCEPT( InvalidParam, "Texture '" + getName() + "': "
                "MM_POOLSLICE can only be used with Type2D textures, and they cannot be MM_FRAME or MM_BufferStroge");
        }

        if( isAutoMipmap() && !isFrame() )
        {
            N_EXCEPT( InvalidParam, "Texture '" + getName() + "': MM_MIPMAP_AUTO requires MM_FRAME." );
        }

        if( mWidth < 1u || mHeight < 1u || mDepthOrSlices < 1u || mFormat == PF_UNKNOWN )
        {
            N_EXCEPT( InvalidParam, "Texture '" + getName() + "': Invalid settings!" );
        }

        mMipmapCount = std::min( mMipmapCount, PixelUtil::getMaxMipmap( mWidth, mHeight, getDepth() ) );
    }
    //-----------------------------------------------------------------------------------
    void Texture::load(StorageType type, Nui8 * data, bool immediate)
    {
        N_assert(type != mCurrentStorage);

        bool allowResidencyChange = true;
        Process::Operation op = Process::Unknown;

        if(type == StorageType::ST_Gpu)
        {
            if(mRecoverType == Texture::RT_CpuGpu)
            {
                N_assert( data && "Must provide a SysRAM copy if using CPUGPU" );
                mData = data;
            }
            else
            {
                if( mData )
                {
                    if( mAutoData )
                        N_free_simd(mData);
                    mData = 0;
                }
            }

            checkValidSettings();

            if( mSample.isMultiSample() )
            {
                // RenderWindows already validated their SampleDescriptions and we cannot override
                // Thus we rely on Windows already having properly called setSample
                // and just call the listeners
                if( !isWindow() )
                {
                    mSrcSample = N_Engine().getRender()->checkSample(mSample, mFormat);
                }
                if(mSrcSample != mSample)
                    process( Process::O_FsaaAlter, immediate);
            }

            if( isPoolSlice() )
            {
                N_Only(Texture).poolAlloc( this );
            }
            else
            {
                createInternalImpl();
            }

            //Delegate to the manager (thread) for loading.
        
            op = Process::O_InGpu;
        }
        else if( type == StorageType::ST_Cpu )
        {
            if( mCurrentStorage == StorageType::ST_Disk )
            {
                N_assert( !mData && "It should be impossible to have SysRAM copy in this stage!" );
                checkValidSettings();

                N_assert( data && "Must provide a SysRAM copy when transitioning from ST_Disk to ST_Cpu!" );
                mData = data;

                op = Process::O_DiskToCpu;
            }
            else
            {
                N_assert(((mData && mRecoverType == Texture::RT_CpuGpu) ||
                    (!mData && mRecoverType != Texture::RT_CpuGpu)) &&
                    "We should already have a SysRAM copy if we were CPUGPU; or we shouldn't have a"
                    "SysRAM copy if we weren't in that strategy." );

                N_assert( (mRecoverType == Texture::RT_CpuGpu || !data || mData == data) &&
                    "data must be nullptr or equal to mData when mRecoverType != Texture::RT_CpuGpu" );

                if( mData )
                {
                    freeInternalImpl(); // 2
                    op = Process::O_OutGpu;
                }
                else
                {
                    N_Only(Texture).syncToCpu( this, true );
                    allowResidencyChange = false;
                }
            }
        }
        else // ST_Disk
        {
            if( mData )
            {
                if( mAutoData )
                    N_free_simd(mData);
                mData = 0;
                
                if( mCurrentStorage == StorageType::ST_Cpu )
                    op = Process::O_OutCpu;
            }

            freeInternalImpl(); // 2

            if( mCurrentStorage == StorageType::ST_Gpu )
                op = Process::O_OutGpu;
            //else
            //    op = Process::O_CpuToDisk;
        }

        if(allowResidencyChange)
        {
            mCurrentStorage = type;

            mTaskCount = std::max( mTaskCount, 1u ) - 1u;
            process(op, immediate);
        }

        if(isManual())
        {
            mNextStorage = mCurrentStorage;
            if( mCurrentStorage == StorageType::ST_Gpu )
            {
                notifyGpuData(immediate);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void Texture::downloadImpl(void)
    {
        if( !isValid() )
        {
            N_log("WARNING: Texture::downloadImpl will stall. If you see this often,"
                " then probably you're performing too many read calls to an CPUGPU texture" );
        }
        wait(false);
        N_assert( mCurrentStorage == StorageType::ST_Gpu );
        ++mTaskCount;
        N_Only(Texture).syncToCpu( this, false );
    }
    //-----------------------------------------------------------------------------------
    void Texture::notifyCpuData(bool storage)
    {
        N_assert( mCurrentStorage == StorageType::ST_Gpu && "GPU should be resident so the copy could've been made and so we can perform the transition!" );

        Process::Operation op = Process::Unknown;

        if(storage )
        {
            N_assert( mRecoverType != Texture::RT_CpuGpu && "This path should've never been hit as always have the RAM copy!" );

            freeInternalImpl(); // 2

            mCurrentStorage  = StorageType::ST_Cpu;

            op = Process::O_OutGpu;
        }
        else
        {
            N_assert( mRecoverType == Texture::RT_CpuGpu && "This path should only hit if we always have the RAM copy!" );

            op = Process::O_GpuSyncCpu;
        }

        mTaskCount = std::max( mTaskCount, 1u ) - 1u;

        process(op, false);
    }
    //-----------------------------------------------------------------------------------
    void Texture::read( Texture * dst, const PixelBlock & dstBox, NCount dstMipmap,
        const PixelBlock & srcBox, NCount srcMipmap, bool msaa = true )
    {
        N_assert( srcBox.equalSize( dstBox ) );
        N_assert( this != dst || !srcBox.overlaps( dstBox ) );
        N_assert( srcMipLevel < getMipmapCount() && dstMipLevel < dst->getMipmapCount() );
    }
    //-----------------------------------------------------------------------------------
    //virtual void swap() {}
    //-----------------------------------------------------------------------------------
    void Texture::msaa( Texture * resolve )
    {
        if( !isMultiSample() )
        {
            N_EXCEPT( InvalidParam, "Source Texture must be MSAA" );
        }
        if(isOpenGLRenderWindow() )
        {
            N_EXCEPT( InvalidParam, "OpenGL MSAA RenderWindows cannot be resolved" );
        }
        if( PixelUtil::isDepth( this->getFormat() ) )
        {
            N_EXCEPT( InvalidParam, "Depth formats cannot be resolved!" );
        }
        if(getFormat() != resolve->getFormat() )
        {
            N_EXCEPT( InvalidParam, "Source Texture and Resolve texture must have the same pixel formats!" );
        }
        if( !getEmptyBox(0).equalSize( resolve->getEmptyBox(0) ) )
        {
            N_EXCEPT( InvalidParam, "Source Texture and Resolve texture must have the same dimensions!");
        }
        if( resolve->isMultiSample() )
        {
            N_EXCEPT( InvalidParam, "Resolve Texture must not be MSAA");
        }
        if( !resolve->isFrame() )
        {
            N_EXCEPT( InvalidParam, "Resolve Texture must be created with Texture::MM_FRAME" );
        }

        RenderSys * renderSystem = N_Only(Texture).getRender();
        FrameBufferObject * fbo = renderSystem->createFBO();
        fbo->mColour[0].texture = this;
        fbo->mColour[0].mTarget = resolve;

        fbo->mColour[0].mInitType = FrameTarget::IT_Texture;
        //Set to both, because we don't want to lose the contents from this RTT.
        fbo->mColour[0].mStoreType = FrameTarget::ST_MultisampleStore;
        fbo->syncType( FrameBufferObject::TT_All );

        Vector4 fullVp( 0, 0, 1, 1 );
        ViewportList vplist;
        createViewport(vplist, 1, this, &fullVp, &fullVp, 0, false);
        renderSystem->beginFBO(fbo, vplist);
        renderSystem->executeFBO();
        renderSystem->endFBO();
        renderSystem->destroyRenderPassDescriptor(fbo);
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isOpenGLRenderWindow() const
    {
        return false;
    }
    //-----------------------------------------------------------------------------------
    void Texture::poolImpl(const TexturePool * pool, Nui16 array)
    {
        mPool = pool;
        mPoolArray = array;
    }
    //-----------------------------------------------------------------------------------
    void Texture::addProcess(Process * prc)
    {
        mProcessList.push_back(prc);
    }
    //-----------------------------------------------------------------------------------
    void Texture::removeProcess(Process * prc)
    {
        vector<Process*>::type::iterator itor = std::find(mProcessList.begin(), mProcessList.end(), prc);
        N_assert(itor != mProcessList.end());
        remove(mProcessList, itor);
    }
    //-----------------------------------------------------------------------------------
    void Texture::process(uint32 prc, bool immediate)
    {
        //Iterate through a copy in case one of the listeners decides to remove itself.
        vector<Process*>::type::iterator itor = mProcessList.begin();
        vector<Process*>::type::iterator end  = mProcessList.end();

        Item temp;
        temp.mTexture = this;
        temp.mOp = prc;
        temp.mImmediate = immediate;

        while(itor != end)
        {
            (*itor)->process(temp);
            ++itor;
        }

        N_Only(Texture).process(temp);
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isDepthStencilSupport(Texture * colour) const
    {
        if(PixelUtil::isDepth(mFormat) && !PixelUtil::isDepth(colour->mFormat) &&
            mWidth == colour->mWidth && mHeight == colour->mHeight &&
                mSrcSample == colour->getOriginSample() && isWindow() == colour->isWindow() )
        {
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------------------------
    /*void Texture::writeContentsToFile( const String& filename, Nui8 minMip, Nui8 maxMip, bool resolve )
    {
        Image image;
        read(&image, minMip, maxMip, resolve );
        image.save( filename, 0, image.getMipmapCount() );
    }*/
    //-----------------------------------------------------------------------------------
    void Texture::read(Image * img, NCount minMip, NCount maxMip, bool resolve)
    {
        assert( minMip <= maxMip );

        //OgreProfileExhaustive( "Image2::read" );

        if( getCurrentStorage() != StorageType::ST_Gpu && getStorage() != StorageType::ST_Gpu )
        {
            N_EXCEPT( InvalidParam, "Texture '" + getName() + "' must be resident or becoming resident!!!");
        }

        wait(false);

        minMip = std::min<NCount>( minMip, getNumMipmaps() - 1u );
        maxMip = std::min<NCount>( maxMip, getNumMipmaps() - 1u );

        img->create(std::max( 1u, getWidth() >> minMip ), std::max( 1u, getHeight() >> minMip ), std::max( 1u, getDepth() >> minMip ),
            getArray(), mFormat, maxMip - minMip + 1u);
        TextureManager * texmag = N_Only(Texture);

        Texture * resolveTex = this;

        if( isMultiSample() && isManualMSAA() && resolve && !isOpenGLRenderWindow())
        {
            resolveTex = texmag->createTexture(0, Texture::RT_Disk, Texture::MM_FRAME, mTextureType);
            resolveTex->copyModel( this );
            resolveTex->setSample( 1u );
            resolveTex->load( StorageType::ST_Gpu, (Nui8*)0 );
            msaa( resolveTex );
        }

        for( NCount mip=minMip; mip<=maxMip; ++mip )
        {
            NCount width  = std::max( 1u, getWidth() >> mip );
            NCount height = std::max( 1u, getHeight() >> mip );
            NCount depth  = std::max( 1u, getDepth() >> mip );
            NCount array  = std::max( depth, getArray() );

            TextureAsync * tasync = texmag->createAsync(width, height, depth, array, mTextureType, mFormat);
            tasync->write( resolveTex, mip, 0, 0, true);

            PixelBlock dstBox = img->getPixel(-1, mip - minMip );

            if( tasync->getArray() > 1)
            {
                const PixelBlock srcBox = tasync->map( 0 );
                dstBox.write( srcBox );
                tasync->unmap();
            }
            else
            {
                for( NCount i = 0; i< tasync->getArray(); ++i )
                {
                    const PixelBlock srcBox = tasync->map(i);
                    dstBox.write( srcBox );
                    //dstBox.data = dstBox.at( 0, 0, 1u );
                    ++dstBox.mArrayIdx;
                    --dstBox.mArray;
                    tasync->unmap();
                }
            }

            texmag->destroyAsync( tasync );
            //tasync = 0;
        }

        if( this != resolveTex )
            texmag->destroyTexture( resolveTex );

        if(isOpenGLRenderWindow() )
            img->verticalFlip();
    }
    //-----------------------------------------------------------------------------------
    void Texture::read(PixelBlock & dstBox, const PixelBlock & srcBox, bool resolve)
    {
        if(getCurrentStorage() != StorageType::ST_Gpu && getStorage() != StorageType::ST_Gpu)
        {
            N_EXCEPT(InvalidParam, "Texture '" + getName() + "' must be resident or becoming resident!!!");
        }

        if(!getEmptyBox(0).contains(srcBox) || !dstBox.equalSize(srcBox))
        {
            N_EXCEPT(InvalidParam, "Invalid box.");
        }

        wait(false);

        TextureManager * texmag = N_Only(Texture);

        Texture * resolve = this;

        if(isMultiSample() && isManualMSAA() && resolve && !isOpenGLRenderWindow() )
        {
            resolve = texmag->createTexture(0, Texture::RT_Disk, Texture::MM_FRAME, getType());
            resolve->copyModel(this);
            resolve->setSample(1u);
            resolve->load(StorageType::ST_Gpu, (Nui8 *)0);
            msaa( resolve );
        }

        TextureAsync *tasync = texmag->createAsync(srcBox.mWidth, srcBox.mHeight, srcBox.mDepth, srcBox.mArray, mTextureType, mFormat);
        tasync->write(resolve, 0, &srcBox, 0, true);

        if( tasync->getArray() > 1)
        {
            srcBox = tasync->map(0);
            PixelUtil::conv(srcBox, dstBox);
            tasync->unmap();
        }
        else
        {
            dstBox.mArray = 1;
            for( NCount i = 0; i < tasync->getArray(); ++i )
            {
                srcBox = tasync->map(i);
                PixelUtil::conv(srcBox, dstBox);
                //dstBox.data = dstBox.at( 0, 0, 1u );
                ++dstBox.mArrayIdx;
                tasync->unmap();
            }
        }

        texmag->destroyAsync(tasync);

        if( this != resolve )
            texmag->destroyTexture(resolve);
    }
    //-----------------------------------------------------------------------------------
    void Texture::write(Texture * texture, NCount minMip, NCount maxMip, NCount depthidx, NCount depthcnt, NCount arrayidx, NCount arraycnt)
    {
        assert(minMip <= maxMip);

        //OgreProfileExhaustive( "Image2::write" );

        if(depthcnt == -1)
            depthcnt = img->getDepth() - depthidx;

        if(arraycnt == -1)
            arraycnt = img->getArray() - arrayidx;

        if(texture->getWidth() != img->getWidth() || texture->getHeight() != img->getHeight() ||
            depthidx + depthcnt > texture->getDepth() || arrayidx + arraycnt > texture->getArray() ||
            PixelUtil::getFamily(texture->getFormat()) != PixelUtil::getFamily(img->getFormat()) ||
            texture->getMipmapCount() <= maxMip)
        {
            N_EXCEPT(InvalidParam, "Texture and Image must have matching resolution and format!");
        }

        TextureManager * texmag = N_Only(Texture);

        //We could grab 1 MappedTexture per loop. But if the MappedTexture is huge,
        //we would waste a lot of memory. So instead we try to reuse them for multiple
        //loop iterations. When we run out, we flush its contents and upload all the
        //unflushed data to the GPU; then grab a new MappedTexture and repeat
        PixelBlock unsentBoxes[256];
        PixelBlock unsentBoxesCpu[256];
        NCount readlist = 0;
        MappedTexture * mappedTex = 0;

        for( NCount i = minMip; i <= maxMip; ++i )
        {
            PixelBlock box = img->getPixel(-1, i);
            box.mDepth = depthcnt;
            box.mArray = arraycnt;
            PixelBlock dstBox;
            for( NCount tries = 0; tries < 2; ++tries )
            {
                //Grab a staging texture and start mapping it!
                if( !mappedTex )
                {
                    mappedTex = texmag->createMapped(box.mWidth, box.mHeight, box.mDepth, box.mArray, texture->getFormat());
                    //mappedTex->startMapRegion();
                }

                //This mappedTex may just have been grabbed,
                //or it may have already been during a previous mipmap iteration
                dstBox = mappedTex->map(PixelVolume(box.mWidth, box.mHeight, box.mDepth, box.mArray, texture->getFormat()));

                //Failed to map? Then the staging texture is out of space. Flush it.
                //This loop will then retry for a 2nd time and grab a new staging texture.
                if( !dstBox.mData )
                {
                    mappedTex->unmap();
                    for( NCount j = 0; j < readlist; ++j)
                    {
                        NCount mipLevel = i - readlist + j;
                        PixelBlock texBox = texture->getEmptyBox(mipLevel);
                        texBox.mFront = depthidx;
                        texBox.mArrayIdx = arrayidx;
                        texBox.mDepth = unsentBoxes[j].mDepth;
                        texBox.mArray = unsentBoxes[j].mArray;
                        mappedTex->read(texture, mipLevel, &texBox, unsentBoxesCpu[j], true);
                        mappedTex->read(texture, mipLevel, &texBox, unsentBoxes[j], false);
                    }
                    readlist = 0;
                    texmag->removeMapped(mappedTex);
                    mappedTex = 0;
                }
                else
                {
                    break;
                }
            }

            unsentBoxesCpu[readlist] = box;
            unsentBoxes[readlist++] = dstBox;
            dstBox.write( box );
        }

        //Flush what's left
        mappedTex->unmap();
        for( NCount j = 0; j < readlist; ++j)
        {
            NCount mipLevel = maxMip + 1u - readlist + j;
            PixelBlock texBox = texture->getEmptyBox(mipLevel);
            texBox.mFront   = depthidx;
            texBox.mArrayIdx = arrayidx;
            texBox.mDepth   = unsentBoxes[j].mDepth;
            texBox.mArray   = unsentBoxes[j].mArray;
            mappedTex->read(texture, mipLevel, &texBox, unsentBoxesCpu[j], true);
            mappedTex->read(texture, mipLevel, &texBox, unsentBoxes[j]);
        }
        //readlist = 0;
        texmag->removeMapped(mappedTex);
        //mappedTex = 0;
    }
    //-----------------------------------------------------------------------------------
    PixelBlock Texture::getEmptyBox(NCount mipLevel)
    {
        PixelBlock re(std::max(1u, mWidth >> mipLevel), std::max(1u, mHeight >> mipLevel), std::max(1u, getDepth() >> mipLevel),
            mArray, PixelUtil::getUnitSize(mFormat), getRowSize(mipLevel), getSliceSize(mipLevel), mFormat);

        return re;
    }
    //-----------------------------------------------------------------------------------
    PixelBlock Texture::getDataBlock(NCount mipLevel)
    {
        if( mData )
        {
            N_assert( mipLevel < mMipmapCount );

            if( !isValid() )
                N_Only(Texture).wait( this, false, true );

            void * data = PixelUtil::getData(mData, mWidth, mHeight, mDepth, mArray, mipLevel, mFormat);

            PixelBlock re( std::max( 1u, mWidth >> mipLevel ), std::max( 1u, mHeight >> mipLevel ), std::max( 1u, mDepth >> mipLevel ), mArray,
                PixelUtil::getUnitSize(mFormat), getRowSize(mipLevel), getSliceSize(mipLevel));
            re.mData = data;
            re.setFormat(mFormat);
            return re;
        }
        return PixelBlock();
    }
    //-----------------------------------------------------------------------------------
    Nui8 * Texture::getData(NCount mipLevel, bool create)
    {
        if(mData)
        {
            N_assert(mipLevel < mMipmapCount);

            if(!isValid())
                N_Only(Texture).wait(this , false, true);

            return PixelUtil::getData(mData, mWidth, mHeight, mDepth, mArray, mipLevel, mFormat);
        }
        else if(create)
        {
            mData = reinterpret_cast<Nui8 *>(N_alloc_simd(getSize(), MEMCATEGORY_RESOURCE));

            return PixelUtil::getData(mData, mWidth, mHeight, mDepth, mArray, mipLevel, mFormat);
        }
        return 0;
    }
    //-----------------------------------------------------------------------------------
    NCount Texture::getRowSize(NCount mipLevel)
    {
        N_assert(mipLevel < mMipmapCount);
        return PixelUtil::getSize( std::max( mWidth >> mipLevel, 1u ), 1u, 1u, 1u, mFormat);
    }
    //-----------------------------------------------------------------------------------
    NCount Texture::getSliceSize(NCount mipLevel)
    {
        N_assert(mipLevel < mMipmapCount);
        uint32 width  = std::max( mWidth >> mipLevel, 1u );
        uint32 height = std::max( mHeight >> mipLevel, 1u );
        return PixelUtil::getSize( width, height, 1u, 1u, mFormat);
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isMetadataValid() const
    { 
        return ((mCurrentStorage == StorageType::ST_Gpu && mNextStorage == StorageType::ST_Gpu) ||
            (mCurrentStorage == StorageType::ST_Cpu && mNextStorage != StorageType::ST_Disk)) && mTaskCount == 0;
    }
    //-----------------------------------------------------------------------------------
    bool Texture::isValid() const
    {
        return isValidImpl() && mTaskCount == 0u;
    }
    //-----------------------------------------------------------------------------------
    void Texture::wait(bool metadata)
    {
        if(metadata && isMetadataValid())
            return;
        else(!metadata && isValid())
            return;
        N_Only(Texture).wait(this, metadata);
    }
    //-----------------------------------------------------------------------------------
    void Texture::load(const Texture * src)
    {
        if(src->getUnitCount() != mArray)
        {
            N_EXCEPT(InvalidParam, _I18n("纹理中的图片数量不一致! "));
        }
        NCount mipmap = std::min(getMipmapCount(), src->getMipmapCount());

        if((mMark & Texture::MM_MIPMAP_AUTO) || (src->getModeMark() & Texture::MM_MIPMAP_AUTO))
            mipmap = 0;

        for(NCount unit = 0; unit < mArray; ++unit)
        {
            for(NCount mip = 0; mip <= mipmap; ++mip)
            {
                getCubeBuffer((CubeFace)unit, mip)->write(const_cast<Texture *>(src)->getCubeBuffer((CubeFace)unit, mip));
            }
        }
    }
    //--------------------------------------------------------------------------
    void Texture::load(const Image * src)
    {
        LoadingState old = mState;
        if(mState != LS_UNLOADED && mState != LS_PLANNED)
            return;

        mState = LS_LOADING;

        try
        {
            N_mutex1_lock
            ImageList images;
            images.push_back(const_cast<Image *>(src));
            load(images);
        }
        catch(...)
        {
            mState = old;
            throw;
        }

        mState = LS_LOADED;

        // 通知管理器
        if(mCreator)
            mCreator->onLoad(this);

        // No deferred loading events since this method is not called in background
    }
    //--------------------------------------------------------------------------
    void Texture::load(const ImageList & images)
    {
        if(images.empty())
            N_EXCEPT(InvalidParam, _I18n("Cannot load empty vector of images"));

        mSrcWidth = mWidth = images[0]->getWidth();
        mSrcHeight = mHeight = images[0]->getHeight();
        mSrcDepth = mDepth = images[0]->getDepth();
        mSrcFormat = images[0]->getFormat();

        if (!mOriginList.empty() && mTextureType != T_CUBE)
            mDepth = mOriginList.size();

        if(((mMark & MM_COLOUR_ALPHA) && !(mMark & MM_COLOUR_RGB)) && (mSrcFormat == PF_L8ui))
        {
            mSrcFormat = PF_A8UN;
        }

        if(mFormat == PF_UNKNOWN)
        {
            if(mMark & MM_DEPTH_BIT_16)
                mFormat = PixelUtil::getFormat(mSrcFormat, 16, 16);
            else if(mMark & MM_DEPTH_BIT_32)
                mFormat = PixelUtil::getFormat(mSrcFormat, 32, 32);
        }

        NCount imageMips = images[0]->getMipmapCount();
        if(imageMips > 0)
        {
            mSrcMipmapCount = mMipmapCount = images[0]->getMipmapCount();
            mMark &= ~Texture::MM_MIPMAP_AUTO;
        }

        if(!mInnerValid)
        {
            createInternalImpl();
            mInnerValid = true;
        }

        NCount faces;
        bool multiImage;
        if(images.size() > 1)
        {
            faces = images.size();
            multiImage = true;
        }
        else
        {
            faces = images[0]->getArray();
            multiImage = false;
        }

        if(faces > mArray)
            faces = mArray;

#if (NII_USERMODE != NII_APPMODE)
        if(N_Only(Texture).getVerbose())
        {
            // 说我们正在做的
            StringStream str;
            str << _I18n("纹理: ") << mOriginID << _I18n("加载: ") << faces << _I18n(" 面片")
                << "(" << PixelUtil::getMarkName(images[0]->getFormat()) << "," <<
                images[0]->getWidth() << "x" << images[0]->getHeight() << "x" <<
                images[0]->getDepth() << ")";
            if(!(mMark & MM_MIPMAP_HW && mSrcMipmapCount == 0))
            {
                str << _I18n("映射图层数量: ") << mSrcMipmapCount;
                if(mMark & Texture::MM_MIPMAP_AUTO)
                {
                    if(mMark & MM_MIPMAP_HW)
                        str << _I18n(" 硬件");

                    str << _I18n("自动生成映射图层");
                }
                else
                {
                    str << _I18n(" 自定义映射图层");
                }
                if(multiImage)
                    str << _I18n(" 纹理来自多个图片");
                else
                    str << _I18n(" 纹理来自单个图片");
            }
            {
                FrameBuffer * buf = getCubeBuffer((CubeFace)0, 0);
                str << _I18n(" 内部格式: ") << PixelUtil::getMarkName(buf->getFormat()) << "," << 
                    buf->getWidth() << "x" << buf->getHeight() << "x" << buf->getDepth() << ".";
            }
            N_Only(Log).log(LL_Info, str.str());
        }
#endif
        // 主要加载循环,设置内部的图象细节及内部缓存
        // imageMips == 0 图像没有自定义 mipmaps,否则包含了自定义mips数量
        for(NCount mip = 0; mip <= std::min(mSrcMipmapCount, imageMips); ++mip)
        {
            for(NCount i = 0; i < std::max(faces, images.size()); ++i)
            {
                PixelBlock src;
                size_t face = (mDepth == 1) ? i : 0; // depth = 1, then cubemap face else 3d/ array layer

                FrameBuffer * buffer = getCubeBuffer((CubeFace)face, mip);
                Box dst(0, 0, 0, buffer->getWidth(), buffer->getHeight(), buffer->getDepth());

                if(multiImage)
                {
                    src = images[i]->getPixel(0, mip);
                    // set dst layer
                    if (mDepth > 1)
                    {
                        dst.mFront = i;
                        dst.mBack = i + 1;
                    }
                }
                else
                {
                    src = images[0]->getPixel(i, mip);
                }

                // 设置已处理格式防止不相同
                src.mFormat = mSrcFormat;
                // 应用伽玛校正
                if(mGamma != 1.0f)
                {
                    // 为转换缓存的范围的删除
                    MemStream * buf = N_new MemStream(PixelUtil::getSize(src.getWidth(),
                        src.getHeight(), src.getDepth(), src.getArray(), src.mFormat));

                    // 不要覆盖原始图像但在临时缓存伽玛要校正
                    PixelBlock corrected = PixelBlock(src.getWidth(), src.getHeight(),
                        src.getDepth(), src.mFormat, buf->getData());

                    PixelUtil::conv(src, corrected);
                    // 应用伽玛
                    PixelBlock::applyGamma(corrected.mData, mGamma, corrected.getWholeSliceSize(),
                        static_cast<Nui8>(PixelUtil::getUnitSize(src.mFormat) * 8));

                    //目的地:整个纹理。blitFrom为我们需要时,缩放2次幕
                    buffer->write(corrected, dst);
                    N_delete buf;
                }
                else
                {
                    //目的地:整个纹理。blitFrom为我们需要时,缩放2次幕
                    buffer->write(src, dst);
                }
            }
        }

        mSize = PixelUtil::getSize(mWidth, mHeight, mDepth, mArray, mFormat);
    }
    //--------------------------------------------------------------------------
    void Texture::load(const Image & org, const ImageList & src)
    {

    }
    //------------------------------------------------------------------------
    void Texture::load(const String & filename, GroupID gid)
    {
        Image img;
        img.load(filename, gid);
        load(&img);
    }
    //--------------------------------------------------------------------------
    void Texture::load(DataStream * stream, NCount w, NCount h, PixelFormat pf)
    {
        Image img;
        img.load(stream, w, h, pf);
        load(&img);
    }
    //------------------------------------------------------------------------
    void Texture::load(Nui8 * buffer, NCount w, NCount h, PixelFormat pf)
    {
        PixelFormat supportpf = N_Only(Texture).getFormat(Texture::T_2D, pf,
            Texture::MM_MIPMAP_AUTO | Buffer::M_NORMAL );

        if(PixelUtil::getUnitSize(supportpf) < PixelUtil::getUnitSize(pf))
            N_EXCEPT(InvalidRequest, _I18n("不支持的像素格式."));

        Image img;
        img.load(buffer, w, h, pf, false);
        load(&img);
    }
    //-----------------------------------------------------------------------------------
    void Texture::setResolution(NCount width, NCount height, NCount depth, NCount array)
    {
        N_assert(mCurrentStorage == StorageType::ST_Disk || isWindow());
        mWidth = width;
        mHeight = height;

        if(mTextureType == Texture::T_CUBE)
        {
            mDepth = 1;
            mArray = 6
        }
        else
        {
            N_assert((mTextureType != Texture::T_CUBEArray || (mTextureType % 6u) == 0) &&
                "depthOrSlices must be a multiple of 6 for T_CUBEArray textures!" );
            mDepth = depth;
            mArray = array;
        }
    }
    //--------------------------------------------------------------------------
    NCount Texture::calcSize() const
    {
        NCount re = PixelUtil::getSize(mWidth, mHeight, mDepth, mArray, mFormat, mMipmapCount);

        if(mSrcSample.isMultiSample())
        {
            if(isManualMSAA())
                re *= mSrcSample.getMultiSampling();
            else
                re *= (mSrcSample.getMultiSampling() + 1u);
        }

        return re;
    }
    //---------------------------------------------------------------------------------------------
    FrameBuffer * Texture::getBuffer(Nidx mipmap)
    {
        if(mipmap > mSrcMipmapCount)
            N_EXCEPT(InvalidParam, _I18n("Mipmap index out of range"));

        N_assert(mipmap < mSurfaceList.size());
        return mSurfaceList[mipmap];
    }
    //-------------------------------------------------------------------------
    FrameBuffer * Texture::getCubeBuffer(CubeFace face, Nidx mipmap)
    {
        if ((NCount)face >= mArray)
            N_EXCEPT(InvalidParam, _I18n("Face index out of range"));

        if (mipmap > mSrcMipmapCount)
            N_EXCEPT(InvalidParam, _I18n("Mipmap index out of range"));

        Nidx idx = face * (mSrcMipmapCount + 1) + mipmap;
        N_assert(idx < mSurfaceList.size());
        return mSurfaceList[idx];
    }
    //-----------------------------------------------------------------------------
    TextureBuffer * Texture::createWindow(PixelFormat pf)
    {
        TextureBufferList::iterator i, iend = mViewList.end();
        for (i = mViewList.begin(); i != iend; ++i)
        {
            if( (*i)->getFormat() == pf)
            {
                return *i;
            }
        }
        return createViewImpl(pf);
    }
    //-----------------------------------------------------------------------------
    void Texture::destroyView(PixelFormat pf)
    {
        TextureBufferList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            if ((*i)->getFormat() == pf)
            {
                N_delete *i;
                mViewList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void Texture::destroyAllView()
    {
        TextureBufferList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            N_delete *i;
            ++i;
        }
        mViewList.clear();
    }
    //-----------------------------------------------------------------------------
    void Texture::unloadImpl()
    {
        if(mInnerValid)
        {
            FrameBufferList::iterator i, iend = mSurfaceList.end();
            for (i = mSurfaceList.begin(); i != iend; ++i)
            {
                N_delete *i;
            }
            mSurfaceList.clear();
            freeInternalImpl();
            mInnerValid = false;
        }
    }
    //-----------------------------------------------------------------------------
    CodecID Texture::getRawType() const
    {
        if(mSrc.empty())
            return N_CODEC_UNKNOW;

        String::size_type pos = mSrc.find_last_of(_T("."));
        if(pos != String::npos && pos < (mSrc.length() - 1))
        {
            String ext = mSrc.substr(pos+1);
            StrUtil::toLower(ext);
            if(ext == _T("dds"))
                return N_IMAGE_DDS;
        }
        else
        {
            DataStream * dstream = 0;
            try
            {
                dstream = N_Only(ResourceScheme).open(mSrc, mGroup, true, 0);
            }
            catch(Exception &)
            {
            }
            if(dstream == 0 && getType() == Texture::T_CUBE)
            {
                // 再尝试其中一个面(非-dds)
                try
                {
                    dstream = N_Only(ResourceScheme).open(mSrc + _T("_rt"), mGroup, true, 0);
                }
                catch (Exception &)
                {
                }
            }
            if(dstream != 0)
            {
                Codec * code = Codec::getCodec(dstream);
                N_delete dstream;

                CodecID temp;
                if(code != 0)
                    temp = code->getCodeID();

                if(temp == N_IMAGE_DDS)
                    return N_IMAGE_DDS;
            }
        }
        return N_CODEC_UNKNOW;
    }
    //-----------------------------------------------------------------------
    static void readImage(ImageList & images, const String & name, GroupID group, const String & ext, bool haveNPOT)
    {
        DataStream * dstream = N_Only(ResourceScheme).open(name, group, true, 0);

        images.push_back(N_new Image());
        Image * img = images.back();
        img->load(dstream, ext);

        if (haveNPOT)
        {
            N_delete dstream;
            return;
        }
        // Scale to nearest power of 2
        Nui32 w = Bitwise::firstPO2From(img->getWidth());
        Nui32 h = Bitwise::firstPO2From(img->getHeight());
        if ((img->getWidth() != w) || (img->getHeight() != h))
            img->resize(w, h);

        N_delete dstream;
    }
    //----------------------------------------------------------------------
    void Texture::planImpl()
    {
        if (mMark & Texture::MM_FRAME_RGB)
            return;

        const RenderFeature * renderCaps = N_Engine().getRender()->getFeature();

        bool haveNPOT = renderCaps->isSupport(GF_Tex_NonPow2) ||
            (renderCaps->isOnlyPow2Texture() && mSrcMipmapCount == 0);

        String baseName, ext;
        StrUtil::splitBaseFilename(mSrc, baseName, ext);

        ImageList loadedImages;

        try
        {
            if (mOriginList.empty())
            {
                readImage(loadedImages, mSrc, mGroup, ext, haveNPOT);

                // If this is a volumetric texture set the texture type flag accordingly.
                // If this is a cube map, set the texture type flag accordingly.
                if (loadedImages[0]->isMode(Image::M_Cube))
                    mTextureType = T_CUBE;
                // If this is a volumetric texture set the texture type flag accordingly.
                if (loadedImages[0]->getDepth() > 1 && mTextureType != Texture::T_2DArray)
                    mTextureType = T_3D;
            }
        }
        catch (const NotExistException&)
        {
            if (mTextureType == T_CUBE)
            {
                mOriginList.resize(6);
                mOriginList[0] = baseName + _T("_rt") + ext;
                mOriginList[1] = baseName + _T("_lf") + ext;
                mOriginList[2] = baseName + _T("_up") + ext;
                mOriginList[3] = baseName + _T("_dn") + ext;
                mOriginList[4] = baseName + _T("_fr") + ext;
                mOriginList[5] = baseName + _T("_bk") + ext;
            }
            else if (mTextureType == Texture::T_2DArray)
            { // ignore
            }
            else
                throw; // rethrow
        }

        // read sub-images
        for (const String& name : mOriginList)
        {
            StrUtil::splitBaseFilename(name, baseName, ext);
            readImage(loadedImages, name, mGroup, ext, haveNPOT);
        }

        // If compressed and 0 custom mipmap, disable auto mip generation and
        // disable software mipmap creation.
        // Not supported by GLES.
        if (PixelUtil::isCompress(loadedImages[0]->getFormat()) &&
            !renderCaps->isSupport(GF_Tex_AutoMipmapCompress) && loadedImages[0]->getMipmapCount() == 0)
        {
            mSrcMipmapCount = mMipmapCount = 0;
            // Disable flag for auto mip generation
            mMark &= ~MM_MIPMAP_AUTO;
        }

        // avoid copying Image data
        std::swap(mLoadedImages, loadedImages);
    }
    //-----------------------------------------------------------------------
    void Texture::unplanImpl()
    {
        ImageList::iterator i, iend = mLoadedImages.end();
        for (i = mLoadedImages.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mLoadedImages.clear();
    }
    //-----------------------------------------------------------------------
    void Texture::loadImpl()
    {
        if (mMark & Texture::MM_FRAME_RGB)
        {
            if(!mInnerValid)
            {
                createInternalImpl();
                mInnerValid = true;
            }
            return;
        }

        // 现在仅是副本在栈中,而且如果_load有异常抛出它将会被清理掉
        ImageList loadedImages;
        std::swap(loadedImages, mLoadedImages);

        load(loadedImages);

        ImageList::iterator i, iend = loadedImages.end();
        for (i = loadedImages.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
    }
    //----------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextureFactory
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdInputFileName : public PropertyCmd
    {
    public:
        CmdInputFileName() : PropertyCmd(N_PropertyExtTexture_FileName, _T("filename"),
            _T("A source for the texture effect (only certain plugins require this)"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const TextureFactory *>(own)->getInputName();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<TextureFactory *>(own)->setInputName(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const TextureFactory *>(own)->getInputName();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            String * temp = (String *)in;
            static_cast<TextureFactory *>(own)->setInputName(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdInputFileName();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdFPS : public PropertyCmd
    {
    public:
        CmdFPS() : PropertyCmd(N_PropertyExtTexture_FPS, _T("frames_per_second"),
            _T("How fast should playback be (only certain plugins use this)"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextureFactory *>(own)->getFPS(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIi temp;
            StrConv::conv(val, temp);
            static_cast<TextureFactory *>(own)->setFPS(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIi * temp = (NIIi *)out;
            *temp = static_cast<const TextureFactory *>(own)->getFPS();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIi * temp = (NIIi *)in;
            static_cast<TextureFactory *>(own)->setFPS(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFPS();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIi)24, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdPlayMode : public PropertyCmd
    {
    public:
        CmdPlayMode() : PropertyCmd(N_PropertyExtTexture_PlayMode, _T("play_mode"),
            _T("How the playback starts(only certain plugins use this)"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            TextureFactory::PlayState mode = static_cast<const TextureFactory*>(own)->getPlayMode();
            switch(mode)
            {
            case TextureFactory::PS_Play:
                out = _T("play");
                break;
            case TextureFactory::PS_Loop:
                out = _T("loop");
                break;
            case TextureFactory::PS_Stop:
                out = _T("stop");
                break;
            case TextureFactory::PS_Pause:
                out = _T("pause");
                break;
            default:
                out = _T("pause");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TextureFactory::PlayState mode = TextureFactory::PS_Pause;

            if(val == _T("play"))
                mode = TextureFactory::PS_Play;
            else if(val == _T("stop"))
                mode = TextureFactory::PS_Stop;
            else if (val == _T("loop"))
                mode = TextureFactory::PS_Loop;
            else if(val == _T("pause"))
                mode = TextureFactory::PS_Pause;

            static_cast<TextureFactory *>(own)->setPlayMode(mode);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TextureFactory::PlayState * temp = (TextureFactory::PlayState *)out;
            *temp = static_cast<const TextureFactory*>(own)->getPlayMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            TextureFactory::PlayState * temp = (TextureFactory::PlayState *)in;
            static_cast<TextureFactory *>(own)->setPlayMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPlayMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("pause");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdShaderFusion : public PropertyCmd
    {
    public:
        CmdShaderFusion() : PropertyCmd(N_PropertyExtTexture_ShaderFusion, _T("setShaderFusion"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextureFactory *>(own)->getShaderFusion(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIi t;
            StrConv::conv(val, t);
            static_cast<TextureFactory *>(own)->setShaderFusion(t);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIi * temp = (NIIi *)out;
            *temp = static_cast<const TextureFactory *>(own)->getShaderFusion();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIi * temp = (NIIi *)in;
            static_cast<TextureFactory *>(own)->setShaderFusion(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdShaderFusion();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdShaderCh : public PropertyCmd
    {
    public:
        CmdShaderCh() : PropertyCmd(N_PropertyExtTexture_ShaderCh, _T("setShaderCh"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextureFactory *>(own)->getShaderCh(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIi t;
            StrConv::conv(val, t);
            static_cast<TextureFactory *>(own)->setShaderCh(t);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIi * temp = (NIIi *)out;
            *temp = static_cast<const TextureFactory *>(own)->getShaderCh();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIi * temp = (NIIi *)in;
            static_cast<TextureFactory *>(own)->setShaderCh(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdShaderCh();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdRenderLevel : public PropertyCmd
    {
    public:
        CmdRenderLevel() : PropertyCmd(N_PropertyExtTexture_RenderLevel, _T("setRenderLevel"),
            _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv((Nui32)static_cast<const TextureFactory *>(own)->getRenderLevel(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIi t;
            StrConv::conv(val, t);

            static_cast<TextureFactory *>(own)->setRenderLevel(t);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIi * temp = (NIIi *)out;
            *temp = static_cast<const TextureFactory *>(own)->getRenderLevel();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIi * temp = (NIIi *)in;
            static_cast<TextureFactory *>(own)->setRenderLevel(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRenderLevel();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    TextureFactory::TextureFactory(PluginID pid, LangID lid) :
        PropertyCmdObj(N_CmdObj_ExternalTexture, lid),
        Plugin(pid, T_Process_Image)
    {
        setupCmd(N_CmdObj_ExternalTexture);

        mInputFileName = _T("None");
        mPreFrame = false;
        mFramesPerSecond = 24;
        mMode = PS_Pause;
    }
    //------------------------------------------------------------------------
    bool TextureFactory::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdInputFileName());
        dest->add(N_new CmdFPS());
        dest->add(N_new CmdPlayMode());
        dest->add(N_new CmdShaderFusion());
        dest->add(N_new CmdShaderCh());
        dest->add(N_new CmdRenderLevel());
        return true;
    }
    //------------------------------------------------------------------------
}