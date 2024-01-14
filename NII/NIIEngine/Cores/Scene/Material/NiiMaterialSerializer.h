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

#ifndef _NII_MaterialSerializer_H_
#define _NII_MaterialSerializer_H_

#include "NiiPreInclude.h"
#include "NiiMaterial.h"
#include "NiiGpuProgram.h"
#include "NiiString.h"

namespace NII
{
    /// 确定材质项的枚举
    enum MaterialScriptSection
    {
        MSS_NONE,
        MSS_MATERIAL,
        MSS_TECHNIQUE,
        MSS_PASS,
        MSS_TEXTUREUNIT,
        MSS_PROGRAM_REF,
        MSS_PROGRAM,
        MSS_DEFAULT_PARAMETERS,
        MSS_TEXTURESOURCE
    };

    /// 结构保存一个主着色程序定义
    struct MaterialScriptProgramDefinition
    {
        ResourceID name;
        GpuProgram::ShaderType progType;
        ShaderLanguage language;
        String source;
        ShaderLanguage syntax;
        bool supportsSkeletalAnimation;
        bool supportsMorphAnimation;
        Nui16 supportsPoseAnimation; // number of simultaneous poses supported
        bool usesVertexTextureFetch;
        vector<std::pair<String, String> >::type customParameters;
    };

    /// 结构当解析时,保存脚本内容
    struct MaterialScriptContext
    {
        MaterialScriptSection section;
        GroupID groupName;
        Material * material;
        ShaderFusion * technique;
        ShaderCh * pass;
        ShaderChTextureUnit * textureUnit;
        GpuProgram * program; // used when referencing a program, not when defining it
        bool isVertexProgramShadowCaster; // when referencing, are we in context of shadow caster
        bool isFragmentProgramShadowCaster; // when referencing, are we in context of shadow caster
        bool isVertexProgramShadowReceiver; // when referencing, are we in context of shadow caster
        bool isFragmentProgramShadowReceiver; // when referencing, are we in context of shadow caster
        GpuProgramParam * programParams;
        Nui16 numAnimationParametrics;
        MaterialScriptProgramDefinition * programDef; // this is used while defining a program

        Nidx techLev,    //Keep track of what tech, pass, and state level we are in
            passLev,
            stateLev;
        StringList defaultParamLines;

        // Error reporting state
        NCount lineNo;
        String filename;
        ComplexArgList textureAliases;
    };
    /// 函数定义,为材质属性分析器;返回确定下一行应该是什么的值 {
    typedef bool (*ATTRIBUTE_PARSER)(String & params, MaterialScriptContext & context);

    /// 序列化材质to/from一个.material脚本的类.
    class _EngineAPI MaterialSerializer : public Serializer
    {
    public:
        /// 材质序列化事件
        enum SerializeEvent
        {
            MSE_PRE_WRITE,
            MSE_WRITE_BEGIN,
            MSE_WRITE_END,
            MSE_POST_WRITE,
        };

        /// 类,充许监听各种各样材质序列化过程.子-类扩展在材质任何部分的属性集
        class Listener
        {
        public:
            virtual ~Listener() {}

            /** 当材质项事件上升时调用
            @param[in] ser The MaterialSerializer instance that writes the given material.
            @param[in] stage The current section writing stage.
            @param[in] skip May set to true by sub-class instances in order to skip the following section write.
            This parameter relevant only when stage equals MSE_PRE_WRITE.
            @param[in] mat The material that is being written.
            */
            virtual void materialEventRaised(MaterialSerializer * ser,
                SerializeEvent event, bool & skip, const Material * mat)
            { (void)ser; (void)event; (void)skip; (void)mat; }

            /** 当技术项事件上升时调用
            @param[in] ser The MaterialSerializer instance that writes the given material.
            @param[in] stage The current section writing stage.
            @param[in] skip May set to true by sub-class instances in order to skip the following section write.
            This parameter relevant only when stage equals MSE_PRE_WRITE.
            @param[in] tech The technique that is being written.
            */
            virtual void techniqueEventRaised(MaterialSerializer * ser,
                SerializeEvent event, bool & skip, const ShaderFusion * tech)
            { (void)ser; (void)event; (void)skip; (void)tech; }

