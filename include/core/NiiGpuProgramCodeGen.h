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

#ifndef _NII_GpuProgramCODEGEN_H_
#define _NII_GpuProgramCODEGEN_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"
#include "NiiRenderPattern.h"
#include "NiiCodeGen.h"

#define N_VertexShaderFile      _T("nii_vsf")
#define N_PixelShaderFile       _T("nii_psf")
#define N_GeometryShaderFile    _T("nii_gsf")
#define N_HullShaderFile        _T("nii_hsf")
#define N_DomainShaderFile      _T("nii_dsf")
#define N_ComputeShaderFile     _T("nii_csf")

namespace NII
{
    typedef map<Nid, String>::type SegmentList;
    typedef vector<SegmentList>::type SegmentGroupList
    
    typedef map<Nid, String *>::type SegmentRefList;
    typedef vector<SegmentRefList>::type SegmentRefGroupList

    /** GPU Shader编码合成器
    @version NIIEngine 6.0.0
    */
    class _EngineAPI GpuProgramCodeGen : public PropertyData, public CodeGen
    {
    public:
        enum QualityType
        {
            QT_Low = 0,
            QT_Mid = 1,
            QT_High = 2
        };
        
        struct Library
        {
            Library(VFS * vfs) : mVFS(vfs){}
            VFS * mVFS;
            StringGroupList mSegment;
        };

        typedef vector<Library>::type LibraryList;
        
        struct Expression
        {
            enum Type
            {
                ET_OR,          //||
                ET_AND,         //&&
                ET_LESS,        //<
                ET_LESSEQUAL,   //<=
                ET_EQUAL,       //==
                ET_NEQUAL,      //!=
                ET_GREATER,     //>
                ET_GREATEREQUAL,//>=
                ET_OBJECT,      //(obj)
                ET_VAR
            };

            Expression() : mResult(false), mNegated(false), mType(ET_VAR) {}

            bool isOperator() const { return mType >= ET_OR && mType <= ET_GREATEREQUAL; }
            void swap(Expression & o);

            std::vector<Expression> mChildren;
            String mValue;
            Type mType;
            int32 mResult;
            bool mNegated;
        };

        typedef std::vector<Expression> ExpressionList;

        class ExpressionAnaly
        {
        public:
            ExpressionAnaly(PropertyData * data, const String * ref, NCount _begin = 0, NCount _end = 0) :
                mData(data),
                mRef(ref),
                mStart(_begin)
            {
                assert(_begin <= _end);
                assert(_begin <= ref->size());
                assert(_end <= ref->size());
                if(_end == 0)
                    mEnd = ref->size();
                else
                    mEnd = _end;
            }

            ExpressionAnaly(PropertyData * data, const String * ref, String::const_iterator _begin) :
                mData(data),
                mRef(ref),
                mStart(_begin - ref->begin()),
                mEnd(ref->size()){}

            ExpressionAnaly & operator = (const ExpressionAnaly & o);

            NCount find(const Ntchar * value, NCount pos = 0) const
            {
                NCount re = mRef->find(value, mStart + pos);
                if( re >= mEnd )
                    re = String::npos;
                else if(re != String::npos)
                    re -= mStart;

                return re;
            }

            NCount findFirstOf(const Ntchar * value, NCount pos) const
            {
                NCount re = mRef->find_first_of(value, mStart + pos);
                if(re >= mEnd)
                    re = String::npos;
                else if(re != String::npos)
                    re -= mStart;

                return re;
            }

            NCount find(const String & value) const
            {
                NCount re = mRef->find(value, mStart);
                if(re >= mEnd)
                    re = String::npos;
                else if(re != String::npos)
                    re -= mStart;

                return re;
            }

            bool matchEqual(const Ntchar * o) const
            {
                const Ntchar * origStr = mRef->c_str() + mStart;
                ptrdiff_t length = mEnd - mStart;
                while( *origStr == *o && *origStr && --length )
                    ++origStr, ++o;

                return length == 0 && *origStr == *o;
            }

