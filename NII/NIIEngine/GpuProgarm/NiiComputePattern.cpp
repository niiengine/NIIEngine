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

#include "NiiMurmurHash.h"

#include <fstream>

namespace NII
{
    class ComputeStateCache : public GpuParamsAlloc
    {
        ComputeRaw * mRaw;
        ComputeStateObject mCSO;
        PropertyValueList mPropertyList;
        Nui32 paramsUpdateCounter;
        Nui32 paramsProfileUpdateCounter;

        ComputeStateCache() : mRaw(0), paramsUpdateCounter(~0u), paramsProfileUpdateCounter(~0u) {}
        ComputeStateCache(ComputeRaw * raw, const PropertyValueList & vlist) :
            mRaw(raw), mPropertyList(vlist), paramsUpdateCounter(~0u), paramsProfileUpdateCounter(~0u) {}

        bool operator == (const ComputeStateCache & o) const
        {
            return mPropertyList == o.mPropertyList && mRaw == o.mRaw;
        }
    };

    struct ComputeProperty
    {
        static const Nid ThreadsPerGroupX = IdString( "threads_per_group_x" );
        static const Nid ThreadsPerGroupY = IdString( "threads_per_group_y" );
        static const Nid ThreadsPerGroupZ = IdString( "threads_per_group_z" );
        static const Nid NumThreadGroupsX = IdString( "num_thread_groups_x" );
        static const Nid NumThreadGroupsY = IdString( "num_thread_groups_y" );
        static const Nid NumThreadGroupsZ = IdString( "num_thread_groups_z" );

        static const Nid TypedUavLoad       = IdString( "typed_uav_load" );

        static const Nid NumTextureSlots    = IdString( "num_texture_slots" );
        static const Nid MaxTextureSlot     = IdString( "max_texture_slot" );
        static const char *Texture          = "texture";