            /** 当通道项事件上升时调用
            @param[in] ser The MaterialSerializer instance that writes the given material.
            @param[in] stage The current section writing stage.
            @param[in] skip May set to true by sub-class instances in order to skip the following section write.
            This parameter relevant only when stage equals MSE_PRE_WRITE.
            @param[in] pass The pass that is being written.
            */
            virtual void passEventRaised(MaterialSerializer * ser, SerializeEvent event,
                bool & skip, const ShaderCh * pass)
            { (void)ser; (void)event; (void)skip; (void)pass; }

            /** 当GPU程序引用项事件上升时调用
            @param[in] ser The MaterialSerializer instance that writes the given material.
            @param[in] stage The current section writing stage.
            @param[in] skip May set to true by sub-class instances in order to skip the following section write.
            This parameter relevant only when stage equals MSE_PRE_WRITE.
            @param[in] attrib The GPU program reference description (vertex_program_ref, fragment_program_ref, etc).
            @param[in] program The program being written.
            @param[in] params The program parameters.
            @param[in] defaultParams The default program parameters.
            */
            void gpuProgramRefEventRaised(MaterialSerializer * ser, SerializeEvent event,
                bool & skip, const String & attrib, const GpuProgram * program,
                    const GpuProgramParam * params, GpuProgramParam * defaultParams)
            {
                (void)ser;
                (void)event;
                (void)skip;
                (void)attrib;
                (void)program;
                (void)params;
                (void)defaultParams;
            }

            /** 当纹理单元状态项事件上升时调用
            @param[in] ser The MaterialSerializer instance that writes the given material.
            @param[in] stage The current section writing stage.
            @param[in] skip May set to true by sub-class instances in order to skip the following section write.
            This parameter relevant only when stage equals MSE_PRE_WRITE.
            @param[in] textureUnit The texture unit state that is being written.
            */
            virtual void textureUnitStateEventRaised(MaterialSerializer * ser,
                SerializeEvent event, bool & skip, const ShaderChTextureUnit * textureUnit)
            {
                (void)ser;
                (void)event;
                (void)skip;
                (void)textureUnit;
            }
        };

        typedef map<String, ATTRIBUTE_PARSER>::type AttribParserList;
   public:
        MaterialSerializer();
        virtual ~MaterialSerializer() {};

        /** 为导出列队,一个在-内存的材质到内部缓存
        @param[in] pMat Material pointer
        @param[in] clearQueued If true, any materials already queued will be removed
        @param[in] exportDefaults If true, attributes which are defaulted will be
            included in the script exported, otherwise they will be omitted
        @param[in] materialName Allow exporting the given material under a different name.
            In case of empty string the original material name will be used.
        */
        void queueForExport(const Material * pMat, bool clearQueued = false,
            bool exportDefaults = false, const String & materialName = _T(""));

        /** 导出队列材质(s)到一个命名材质脚本文件
        @param[in] filename the file name of the material script to be exported
        @param[in] includeProgDef If true, vertex program and fragment program
            definitions will be written at the top of the material script
        @param[in] programFilename the file name of the vertex / fragment program
            script to be exported. This is only used if there are program definitions
            to be exported and includeProgDef is false
            when calling queueForExport.
        */
        void exportQueued(const String & filename, const bool includeProgDef = false,
            const String & programFilename = _T(""));

        /** 导出一个单一 在-内存 材质到 命名的材质脚本文件
        @param[in] exportDefaults if true then exports all values including defaults
        @param[in] includeProgDef if true includes Gpu shader program definitions in the
            export material script otherwise if false then program definitions will
            be exported to a separate file with name programFilename if
            programFilename is not empty
        @param[in] programFilename the file name of the vertex / fragment program
            script to be exported. This is only used if includeProgDef is false.
        @param[in] materialName Allow exporting the given material under a different name.
            In case of empty string the original material name will be used.
        */
        void exportMaterial(const Material *pMat, const String & filename,
            bool exportDefaults = false, const bool includeProgDef = false,
                const String& programFilename = _T(""), const String& materialName = _T(""));