            bool startWith(const Ntchar * o) const
            {
                return strncmp(mRef->c_str() + mStart, o, strlen(o)) == 0;
            }

            bool startWith(const String & o) const
            {
                return strncmp(mRef->c_str() + mStart, o.c_str(), o.size()) == 0;
            }

            void setStart(NCount _begin)                { mStart = std::min(_begin, mRef->size()); }
            void setEnd(NCount _end)                    { mEnd = std::min(_end, mRef->size()); }
            NCount getStart() const                     { return mStart; }
            NCount getEnd() const                       { return mEnd; }
            NCount getSize() const                      { return mEnd - mStart; }
            String::const_iterator begin() const        { return mRef->begin() + mStart; }
            String::const_iterator end() const          { return mRef->begin() + mEnd; }
            const String * getRef() const               { return mRef; }
            PropertyData * getData() const              { return mData; }
            
            bool checkBegin(bool & error) const;
            bool checkArgs(StringList & outArgs);
            bool checkBlockEnd(bool & error, bool allowsElse = false);
            NCount checkEnd();
            NCount getLineCount();

            static NCount getLineCount(const String & buffer, NCount idx);
        private:
            PropertyData * mData;
            String const * mRef;
            NCount mStart;
            NCount mEnd;
        };
    public:
        GpuProgramCodeGen(Nid codeid);
        GpuProgramCodeGen(const String * name, Nid codeid);
        virtual ~GpuProgramCodeGen();

        /// @copydetails CodeGen::compound
        void compound(const String * in, String * out);
        
        /// @copydetails CodeGen::compound
        void compound(String * out);
        
        /**
        @version NIIEngine 6.0.0
        */
        const String & getShaderProfile() const             { return mShaderProfile; }
        
        /**
        @version NIIEngine 6.0.0
        */
        void setAutoDestroy(bool set)                       { mAutoDestroy = set; }
        
        /**
        @version NIIEngine 6.0.0
        */
        bool isAutoDestroy() const                          { return mAutoDestroy; }
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void setup(const VFSList & vfsl);
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void addLib(VFS * lib);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void removeLib(VFS * lib);
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void setRender(RenderSys * rsys);
        
        /** 
        @version NIIEngine 6.0.0
        */
        RenderSys * getRender() const                       { return mRenderSys; }
        
        /** 
        @version NIIEngine 6.0.0
        */
        VFS * getData() const                               { return mVFS; }
        
        /** 
        @version NIIEngine 6.0.0
        */
        const LibraryList & getLibraryList() const          { return mLibrary; }
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void setQuality(QualityType type);
        
        /**
        @version NIIEngine 6.0.0
        */
        QualityType getQuality() const                      { return mQualityType; }
        
        /**
        @version NIIEngine 6.0.0
        */
        void getHash(Nui32 & out[4]) const;
        
        /** 
        @version NIIEngine 6.0.0
        */
        void setDebug(bool enable, bool property, const String & path = N_StrBlank);

        /**
        @version NIIEngine 6.0.0
        */
        virtual void executeBegin(DrawCallGroup * dcg)      {}

        /**
        @version NIIEngine 6.0.0
        */
        virtual void executeEnd(DrawCallGroup * dcg)        {}
    protected:
        void debugOutput(Nofstream & out);

        void genSegment(VFS * vfs, const StringList & segments);
        bool genMath(const String & in, String & out);
        bool genFor(const String & in, String & out) const;
        bool genProperty(String & in, String & out) const;
        bool genPropertyMath(const String & in, String & out);
        bool genUndef(String & in, String & out);
        bool genDefine(const String & in, String & out);
        bool genRef(String & in, String & out) const;
    protected:
        VFS * mVFS;
        RenderSys * mRenderSys;
        QualityType mQualityType;
        LibraryList mLibrary;
        StringGroupList mSegment;
        SegmentList mSegmentList;
        String mShaderProfile;
        String mShaderFileExt;
        StringList mShaderTargets;
        IDList mFeatureList;
        String mOutputPath;
        bool mDebugProperty;
        bool mAutoDestroy;
        bool mDebug;
    };
}
#endif