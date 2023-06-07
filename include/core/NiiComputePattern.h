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
    
    enum ComputePatternType
    {
        CPT_Normal  = N_CodeGen_Compute + 1,
        
        CPT_Custom1 = N_CodeGen_Compute + 2,
        CPT_Custom2 = N_CodeGen_Compute + 3,
        CPT_Custom3 = N_CodeGen_Compute + 4,
        CPT_Custom4 = N_CodeGen_Compute + 5,
        CPT_Custom5 = N_CodeGen_Compute + 6,
        CPT_Custom6 = N_CodeGen_Compute + 7,
        CPT_Custom7 = N_CodeGen_Compute + 8,
        CPT_Custom8 = N_CodeGen_Compute + 9,
        CPT_Custom9 = N_CodeGen_Compute + 10,
        CPT_Custom10= N_CodeGen_Compute + 11,
        
        CPT_Count   = N_CodeGen_Compute + 12
    };
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
        ComputeRaw(Nid name, ComputePattern * parent, const String & source, const SegmentList & segmentList);
        virtual ~ComputeRaw();
        
        /** 
        @version NIIEngine 5.0.0
        */
        void copy(ComputeRaw * src);

        /** 
        @version NIIEngine 5.0.0
        */
        inline Nid getID() const                               { return mID; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        inline void setName(const String & name)               { mName = name; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        inline const String & getName() const                  { return mName; }

        /** 
        @version NIIEngine 5.0.0
        */
        inline ComputePattern * getParent() const              { return mParent; }
        
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
        inline Nui32 getCeilX() const                          { return mCeilCount[0]; }
        inline Nui32 getCeilY() const                          { return mCeilCount[1]; }
        inline Nui32 getCeilZ() const                          { return mCeilCount[2]; }

        /** 
        @version NIIEngine 5.0.0
        */
        void setGroupCount(Nui32 x, Nui32 y, Nui32 z);
        inline Nui32 getGroupX() const                         { return mGroupCount[0]; }
        inline Nui32 getGroupY() const                         { return mGroupCount[1]; }
        inline Nui32 getGroupZ() const                         { return mGroupCount[2]; }

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
        inline NCount getTextureCount() const           { return mTextureUnitList.size(); }

        /**
        @version NIIEngine 5.0.0
        */
        Texture * getTexture(Nidx idx) const;

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
        inline NCount getBufferCount() const                   { return mBufferUnitList.size(); }

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
        inline StructBuffer * getStructBuffer(Nidx idx) const  { return mStructBufferList[idx]; }
        
        /**
        @version NIIEngine 5.0.0 adv
        */
        inline TextureSampleUnit * getSampler(Nidx idx) const  { return mSamplerUnitList[idx]; }
        
        /**
        @version NIIEngine 5.0.0 adv
        */
        inline GpuParamBufferUnit & getBuffer(Nidx idx) const  { return mBufferUnitList[idx]; }

        /**
        @version NIIEngine 5.0.0
        */
        void clearTextureBuffer();

        /**
        @version NIIEngine 5.0.0
        */
        void clearStroageBuffer();

        /**
        @version NIIEngine 5.0.0
        */
        ComputeRaw * clone(const String & name);

        /**
        @version NIIEngine 5.0.0
        */
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
        ComputePattern * mParent;
        Nid mID;
        String mName;

        String mSourceFilename;
        SegmentList mSegmentList;
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
        ComputePattern(ComputePatternType type, SysSyncParam * param, Nid id, const String & name = N_StrBlank);
        virtual ~ComputePattern();
        
        /**
        @version NIIEngine 5.0.0
        */
        inline ComputePatternType getType() const      { return mCodeID; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline Nid getID() const                       { return mID; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline void setName(const String & str)        { mName = str;}

        /**
        @version NIIEngine 5.0.0
        */
        inline const String & getName() const          { return mName; }

        /**
        @version NIIEngine 5.0.0
        */
        virtual void setup(VFS * vfs, VFSList * vfslist = 0);

        /** 
        @version NIIEngine 5.0.0
        */
        ComputeRaw * createComputeRaw(Nid id, const String & name, const String & source, const SegmentList & segmentList);

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
        inline SysSyncParam * getParam() const         { return mSysParam; }
        
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
        String mName;
        SysSyncParam * mSysParam;
        String mLanguageTarget;
        ComputeRawList mComputeRawList;
        CompiledList mCompiledList;
        ShaderCacheList mShaderCacheList;
    };
}
#endif