        /// 返回一个字符串描述解析过的材料(s)
        const String & getQueuedAsString() const;

        /// 清除内部缓存
        void clearQueue();

        /// 解析一个 材质脚本文件到输入的流
        void parseScript(DataStream * stream, void * out, GroupID gid);

        /** 注册一个监听到这个串行器
        @see MaterialSerializer::Listener
        */
        void add(Listener * listener);

        /** 从这个串行器里移去一个监听
        @see MaterialSerializer::Listener
        */
        void remove(Listener * listener);

        void beginSection(Nui16 level, const bool useMainBuffer = true)
        {
            String & buffer = (useMainBuffer ? mBuffer : mGpuProgramBuffer);
            buffer += _T("\n");
            for (Nui16 i = 0; i < level; ++i)
            {
                buffer += _T("\t");
            }
            buffer += _T("{");
        }

        void endSection(Nui16 level, const bool useMainBuffer = true)
        {
            String& buffer = (useMainBuffer ? mBuffer : mGpuProgramBuffer);
            buffer += _T("\n");
            for (Nui16 i = 0; i < level; ++i)
            {
                buffer += _T("\t");
            }
            buffer += _T("}");
        }

        void writeAttribute(Nui16 level, const String& att, const bool useMainBuffer = true)
        {
            String& buffer = (useMainBuffer ? mBuffer : mGpuProgramBuffer);
            buffer += _T("\n");
            for (Nui16 i = 0; i < level; ++i)
            {
                buffer += _T("\t");
            }
            buffer += att;
        }

        void writeValue(const String & val, const bool useMainBuffer = true)
        {
            String & buffer = (useMainBuffer ? mBuffer : mGpuProgramBuffer);
            buffer += (_T(" ") + val);
        }

        WString quoteWord(const String & val)
        {
            if (val.find_first_of(_T(" \t")) != String::npos)
                return (_T("\"") + val + _T("\""));
            else return val;
        }

        void writeComment(Nui16 level, const String& comment, const bool useMainBuffer = true)
        {
            String & buffer = (useMainBuffer ? mBuffer : mGpuProgramBuffer);
            buffer += _T("\n");
            for (Nui16 i = 0; i < level; ++i)
            {
                buffer += _T("\t");
            }
            buffer += _T("// ") + comment;
        }
    protected:
        /** 内部方法,解析一个材质
        @return true if it expects the next line to be a {
        */
        bool parseScriptLine(String & line);

        /// 内部方法,寻找&调用一个属性解析器
        bool invokeParser(String & line, AttribParserList & parsers);

        /// 内部方法,保存一个已经建立的着色程序定义
        void finishProgramDefinition(void);

        void writeMaterial(const Material * pMat, const String & materialName = _T(""));
        void writeTechnique(const ShaderFusion * pTech);
        void writePass(const ShaderCh * pPass);
        void writeVertexProgramRef(const ShaderCh * pPass);
        void writeShadowCasterVertexProgramRef(const ShaderCh * pPass);
        void writeShadowCasterFragmentProgramRef(const ShaderCh * pPass);
        void writeShadowReceiverVertexProgramRef(const ShaderCh * pPass);
        void writeShadowReceiverFragmentProgramRef(const ShaderCh * pPass);
        void writeFragmentProgramRef(const ShaderCh * pPass);
        void writeGpuProgramRef(const String & attrib, const GpuProgram * program, const GpuProgramParam * params);
        void writeGpuPrograms(void);
        void writeGPUProgramParameters(const GpuProgramParam * params, GpuProgramParam* defaultParams,
            const Nui16 level = 4, bool useMainBuffer = true);
        void writeNamedGpuProgramParameters(const GpuProgramParam * params, GpuProgramParam* defaultParams,
            const Nui16 level = 4, bool useMainBuffer = true);
        void writeLowLevelGpuProgramParameters(const GpuProgramParam * params, GpuProgramParam* defaultParams,
            const Nui16 level = 4, bool useMainBuffer = true);
        void writeGpuProgramParameter(
            const String & commandName, const String & identifier,
            const GpuParamBlock * autoEntry,
            const GpuParamBlock * defaultAutoEntry,
            GpuDataType dtype, NCount mindex, NCount physicalSize,
            const GpuProgramParam *  params, GpuProgramParam * defaultParams,
            const Nui16 level, const bool useMainBuffer);
        void writeTextureUnit(const ShaderChTextureUnit *pTex, const ShaderCh * ch);
        void writeSceneBlendFactor(ColourFactor c_src, ColourFactor c_dest,
            ColourFactor a_src, ColourFactor a_dest);
        void writeSceneBlendFactor(ColourFactor sbf_src, ColourFactor sbf_dest);
        void writeSceneBlendFactor(ColourFactor sbf);
        void writeCompareFunction(const CmpMode cf);
        void writeColourValue(const Colour & colour, bool writeAlpha = false);
        void writeLayerBlendOperationEx(TextureBlendOp op);
        void writeLayerBlendSource(TextureBlendSource lbs);

