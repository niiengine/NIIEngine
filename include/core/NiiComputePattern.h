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

#ifndef _NIICOMPUTEPATTERN_H_
#define _NIICOMPUTEPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPropertyData.h"
#include "NiiTexture.h"
#include "NiiShaderCh.h"

namespace NII
{
    class ComputeStateCache;
    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI ComputeRaw : public PropertyData, public Texture::Process, public GpuParamsAlloc
    {
        friend class ComputePattern;
    public:
        enum GroupBaseType
        {
            GBT_Null,
            GBT_Sampler,
            GBT_Storage
        };
    public:
        ComputeRaw(Nid name, ComputePattern * creator, const String & source, const StringList & segmentList);
        virtual ~ComputeRaw();
        
        /** 
        @version NIIEngine 5.0.0
        */
        void copy(ComputeRaw * src);

        /** 
        @version NIIEngine 5.0.0
        */
        Nid getID() const                               { return mID; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        void setName(const String & name)               { mName = name; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        const String & getName() const                  { return mName; }

        /** 
        @version NIIEngine 5.0.0
        */
        ComputePattern * getCreator() const             { return mCreator; }
        
        /** 手动设置状态
        @version NIIEngine 5.0.0
        */
        inline void setStateMark() const                { mParamValid = -1; }

        /** 
        @version NIIEngine 5.0.0
        */
        void setTextureStateMark(bool set);
        
        /** 
        @version NIIEngine 5.0.0
        */
        void setGroupDivisor(GroupBaseType source, Nidx texIdx, Nui8 divisorX, Nui8 divisorY, Nui8 divisorZ);

        /** 
        @version NIIEngine 5.0.0
        */
        void setCeilCount(Nui32 x, Nui32 y, Nui32 z);
        Nui32 getCeilX() const                          { return mCeilCount[0]; }
        Nui32 getCeilY() const                          { return mCeilCount[1]; }
        Nui32 getCeilZ() const                          { return mCeilCount[2]; }

        /** 
        @version NIIEngine 5.0.0
        */
        void setGroupCount(Nui32 x, Nui32 y, Nui32 z);
        Nui32 getGroupX() const                         { return mGroupCount[0]; }
        Nui32 getGroupY() const                         { return mGroupCount[1]; }
        Nui32 getGroupZ() const                         { return mGroupCount[2]; }

        /**
        @version NIIEngine 5.0.0
        */
        void setSegment(Nid id, const String & segment);

        /**
        @version NIIEngine 5.0.0
        */
        void removeSegment(Nid id);

        /**
        @version NIIEngine 5.0.0
        */
        GpuParamValue * getParam(Nid key, bool create = true);

        /**
        @version NIIEngine 5.0.0
        */
        void setTextureCount(NCount cnt);
        
        /**
        @version NIIEngine 5.0.0
        */
        void removeTexture(Nidx idx);
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getTextureCount() const                  { return mTextureUnitList.size(); }

        /**
        @version NIIEngine 5.0.0
        */
        TextureGpu * getTexture(Nidx idx) const;

        /**
        @version NIIEngine 5.0.0
        */
        void setBufferCount(NCount cnt);
        
        /**
        @version NIIEngine 5.0.0
        */
        void removeBuffer(Nidx idx);
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getBufferCount() const                   { return mBufferUnitList.size(); }

        /**
        @version NIIEngine 5.0.0
        */
        void setStructBuffer(Nidx idx, StructBuffer * buf);

        /** 
        @version NIIEngine 5.0.0
        */
        void setTexture(Nidx idx, const ShaderChTexture::Slot & nslot, const TextureSampleUnit * tsu = 0);

        /** 
        @version NIIEngine 5.0.0
        */
        void setSampler(Nidx idx, const TextureSampleUnit * tsu);

        /** 
        @version NIIEngine 5.0.0
        */
        void setBuffer(Nidx idx, const GpuParamBufferUnit & unit);
        
        /**
        @version NIIEngine 5.0.0 adv
        */
        StructBuffer * getStructBuffer(Nidx idx) const  { return mStructBufferList[idx]; }
        
        /**
        @version NIIEngine 5.0.0 adv
        */
        TextureSampleUnit * getSampler(Nidx idx) const  { return mSamplerUnitList[idx]; }
        
        /**
        @version NIIEngine 5.0.0 adv
        */
        GpuParamBufferUnit & getBuffer(Nidx idx) const  { return mBufferUnitList[idx]; }

        /**
        @version NIIEngine 5.0.0
        */
        void clearTextureBuffer();

        /**
        @version NIIEngine 5.0.0
        */
        void clearStroageBuffer();

        ComputeRaw * clone(const String & cloneName);

        virtual void process(const Item & item);
    protected:
        void _syncParam();
        
        void _calcGroup();
        
        void destroyAllSampler();
        
        void destroyAllTexture();
        
        void destroyAllStorage();

        typedef vector<StructBuffer *>::type StructBufferList;
        typedef vector<ShaderChTexture::Slot>::type TextureUnitList;
        typedef vector<GpuParamBufferUnit>::type BufferUnitList;
        typedef vector<const TextureSampleUnit *>::type SamplerUnitList;
        typedef map<Nid, GpuParamValue>::type ParamValueList;
    protected:
        ComputePattern * mCreator;
        Nid mID;
        String mName;

        String mSourceFilename;
        StringList mSegmentList;

        Nui32 mCeilCount[3];
        Nui32 mGroupCount[3];

        GroupBaseType mGroupBaseType;
        Nui8 mGroupsDivisor[4];

        ParamValueList mParamValueList;
        StructBufferList mStructBufferList;
        SamplerUnitList mSamplerUnitList;
        TextureUnitList mTextureUnitList;
        BufferUnitList mBufferUnitList;

        ShaderChTexture const * mTextureList;
        TextureSample const * mSampleList;
        GpuParamBuffer const * mGpuBufferList;
        SegmentList mSegmentList;
        NCount mTextureUnitMax;
        NCount mBufferUnitMax;
        NCount mParamValid;
        bool mTextureStateMark;
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI ComputePattern : public GpuProgramCodeGen
    {
    public:
        ComputePattern(ComputePatternType type, SysSyncParam * param);
        virtual ~ComputePattern();
        
        /**
        @version NIIEngine 5.0.0
        */
        ComputePatternType getType() const      { return mType; }

        /**
        @version NIIEngine 5.0.0
        */
        virtual void setup(VFS * vfs, VFSList * vfslist = 0);

        /** 
        @version NIIEngine 5.0.0
        */
        ComputeRaw * createComputeRaw(Nid id, const String & name, const String & source, const StringList & segmentList);

        /**
        @version NIIEngine 5.0.0
        */
        ComputeRaw * getComputeRaw(Nid id) const;

        /**
        @version NIIEngine 5.0.0
        */
        void destroyComputeRaw(Nid id);

        /**
        @version NIIEngine 5.0.0
        */
        void destroyAllComputeRaw();

        /**
        @version NIIEngine 5.0.0
        */
        virtual void clearCache();
        
        /**
        @version NIIEngine 5.0.0
        */
        SysSyncParam * getParam() const         { return mSysParam; }
        
        /**
        @version NIIEngine 5.0.0
        */
        void execute(ComputeRaw * job);

        /// @copydetails RenderPattern::setRender
        virtual void setRender(RenderSys * rsys);
    protected:
        ComputeStateObject compile(ComputeRaw * job, Nui32 hash);
    protected:
        typedef std::map<Nid, ComputeRaw *> ComputeRawList;
        typedef vector<ComputeStateCache *>::type ShaderCacheList;
        typedef map<HashKey, GpuProgram *>::type CompiledList;
    protected:
        SysSyncParam * mSysParam;
        String mLanguageTarget;
        ComputePatternType mType;
        ComputeRawList mComputeRawList;
        CompiledList mCompiledList;
        ShaderCacheList mShaderCacheList;
    };
}
#endif