        static const Nid NumUavSlots        = IdString( "num_uav_slots" );
        static const Nid MaxUavSlot         = IdString( "max_uav_slot" );
        static const char *Uav              = "uav";
    };
    //-----------------------------------------------------------------------------------
    template <typename T>
    void removeListenerFromTextures(T & container, size_t first, size_t lastPlusOne)
    {
        typename T::const_iterator itor = container.begin() + first;
        typename T::const_iterator end  = container.begin() + lastPlusOne;
        while(itor != end)
        {
            if(itor->isTexture() && itor->getTexture().mTexture)
                itor->getTexture().mTexture->removeListener(this);
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ComputeRaw
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    static const IdString c_textureTypesProps[8] =
    {
        "TextureTypes_Unknown",
        "TextureTypes_Type1D",
        "TextureTypes_Type1DArray",
        "TextureTypes_Type2D",
        "TextureTypes_Type2DArray",
        "TextureTypes_TypeCube",
        "TextureTypes_TypeCubeArray",
        "TextureTypes_Type3D"
    };
    //-----------------------------------------------------------------------------------
    ComputeRaw::ComputeRaw(Nid name, ComputePattern * creator, const String & source, const SegmentList & slist) :
        mParent(creator),
        mID(name),
        mSourceFilename(source),
        mSegmentList(slist),
        mGroupBaseType(GBT_Null),
        mTextureList( 0 ),
        mSampleList( 0 ),
        mGpuBufferList( 0 ),
        mTextureStateMark( false ),
        mTextureUnitMax( 0 ),
        mBufferUnitMax( 0 ),
        mParamValid( -1 )
    {
        memset(mCeilCount, 0, sizeof(mCeilCount));
        memset(mGroupCount, 0, sizeof(mGroupCount));

        mGroupsDivisor[0] = 1;
        mGroupsDivisor[1] = 1;
        mGroupsDivisor[2] = 1;
        mGroupsDivisor[3] = 0;
    }
    //-----------------------------------------------------------------------------------
    ComputeRaw::~ComputeRaw()
    {
        destroyAllSampler();
        destroyAllTexture();
        destroyAllStorage();

        removeListenerFromTextures(mBufferUnitList, 0, mBufferUnitList.size());
        removeListenerFromTextures(mTextureUnitList, 0, mTextureUnitList.size());

        RenderSys * rsys = mParent->getRender();
        vectorL<const TextureSampleUnit *>::const_iterator itor = mSamplerUnitList.begin();
        vectorL<const TextureSampleUnit *>::const_iterator end  = mSamplerUnitList.end();

        while(itor != end)
        {
            if(*itor)
                rsys->destroyTexSample(*itor);
            ++itor;
        }

        mSamplerUnitList.clear();
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::destroyAllSampler()
    {
        if(mSampleList)
        {
            mParent->getRender()->destroySampler(mSampleList);
            mSampleList = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::destroyAllTexture()
    {
        if(mTextureList)
        {
            mParent->getRender()->destroyTexture(mTextureList);
            mTextureList = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::destroyAllStorage()
    {
        if(mGpuBufferList)
        {
            mParent->getRender()->destroyBuffer(mGpuBufferList);
            mGpuBufferList = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void setTextureProperties( const Texture *texture, PixelFormat pf, Buffer::OpType access, LwString & propName, ComputeRaw * obj)
    {
        if( pf == PF_UNKNOWN )
            pf = texture->getFormat();

        const size_t texturePropSize = propName.size();

        propName.a( "_width" );                 //texture0_width
        add( propName.c_str(), texture->getWidth() );
        propName.resize( texturePropSize );

        propName.a( "_height" );                //texture0_height
        add( propName.c_str(), texture->getHeight() );
        propName.resize( texturePropSize );

        propName.a( "_depth" );                 //texture0_depth
        add( propName.c_str(), texture->getVolume() );
        propName.resize( texturePropSize );

        propName.a( "_mipmaps" );               //texture0_mipmaps
        add( propName.c_str(), texture->getNumMipmaps() );
        propName.resize( texturePropSize );

        propName.a( "_msaa" );                  //texture0_msaa
        add( propName.c_str(), texture->isMultiSample() ? 1 : 0 );
        propName.resize( texturePropSize );

        propName.a( "_msaa_samples" );          //texture0_msaa_samples
        add( propName.c_str(), texture->getOriginSample().getMultiSampling() );
        propName.resize( texturePropSize );

        propName.a( "_texture_type" );          //_texture_type
        add( propName.c_str(), c_textureTypesProps[texture->getActualType()].mHash );
        propName.resize( texturePropSize );

        propName.a( "_pf_type" );               //uav0_pf_type
        const char *typeName = N_OnlyPtr(GpuProgram)->getToken( pf );
        if( typeName )
            obj->setSegment( propName.c_str(), typeName );
        propName.resize( texturePropSize );

        propName.a( "_orig_pf_srgb" );  // uav0_orig_pf_srgb
        add( propName.c_str(), PixelUtil::isSRGB( texture->getFormat() ) );
        propName.resize( texturePropSize );

        propName.a( "_data_type" );             //uav0_data_type
        const char *dataType = N_OnlyPtr(GpuProgram)->getToken( pf, texture->getActualType(), texture->isMultiSample(), access );

        //if( typeName )
        if(dataType)
            obj->setSegment( propName.c_str(), dataType );
        propName.resize( texturePropSize );

        if( PixelUtil::isInteger( pf ) )
        {
            propName.a( "_is_integer" );        //uav0_is_integer
            add( propName.c_str(), 1 );
            propName.resize( texturePropSize );
        }
        if( PixelUtil::isSigned( pf ) )
        {
            propName.a( "_is_signed" );         //uav0_is_signed
            add( propName.c_str(), 1 );
            propName.resize( texturePropSize );
        }
    }
    //-----------------------------------------------------------------------------------
    void clearAutoProperties( const char * propTexture, uint8 maxTexUnitReached, ComputeRaw * obj)
    {
        assert( propTexture == ComputeProperty::Texture || propTexture == ComputeProperty::Uav );
        char tmpData[64];
        LwString propName = LwString::FromEmptyPointer( tmpData, sizeof(tmpData) );
        propName = propTexture; //It's either ComputeProperty::Texture or ComputeProperty::Uav

        const size_t texturePropNameSize = propName.size();

        //Remove everything from any previous run.
        for( uint8 i = 0; i < maxTexUnitReached; ++i )
        {
            propName.resize( texturePropNameSize );
            propName.a( i );                        //texture0 or uav0

            const size_t texturePropSize = propName.size();

            propName.a( "_width" );                 //texture0_width
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_height" );                //texture0_height
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_depth" );                 //texture0_depth
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_mipmaps" );               //texture0_mipmaps
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_msaa" );                  //texture0_msaa
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_msaa_samples" );          //texture0_msaa_samples
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_texture_type" );          //texture0_texture_type
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_is_buffer" );             //texture0_is_buffer
            obj->remove( propName.c_str() );
            propName.resize( texturePropSize );

            propName.a( "_pf_type" );           //uav0_pf_type
            obj->removeSegment( propName.c_str() );
            propName.resize( texturePropSize );

            //Note we're comparing pointers, not string comparison!
            if( propTexture == ComputeProperty::Uav )
            {
                propName.a( "_width_with_lod" );    //uav0_width_with_lod
                obj->remove( propName.c_str() );
                propName.resize( texturePropSize );

                propName.a( "_height_with_lod" );   //uav0_height_with_lod
                obj->remove( propName.c_str() );
                propName.resize( texturePropSize );

                propName.a( "_is_integer" );        //uav0_is_integer
                obj->remove( propName.c_str() );
                propName.resize( texturePropSize );

                propName.a( "_is_signed" );         //uav0_is_signed
                obj->remove( propName.c_str() );
                propName.resize( texturePropSize );

                propName.a( "_data_type" );         //uav0_data_type
                obj->removeSegment( propName.c_str() );
                propName.resize( texturePropSize );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::_syncParam()
    {
        RenderSys * rsys = mParent->getRender();
        const bool typedUavs = rsys->getFeature()->hasCapability( CF_Program_StorgeBuffer );
        add( ComputeProperty::TypedUavLoad, typedUavs ? 1 : 0 );

        remove( ComputeProperty::NumTextureSlots );
        remove( ComputeProperty::MaxTextureSlot );
        remove( ComputeProperty::NumUavSlots );
        remove( ComputeProperty::MaxUavSlot );

        for( size_t i=0; i<sizeof(c_textureTypesProps) / sizeof(c_textureTypesProps[0]); ++i )
            remove( c_textureTypesProps[i] );

        clearAutoProperties( ComputeProperty::Texture, mTextureUnitMax, this);
        clearAutoProperties( ComputeProperty::Uav, mBufferUnitMax, this );

        mTextureUnitMax = 0;
        mBufferUnitMax = 0;

        if( mTextureStateMark )
        {
            for( size_t i=0; i<sizeof(c_textureTypesProps) / sizeof(c_textureTypesProps[0]); ++i )
                add( c_textureTypesProps[i], c_textureTypesProps[i].mHash );

            //const GpuProgramManager * toShaderType = N_OnlyPtr(GpuProgram);

            char tmpData[64];
            LwString propName = LwString::FromEmptyPointer( tmpData, sizeof(tmpData) );

            //Deal with textures
            {
                //Inform number of UAVs
                add( ComputeProperty::NumTextureSlots, static_cast<int32>( mTextureUnitList.size() ) );

                propName = ComputeProperty::Texture;
                const size_t texturePropNameSize = propName.size();

                TextureUnitList::const_iterator begin= mTextureUnitList.begin();
                TextureUnitList::const_iterator itor = mTextureUnitList.begin();
                TextureUnitList::const_iterator end  = mTextureUnitList.end();

                while( itor != end )
                {
                    const size_t idx = itor - begin;
                    propName.resize( texturePropNameSize );
                    propName.a( static_cast<Nui32>(idx) ); //texture0
                    const size_t texturePropSize = propName.size();

                    if( !itor->empty() )
                    {
                        add( propName.c_str(), 1 );

                        if( itor->isTexture() )
                        {
                            setTextureProperties( itor->getTexture().texture, itor->getTexture().pixelFormat,
                                ResourceAccess::Undefined, propName, this );
                        }
                        else if( itor->isBuffer() )
                        {
                            propName.a( "_is_buffer" );             //texture0_is_buffer
                            add( propName.c_str(), 1 );
                            propName.resize( texturePropSize );
                        }
                    }

                    ++itor;
                }

                mTextureUnitMax = mTextureUnitList.size();
            }

            //Deal with UAVs
            {
                //Inform number of UAVs
                add( ComputeProperty::NumUavSlots, static_cast<int32>( mBufferUnitList.size() ) );

                propName = ComputeProperty::Uav;
                const size_t texturePropNameSize = propName.size();

                BufferUnitList::const_iterator begin= mBufferUnitList.begin();
                BufferUnitList::const_iterator itor = mBufferUnitList.begin();
                BufferUnitList::const_iterator end  = mBufferUnitList.end();

                while( itor != end )
                {
                    const size_t idx = itor - begin;
                    propName.resize( texturePropNameSize );
                    propName.a( static_cast<Nui32>(idx) ); //uav0
                    const size_t texturePropSize = propName.size();

                    if( !itor->empty() )
                        add( propName.c_str(), 1 );

                    if( itor->isTexture() && itor->getTexture().texture )
                    {
                        const GpuParamBuffer::SamplerUnit &texSlot = itor->getTexture();
                        setTextureProperties( texSlot.texture, texSlot.pixelFormat, texSlot.access, propName, this );

                        const Texture *texture = texSlot.texture;

                        Nui32 mipLevel = std::min<Nui32>( texSlot.mipmapLevel, texture->getNumMipmaps() - 1u );

                        propName.a( "_width_with_lod" );    //uav0_width_with_lod
                        add( propName.c_str(), std::max( texture->getWidth() >> (Nui32)mipLevel, 1u ) );
                        propName.resize( texturePropSize );

                        propName.a( "_height_with_lod" );   //uav0_height_with_lod
                        add( propName.c_str(), std::max( texture->getHeight() >> (Nui32)mipLevel, 1u ) );
                        propName.resize( texturePropSize );
                    }
                    else if( itor->isBuffer() && itor->getBuffer().buffer )
                    {
                        propName.a( "_is_buffer" );             //uav0_is_buffer
                        add( propName.c_str(), 1 );
                        propName.resize( texturePropSize );
                    }

                    ++itor;
                }

                mBufferUnitMax = mBufferUnitList.size();
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setTextureStateMark(bool set)
    {
        mTextureStateMark = set;
        mParamValid = -1;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setCeilCount(Nui32 x, Nui32 y, Nui32 z)
    {
        if(mCeilCount[0] != x || mCeilCount[1] != y || mCeilCount[2] != z)
        {
            mCeilCount[0] = x;
            mCeilCount[1] = y;
            mCeilCount[2] = z;

            add(ComputeProperty::x, mCeilCount[0]);
            add(ComputeProperty::y, mCeilCount[1]);
            add(ComputeProperty::z, mCeilCount[2]);
            mParamValid = -1;
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setGroupCount(Nui32 x, Nui32 y, Nui32 z)
    {
        if( mGroupCount[0] != x || mGroupCount[1] != y || mGroupCount[2] != z )
        {
            mGroupCount[0] = x;
            mGroupCount[1] = y;
            mGroupCount[2] = z;

            add(ComputeProperty::x, mGroupCount[0]);
            add(ComputeProperty::y, mGroupCount[1]);
            add(ComputeProperty::z, mGroupCount[2]);
            mParamValid = -1;
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setGroupDivisor(GroupBaseType source, Nidx texIdx, uint8 divisorX, uint8 divisorY, uint8 divisorZ )
    {
        mGroupBaseType = source;

        mGroupsDivisor[0] = divisorX;
        mGroupsDivisor[1] = divisorY;
        mGroupsDivisor[2] = divisorZ;
        mGroupsDivisor[3] = texIdx;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::_calcGroup()
    {
        bool hasChanged = false;

        if( mGroupBaseType != GBT_Null )
        {
            const size_t maxSlots = mGroupBaseType == GBT_Sampler ? mTextureUnitList.size() : mBufferUnitList.size();
            Texture const * tex = 0;
            uint8 mipLevel = 0;
            if( mGroupsDivisor[3] < maxSlots )
            {
                if( mGroupBaseType == GBT_Sampler )
                {
                    tex = mTextureUnitList[mGroupsDivisor[3]].getTexture().mTexture;
                    mipLevel = mTextureUnitList[mGroupsDivisor[3]].getTexture().mipmapLevel;
                }
                else if( mBufferUnitList[mGroupsDivisor[3]].isTexture() )
                {
                    tex = mBufferUnitList[mGroupsDivisor[3]].getTexture().mTexture;
                    mipLevel = mBufferUnitList[mGroupsDivisor[3]].getTexture().mipmapLevel;
                }
            }

            if( tex )
            {
                Nui32 resolution[3];
                resolution[0] = std::max( tex->getWidth() >> mipLevel, 1u );
                resolution[1] = std::max( tex->getHeight() >> mipLevel, 1u );
                if( tex->getActualType() == Texture::Type::Type3D )
                    resolution[2] = std::max( tex->getVolume() >> mipLevel, 1u );
                else
                    resolution[2] = std::max( tex->getVolume(), 1u );

                for( int i = 0; i < 3; ++i )
                {
                    resolution[i] = (resolution[i] + mGroupsDivisor[i] - 1u) / mGroupsDivisor[i];

                    Nui32 numThreadGroups = (resolution[i] + mCeilCount[i] - 1u) / mCeilCount[i];
                    if( mGroupCount[i] != numThreadGroups )
                    {
                        mGroupCount[i] = numThreadGroups;
                        hasChanged = true;
                    }
                }
            }
            else
            {
                N_log(
                    "WARNING: No texture/uav bound to compute job '" + mName +
                    "' at slot " + StrConv::conv(mGroupsDivisor[3]) +
                    " while calculating number of thread groups based on texture");
            }
        }

        if( hasChanged )
            mParamValid = -1;

        if( !mGpuBufferList && !mBufferUnitList.empty() )
        {
            //UAV desc set is dirty. Time to calculate it again.
            GpuParamBuffer baseParams;
            baseParams.mUavs = mBufferUnitList;
            mGpuBufferList = mParent->getRender()->createBuffer( baseParams );
        }

        if( !mTextureList && !mTextureUnitList.empty() )
        {
            //Texture desc set is dirty. Time to calculate it again.

            ShaderChTexture baseParams;
            baseParams.mTextures = mTextureUnitList;
            baseParams.mShaderTypeTexCount[0] = mTextureUnitList.size();
            mTextureList = mParent->getRender()->createTexture( baseParams );
        }

        if( !mSampleList && !mSamplerUnitList.empty() )
        {
            //Sampler desc set is dirty. Time to calculate it again.

            TextureSample baseParams;
            baseParams.mSamplers = mSamplerUnitList;
            baseParams.mShaderTypeSamplerCount[0] = mSamplerUnitList.size();
            mSampleList = mParent->getRender()->createSampler( baseParams );
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setSegment(Nid id, const String & segment)
    {
        mSegmentList[id] = segment;

        int32 contentHash = 0;
        MurmurHash3_x86_32(segment.c_str(), segment.size(), IdString::Seed, &contentHash);
        add(PropertyID(id), contentHash );
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::removeSegment(Nid id)
    {
        SegmentList::iterator it = mSegmentList.find(id);
        if(it != mSegmentList.end())
        {
            remove(PropertyID(id));
            mSegmentList.erase(it);
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setStructBuffer(Nidx idx, StructBuffer * sb)
    {
        StructBufferList::iterator itor = std::lower_bound(mStructBufferList.begin(), mStructBufferList.end(), idx, StructBuffer);

        if(sb)
        {
            if(itor == mStructBufferList.end() || itor->getIdx() != idx)
                itor = mStructBufferList.insert(itor, sb);

            itor->setIdx(idx);
        }
        else
        {
            if(itor != mStructBufferList.end() && itor->getIdx() == idx)
                mStructBufferList.erase(itor);
        }
    }
    //-----------------------------------------------------------------------------------
    GpuParamValue * ComputeRaw::getParam(Nid key, bool create)
    {
        ParamValueList::type::iterator itor = mParamValueList.find(key);
        if(itor == mParamValueList.end())
        {
            if(create)
            {
                if( mParamValueList.find(key) == mParamValueList.end())
                    mParamValueList[key] = GpuParamValue();
                itor = mParamValueList.find(ke);
            }
        }

        return &itor->second;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setTextureCount( NCount cnt )
    {
        destroyAllSampler();
        destroyAllTexture();

        if( cnt < mSamplerUnitList.size() )
        {
            RenderSys * rsys = mParent->getRender();
            vectorL<const TextureSampleUnit *>::const_iterator itor = mSamplerUnitList.begin() + cnt;
            vectorL<const TextureSampleUnit *>::const_iterator end  = mSamplerUnitList.end();

            while( itor != end )
            {
                if( *itor )
                    rsys->destroyTexSample(*itor);
                ++itor;
            }

            removeListenerFromTextures(mTextureUnitList, cnt, mTextureUnitList.size());
        }

        mSamplerUnitList.resize(cnt);
        mTextureUnitList.resize(cnt);
        if( mTextureStateMark )
            mParamValid = -1;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::removeTexture(Nidx idx)
    {
        destroyAllSampler();
        destroyAllTexture();

        if(mSamplerUnitList[idx])
        {
            RenderSys * rsys = mParent->getRender();
            rsys->destroyTexSample(mSamplerUnitList[idx]);
        }
        removeListenerFromTextures(mTextureUnitList, idx, idx + 1u);

        mSamplerUnitList.erase(mSamplerUnitList.begin() + idx);
        mTextureUnitList.erase(mTextureUnitList.begin() + idx);
        if( mTextureStateMark )
            mParamValid = -1;
    }
    //-----------------------------------------------------------------------------------
    Texture * ComputeRaw::getTexture( Nidx idx ) const
    {
        if( mTextureUnitList[idx].isTexture() )
            return mTextureUnitList[idx].getTexture().mTexture;

        return 0;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setBufferCount( NCount cnt )
    {
        destroyAllStorage();

        if( cnt < mBufferUnitList.size() )
            removeListenerFromTextures( mBufferUnitList, cnt, mBufferUnitList.size() );

        mBufferUnitList.resize( cnt );
        if( mTextureStateMark )
            mParamValid = -1;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::removeBuffer( Nidx idx )
    {
        destroyAllStorage();
        removeListenerFromTextures( mBufferUnitList, idx, idx + 1u );
        mBufferUnitList.erase( mBufferUnitList.begin() + idx );
        if( mTextureStateMark )
            mParamValid = -1;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setTexture(Nidx idx, const ShaderChTexture::Slot & nslot, const TextureSampleUnit * tsu)
    {
        assert( idx < mTextureUnitList.size() );

        RenderSys * rsys = mParent->getRender();

        ShaderChTexture::Slot & slot = mTextureUnitList[idx];

        if( nslot.isBuffer() && slot != nslot )
        {
            if( mTextureStateMark && slot.mPType == ShaderChTexture::PT_Sampler && slot.getTexture().mTexture )
            {
                mParamValid = -1;
            }

            slot.mPType = ShaderChTexture::ST_Storage;
            ShaderChTexture::StorageUnit & bufferSlot = slot.getBuffer();
            bufferSlot = nslot.getBuffer();
            destroyAllTexture();    //Descriptor is dirty

            //Remove sampler
            if( mSamplerUnitList[idx] )
            {
                destroyAllSampler();    //Sampler descriptors are also dirty

                rsys->destroyTexSample( mSamplerUnitList[idx] );
                mSamplerUnitList[idx] = 0;
            }
        }
        else if( nslot.isTexture() && slot != nslot )
        {
            slot.mPType = ShaderChTexture::PT_Sampler;
            ShaderChTexture::SamplerUnit & texSlot = slot.getTexture();
            const ShaderChTexture::SamplerUnit & unitSlot = nslot.getTexture();

            if( mTextureStateMark && texSlot.mTexture != unitSlot.mTexture &&
                (!texSlot.mTexture || !unitSlot.mTexture || !texSlot.mTexture->isModelEqual( unitSlot.mTexture )) )
            {
                mParamValid = -1;
            }

            if( texSlot.mTexture )
                texSlot.mTexture->removeListener(this);
            if( unitSlot.mTexture )
                unitSlot.mTexture->addListener(this);

            texSlot = unitSlot;
            destroyAllTexture();    //Descriptor is dirty

            //Set explicit sampler, or create a default one if needed.
            if( tsu || (!mSamplerUnitList[idx] && texSlot.mTexture) )
            {
                const TextureSampleUnit * otsu = mSamplerUnitList[idx];
                if( tsu )
                    mSamplerUnitList[idx] = rsys->getTexSample( *tsu );
                else
                    mSamplerUnitList[idx] = rsys->getTexSample( TextureSampleUnit() );

                if( otsu != mSamplerUnitList[idx] )
                    destroyAllSampler();    //Sampler descriptors are also dirty

                if( otsu )
                    rsys->destroyTexSample( otsu );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setSampler(Nidx idx, const TextureSampleUnit * tsu)
    {
        assert( idx < mSamplerUnitList.size() );

        RenderSys * rsys = mParent->getRender();

        const TextureSampleUnit * otsu = mSamplerUnitList[idx];
        mSamplerUnitList[idx] = tsu;

        if(otsu != mSamplerUnitList[idx])
            destroyAllSampler();    //Sampler descriptors are dirty

        if(otsu)
            rsys->destroyTexSample(otsu);
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::setBuffer(Nidx idx, const GpuParamBufferUnit & unit)
    {
        assert( idx < mBufferUnitList.size() );

        GpuParamBufferUnit & slot = mBufferUnitList[idx];
        if( unit.mParamType == GpuParamBufferUnit::PT_Storage && slot != unit )
        {
            if( mTextureStateMark && slot.mParamType == GpuParamBufferUnit::PT_Sampler && slot.getTexture().mTexture )
            {
                mParamValid = -1;
            }

            slot.mParamType = GpuParamBufferUnit::PT_Storage;
            GpuParamBufferUnit::StorageUnit & bufferSlot = slot.getBuffer();

            bufferSlot = unit.getBuffer();
            destroyAllStorage();    //Descriptor is dirty
        }
        else if( unit.mParamType == GpuParamBufferUnit::PT_Sampler && slot != unit )
        {
            GpuParamBufferUnit::SamplerUnit & texSlot = slot.getTexture();
            GpuParamBufferUnit::SamplerUnit & unitSlot = unit.getTexture();
            
            if( mTextureStateMark && texSlot.mTexture != unitSlot.mTexture &&
                (!texSlot.mTexture || !unitSlot.mTexture || !texSlot.mTexture->isModelEqual( unitSlot.mTexture )) )
            {
                mParamValid = -1;
            }
            
            slot.mParamType = GpuParamBufferUnit::PT_Sampler;
            if(texSlot.mTexture)
                texSlot.mTexture->removeListener( this );
            if(unitSlot.mTexture)
                unitSlot.mTexture->addListener( this );

            texSlot = unitSlot;
            destroyAllStorage();    //Descriptor is dirty
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::clearTextureBuffer()
    {
        bool bChanged = false;
        ShaderChTexture::StorageUnit emptySlot();
        TextureUnitList::iterator itor = mTextureUnitList.begin();
        TextureUnitList::iterator end  = mTextureUnitList.end();

        while(itor != end)
        {
            if(itor->isBuffer() && itor->getBuffer().mBuffer)
            {
                itor->getBuffer() = emptySlot;
                bChanged = true;
            }
            ++itor;
        }

        if( bChanged )
            destroyAllTexture();
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::clearStroageBuffer()
    {
        bool bChanged = false;
        GpuParamBuffer::StorageUnit emptySlot();
        BufferUnitList::iterator itor = mBufferUnitList.begin();
        BufferUnitList::iterator end  = mBufferUnitList.end();

        while(itor != end)
        {
            if(itor->isBuffer() && itor->getBuffer().mBuffer)
            {
                itor->getBuffer() = emptySlot;
                bChanged = true;
            }
            ++itor;
        }

        if( bChanged )
            destroyAllStorage();
    }
    //-----------------------------------------------------------------------------------
    ComputeRaw * ComputeRaw::clone(const String & name)
    {
        ComputePattern * compute = static_cast<ComputePattern *>(mParent);
        ComputeRaw * re = compute->createComputeRaw(name, name, mSourceFilename, mSegmentList);

        re->copy(this);

        return re;
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::copy(ComputeRaw * src)
    {
        IdString originalName = mName;
        *this = *src;
        mName = originalName;

        RenderSys *rsys = src->getParent()->getRender();

        if( mTextureList )
            mTextureList = rsys->createTexture( src->mTextureList );
        if( mSampleList )
            mSampleList = rsys->createSampler( src->mSampleList );
        if( mGpuBufferList )
            mGpuBufferList = rsys->createBuffer( src->mGpuBufferList );

        vectorL<const TextureSampleUnit*>::const_iterator itor = mSamplerUnitList.begin();
        vectorL<const TextureSampleUnit*>::const_iterator end  = mSamplerUnitList.end();

        while( itor != end )
        {
            if( *itor )
                rsys->ref( *itor );
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputeRaw::process(const Item & item)
    {
        if( reason == Texture::Process::Deleted )
        {
            if( texture->isTexture() )
            {
                TextureUnitList::const_iterator itor = mTextureUnitList.begin();
                TextureUnitList::const_iterator end  = mTextureUnitList.end();

                while( itor != end )
                {
                    if( itor->isTexture() && itor->getTexture().mTexture == texture )
                    {
                        ShaderChTexture::SamplerUnit emptySlot = itor->getTexture();
                        emptySlot.mTexture = 0;
                        setTexture( itor - mTextureUnitList.begin(), emptySlot );
                    }
                    ++itor;
                }
            }

            if( texture->isUav() )
            {
                BufferUnitList::const_iterator itor = mBufferUnitList.begin();
                BufferUnitList::const_iterator end  = mBufferUnitList.end();

                while( itor != end )
                {
                    if( itor->isTexture() && itor->getTexture().mTexture == texture )
                    {
                        GpuParamBuffer::SamplerUnit emptySlot = itor->getTexture();
                        emptySlot.mTexture = 0;
                        setBuffer( itor - mBufferUnitList.begin(), emptySlot );
                    }
                    ++itor;
                }
            }
        }

        if( texture->isTexture() )
            destroyAllTexture();
        if( texture->isUav() )
            destroyAllStorage();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ComputePattern
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    const String BestD3DComputeShaderTargets[3] =
    {
        "cs_5_0", "cs_4_1", "cs_4_0"
    };
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ComputePattern
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ComputePattern::ComputePattern(ComputePatternType type, SysSyncParam * param, Nid id, const String & name) :
        GpuProgramCodeGen(type),
        mSysParam(param),
        mLanguageTarget(0),
        mID(id),
        mName(name)
    {
    }
    //-----------------------------------------------------------------------------------
    ComputePattern::~ComputePattern()
    {
        destroyAllComputeRaw();
    }
    //-----------------------------------------------------------------------------------
    void ComputePattern::setRender(RenderSys * rsys)
    {
        GpuProgramCodeGen::setRender(rsys);

        if(mRenderSys)
        {
            //Prefer glsl over glsles
            const String shaderProfiles[3] = { "hlsl", "glsles", "glsl" };
            const RenderFeature * rf = mRenderSys->getFeature();

            if(rf->isShaderSupport(shaderProfiles[0]))
                mShaderProfile = shaderProfiles[0];
            if(rf->isShaderSupport(shaderProfiles[1]))
                mShaderProfile = shaderProfiles[1];
            if(rf->isShaderSupport(shaderProfiles[2]))
                mShaderProfile = shaderProfiles[2];

            if( mShaderProfile == "hlsl" )
            {
                if(mLanguageTarget.empty() && rf->isShaderSupport(BestD3DComputeShaderTargets[0]))
                    mLanguageTarget = &BestD3DComputeShaderTargets[0];
                if(mLanguageTarget.empty() && rf->isShaderSupport(BestD3DComputeShaderTargets[1]))
                    mLanguageTarget = &BestD3DComputeShaderTargets[1];
                if(mLanguageTarget.empty() && rf->isShaderSupport(BestD3DComputeShaderTargets[2]))
                mLanguageTarget = &BestD3DComputeShaderTargets[2];
            }
        }
    }
    //-----------------------------------------------------------------------------------
    ComputeStateObject ComputePattern::compile(ComputeRaw * job, Nui32 finalHash)
    {
        //Assumes mPropertys is already set
        //mPropertys.clear();
        {
            //Add RenderSys-specific properties
            IdStringVec::const_iterator itor = mFeatureList.begin();
            IdStringVec::const_iterator end  = mFeatureList.end();

            while( itor != end )
                add( *itor++, 1 );
        }

        GpuProgram * shader;
        //Generate the shader

        //Collect pieces
        mSegmentList.clear();

        //Start with the pieces sent by the user
        mSegmentList = job->mSegmentList;

        const String sourceFilename = job->mSourceFilename + mShaderFileExt;

        //ResourceGroupManager & resourceGroupMgr = ResourceGroupManager::getSingleton();
        ScopeDataStream inFile(N_Only(ResourceScheme).open(sourceFilename));

        if( mShaderProfile == "glsl" ) //TODO: String comparision
        {
            add( N_PropertyRender::GL3Plus, mRenderSys->getShaderVersion() );
        }
        if( mShaderProfile == "glsles" ) //TODO: String comparision
            add( N_PropertyRender::GLES, 300 );

        add( N_PropertyRender::Syntax,  mShaderProfile.mHash );
        add( N_PropertyRender::Hlsl,    N_PropertyRender::Hlsl.mHash );
        add( N_PropertyRender::Glsl,    N_PropertyRender::Glsl.mHash );
        add( N_PropertyRender::Glsles,  N_PropertyRender::Glsles.mHash );
        add( N_PropertyRender::Metal,   N_PropertyRender::Metal.mHash );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        add( N_PropertyRender::iOS, 1 );
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        add( N_PropertyRender::macOS, 1 );
#endif
        add( N_PropertyRender::HighQuality, mQualityType );

        if( mFastShaderBuildHack )
            add( N_PropertyRender::FastShaderBuildHack, 1 );

        //Piece files
        ResourceSchemeManager & resourceGroupMgr = N_Only(ResourceScheme);

        StringList::const_iterator itor = job->mSegmentList.begin();
        StringList::const_iterator end  = job->mSegmentList.end();

        while( itor != end )
        {
            String filename = *itor;

            //If it has an explicit extension, only open it if it matches the current
            //render system's. If it doesn't, then we add it ourselves.
            String::size_type pos = filename.find_last_of( '.' );
            if( pos == String::npos ||
                (filename.compare( pos + 1, String::npos, mShaderFileExt ) != 0 &&
                 filename.compare( pos + 1, String::npos, "any" ) != 0 &&
                 filename.compare( pos + 1, String::npos, "metal" ) != 0 &&
                 filename.compare( pos + 1, String::npos, "glsl" ) != 0 &&
                 filename.compare( pos + 1, String::npos, "glsles" ) != 0 &&
                 filename.compare( pos + 1, String::npos, "hlsl" ) != 0) )
            {
                filename += mShaderFileExt;
            }

            ScopeDataStream tinFile(resourceGroupMgr.open( filename ));

            String inString;
            String outString;

            inString.resize(tinFile->size());
            tinFile->read(&inString[0], tinFile->size());

            genMath(inString, outString);
            while( outString.find( "@foreach" ) != String::npos )
            {
                genFor(outString, inString);
                inString.swap( outString );
            }
            genProperty(outString, inString);
            genUndef(inString, outString);
            genDefine(outString, inString);
            genPropertyMath(inString, outString);

            ++itor;
        }
        // end Piece files

        String inString;
        String outString;

        inString.resize( inFile->size() );
        inFile->read( &inString[0], inFile->size() );

        bool syntaxError = false;

        syntaxError |= genMath( inString, outString );
        while( !syntaxError && outString.find( "@foreach" ) != String::npos )
        {
            syntaxError |= genFor( outString, inString );
            inString.swap( outString );
        }
        syntaxError |= genProperty( outString, inString );
        syntaxError |= genUndef( inString, outString );
        while( !syntaxError  && (outString.find( "@piece" ) != String::npos || outString.find( "@insertpiece" ) != String::npos) )
        {
            syntaxError |= genDefine( outString, inString );
            syntaxError |= genRef( inString, outString );
        }
        syntaxError |= genPropertyMath( outString, inString );

        outString.swap( inString );

        if( syntaxError )
        {
            N_log( "There were HLMS syntax errors while parsing " + 
                StrConv::conv( finalHash ) + job->mSourceFilename + mShaderFileExt );
        }

        String debugFilenameOutput;

        if( mDebug )
        {
            debugFilenameOutput = mOutputPath + "./" + StrConv::conv( finalHash ) + job->mSourceFilename + mShaderFileExt;
            std::ofstream outFile( debugFilenameOutput.c_str(), std::ios::out | std::ios::binary );
            if( mDebugProperty )
                debugOutput( outFile );
            outFile.write( &outString[0], outString.size() );
        }

        //Don't create and compile if template requested not to
        if( !get( N_PropertyRender::DisableStage ) )
        {
            //Very similar to what the GpuProgramManager does with its microcode cache,
            //but we **need** to know if two Compute Shaders share the same source code.
            Hash hashVal;
            MurmurHash_128( outString.c_str(), outString.size(), IdString::Seed, &hashVal );

            CompiledList::const_iterator itor = mCompiledList.find( hashVal );
            if( itor != mCompiledList.end() )
            {
                shader = itor->second;
            }
            else
            {
                HighLevelGpuProgramManager * gpuProgramManager = HighLevelGpuProgramManager::getSingletonPtr();

                HighLevelGpuProgramPtr gp = gpuProgramManager->createProgram(
                            StrConv::conv(finalHash) + job->mSourceFilename,
                            ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
                            mShaderProfile, GPT_COMPUTE_PROGRAM);
                gp->setSource(outString, debugFilenameOutput);

                if(mLanguageTarget)
                {
                    //D3D-specific
                    gp->setParameter( "target", *mLanguageTarget );
                    gp->setParameter( "entry_point", "main" );
                }

                gp->setSkeletalAnimationIncluded( get( N_PropertyRender::Skeleton ) != 0 );
                gp->setMorphAnimationIncluded( false );
                gp->setPoseAnimationIncluded( get( N_PropertyRender::Pose ) != 0);
                gp->setVertexTextureFetchRequired( false );

                gp->load();

                shader = gp;

                mCompiledList[hashVal] = shader;
            }
        }

        //Reset the disable flag.
        add( N_PropertyRender::DisableStage, 0 );

        ComputeStateObject cso;
        cso.initialize();
        cso.mCS = shader;
        cso.mParam = shader->createParameters();
        cso.mCeilCount[0] = get(ComputeProperty::ThreadsPerGroupX);
        cso.mCeilCount[1] = get(ComputeProperty::ThreadsPerGroupY);
        cso.mCeilCount[2] = get(ComputeProperty::ThreadsPerGroupZ);
        cso.mGroupCount[0] = get(ComputeProperty::NumThreadGroupsX);
        cso.mGroupCount[1] = get(ComputeProperty::NumThreadGroupsY);
        cso.mGroupCount[2] = get(ComputeProperty::NumThreadGroupsZ);

        if( cso.mCeilCount[0] * cso.mCeilCount[1] * cso.mCeilCount[2] == 0u ||
            cso.mGroupCount[0] * cso.mGroupCount[1] * cso.mGroupCount[2] == 0u )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, job->getName() +
                         ": Shader or C++ must set threads_per_group_x, threads_per_group_y & "
                         "threads_per_group_z and num_thread_groups_x through num_thread_groups_z."
                         " Otherwise we can't run on Metal. Use @pset( threads_per_group_x, 512 );"
                         " or read the value using @value( threads_per_group_x ) if you've already"
                         " set it from C++ or the JSON material");
        }

        GpuParamValue * gpv = job->getParam( "default", false);
        if(gpv)
            gpv->update(cso.mParam, true);

        gpv = job->getParam( mShaderProfile, false);
        if(gpv)
            gpv->update(cso.mParam, true);

        mRenderSys->createCSO(&cso);

        return cso;
    }
    //-----------------------------------------------------------------------------------
    void ComputePattern::destroyComputeRaw(Nid id)
    {
        ComputeRawList::iterator itor = mComputeRawList.find(id);
        if( itor != mComputeRawList.end() )
        {
            N_delete itor->second.computeJob;
            mComputeRawList.erase( itor );
        }
    }
    //-----------------------------------------------------------------------------------
    void ComputePattern::destroyAllComputeRaw()
    {
        ComputeRawList::const_iterator itor = mComputeRawList.begin();
        ComputeRawList::const_iterator end  = mComputeRawList.end();

        while( itor != end )
        {
            N_delete itor->second.computeJob;
            ++itor;
        }

        mComputeRawList.clear();
    }
    //-----------------------------------------------------------------------------------
    void ComputePattern::clearCache()
    {
        ShaderCacheList::iterator itor = mShaderCacheList.begin();
        ShaderCacheList::iterator end  = mShaderCacheList.end();

        while( itor != end )
        {
            mRenderSys->destroyCSO( &itor->mCSO );
            itor->job->mParamValid = -1;
            ++itor;
        }

        GpuProgramCodeGen::clearCache();
        mCompiledList.clear();
        mShaderCacheList.clear();
    }
    //-----------------------------------------------------------------------------------
    void ComputePattern::execute(ComputeRaw * job)
    {
        job->_calcGroup();

        if(job->mParamValid >= mShaderCacheList.size())
        {
            //Potentially needs to recompile.
            job->_syncParam();

            ComputeStateCache psoCache;
            psoCache.job = job;
            //To perform the search, temporarily borrow the properties to avoid an allocation & a copy.
            psoCache.mPropertyList.swap( job->mPropertys );
            ShaderCacheList::const_iterator itor = std::find(mShaderCacheList.begin(), mShaderCacheList.end(), psoCache);
            if( itor == mShaderCacheList.end() )
            {
                //Needs to recompile.

                //Return back the borrowed properties and make
                //a hard copy for starting the compilation.
                psoCache.mPropertyList.swap( job->mPropertys );
                this->mPropertys = job->mPropertys;

                //Compile and add the PSO to the cache.
                psoCache.mCSO = compile( job, mShaderCacheList.size() );

                GpuParamValue * shaderParams = job->getParam( "default", false);
                if( shaderParams )
                    psoCache.paramsUpdateCounter = shaderParams->getStateMark();
                if( shaderParams )
                    psoCache.paramsProfileUpdateCounter = shaderParams->getStateMark();

                mShaderCacheList.push_back( psoCache );

                //The PSO in the cache doesn't have the properties. Make a hard copy.
                //We can use this->mPropertys as it may have been modified during
                //compilerShader by the template.
                mShaderCacheList.back().mPropertyList = job->mPropertys;

                job->mParamValid = mShaderCacheList.size() - 1u;
            }
            else
            {
                //It was already in the cache. Return back the borrowed
                //properties and set the proper index to the cache.
                psoCache.mPropertyList.swap( job->mPropertys );
                job->mParamValid = itor - mShaderCacheList.begin();
            }
        }

        ComputeStateCache & psoCache = mShaderCacheList[job->mParamValid];

        {
            //Update dirty parameters, if necessary
            GpuParamValue * shaderParams = job->getParam( "default", false);
            if( shaderParams && psoCache.paramsUpdateCounter != shaderParams->getStateMark() )
            {
                shaderParams->update( psoCache.mCSO.mParam, false );
                psoCache.paramsUpdateCounter = shaderParams->getStateMark();
            }

            shaderParams = job->getParam( mShaderProfile, false);
            if( shaderParams && psoCache.paramsProfileUpdateCounter != shaderParams->getStateMark() )
            {
                shaderParams->update( psoCache.mCSO.mParam, false );
                psoCache.paramsProfileUpdateCounter = shaderParams->getStateMark();
            }
        }

        mRenderSys->setCSO( &psoCache.mCSO );

        ComputeRaw::StructBufferList::const_iterator itConst = job->mStructBufferList.begin();
        ComputeRaw::StructBufferList::const_iterator enConst = job->mStructBufferList.end();

        while( itConst != enConst )
        {
            itConst->buffer->bindBufferCS(itConst->getIdx());
            ++itConst;
        }

        if( job->mTextureList )
            mRenderSys->_setTextureCS(0, job->mTextureList);
        if( job->mSampleList )
            mRenderSys->_setSamplerCS(0, job->mSampleList);
        if( job->mGpuBufferList )
            mRenderSys->_setBufferCS(0u, job->mGpuBufferList);

        mSysParam->setCurrentJob(job);
        //mSysParam->setCurrentCamera( camera );
        //mSysParam->setCurrentSpaceManager( sceneManager );

        GpuProgramParam * csParams = psoCache.mCSO.mParam;
        csParams->sync(mSysParam, GPV_ALL);
        mRenderSys->on(GPT_COMPUTE_PROGRAM, csParams, GPV_ALL);

        mRenderSys->_execute(psoCache.mCSO);
    }
    //----------------------------------------------------------------------------------
    void ComputePattern::setup(VFS * vfs, VFSList * vfslist)
    {
        GpuProgramCodeGen::setup(vfs, vfslist);

        ComputeRawList::const_iterator itor = mComputeRawList.begin();
        ComputeRawList::const_iterator end  = mComputeRawList.end();

        while(itor != end)
        {
            ComputeRaw * job = itor->second.computeJob;
            ParamValueList::type::iterator it = job->mParamValueList.begin();
            ParamValueList::type::iterator en = job->mParamValueList.end();

            while(it != en)
            {
                GpuParamValue & value = it->second;
                GpuParamValue::GpuParamUnitList::iterator itParam = value.getDefList().begin();
                GpuParamValue::GpuParamUnitList::iterator enParam = value.getDefList().end();

                while(itParam != enParam)
                {
                    itParam->mValidValue = false;
                    ++itParam;
                }

                value.setStateMark();
                ++it;
            }

            ++itor;
        }
    }
    //----------------------------------------------------------------------------------
    ComputeRaw * ComputePattern::createComputeRaw(Nid id, const String & name,
        const String & source, const SegmentList & slist)
    {
        N_assert1(mComputeRawList.find(id) == mComputeRawList.end());

        ComputeRaw * re = N_new ComputeRaw(id, this, source, slist);
        re->setName(name);
        mComputeRawList[id] = re;

        return re;
    }
    //----------------------------------------------------------------------------------
    ComputeRaw * ComputePattern::getComputeRaw(Nid id) const
    {
        ComputeRawList::const_iterator itor = mComputeRawList.find(id);
        if(itor != mComputeRawList.end())
            return itor->second.computeJob;

        return 0;
    }
    //----------------------------------------------------------------------------------
}