        typedef multimap<ShaderChTextureUnit::TextureTransformType, ShaderChTextureUnit::TextureEffect>::type EffectMap;

        void writeRotationEffect(const ShaderChTextureUnit::TextureEffect& effect, const ShaderChTextureUnit *pTex);
        void writeTransformEffect(const ShaderChTextureUnit::TextureEffect& effect, const ShaderChTextureUnit *pTex);
        void writeScrollEffect(const ShaderChTextureUnit::TextureEffect& effect, const ShaderChTextureUnit *pTex);
        void writeEnvironmentMapEffect( const ShaderChTextureUnit *pTex);

        String convertFiltering(TextureFilterOP fo);

        /** 内部方法,调用注册的监听器回调
        @see Listener::materialEventRaised.
        */
        void fireMaterialEvent(SerializeEvent event, bool & skip, const Material * mat);

        /** 内部方法,调用注册的监听器回调
        @see Listener::techniqueEventRaised.
        */
        void fireTechniqueEvent(SerializeEvent event, bool & skip, const ShaderFusion * tech);

        /** 内部方法,调用注册的监听器回调
        @see Listener::passEventRaised.
        */
        void firePassEvent(SerializeEvent event, bool & skip, const ShaderCh * pass);

        /** 内部方法,调用注册的监听器回调
        @see Listener::gpuProgramRefEventRaised.
        */
        void fireGpuProgramRefEvent(SerializeEvent event, bool & skip,
            const String & attrib, const GpuProgram * program,
                const GpuProgramParam * params,
                    GpuProgramParam * defaultParams);

        /** 内部方法,调用注册的监听器回调
        @see Listener::textureUnitStateEventRaised.
        */
        void fireTextureUnitStateEvent(SerializeEvent event, bool & skip, const ShaderChTextureUnit * textureUnit);
    protected:
        MaterialScriptContext mScriptContext;

        /// Parsers for the root of the material script
        AttribParserList mRootAttribParsers;
        /// Parsers for the material section of a script
        AttribParserList mMaterialAttribParsers;
        /// Parsers for the technique section of a script
        AttribParserList mTechniqueAttribParsers;
        /// Parsers for the pass section of a script
        AttribParserList mPassAttribParsers;
        /// Parsers for the texture unit section of a script
        AttribParserList mTextureUnitAttribParsers;
        /// Parsers for the program reference section of a script
        AttribParserList mProgramRefAttribParsers;
        /// Parsers for the program definition section of a script
        AttribParserList mProgramAttribParsers;
        /// Parsers for the program definition section of a script
        AttribParserList mProgramDefaultParamAttribParsers;

        /// Listeners list of this Serializer.
        typedef vector<Listener *>::type Listeners;
        typedef Listeners::iterator ListenerListIterator;
        typedef Listeners::const_iterator ListenerListConstIterator;
        Listeners mListeners;
    private:
        String mBuffer;
        String mGpuProgramBuffer;
        typedef set<ResourceID>::type GpuProgramDefinitionContainer;
        typedef GpuProgramDefinitionContainer::iterator GpuProgramDefIterator;
        GpuProgramDefinitionContainer mGpuProgramDefinitionContainer;
        bool mDefaults;
    };
}
#endif