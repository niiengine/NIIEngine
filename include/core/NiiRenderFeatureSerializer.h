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

#ifndef _NII_RenderSysFeatureSerializer_H_
#define _NII_RenderSysFeatureSerializer_H_

#include "NiiPreInclude.h"
#include "NiiRenderSys.h"
#include "NiiDataStream.h"

namespace NII
{
    /** RenderFeature 解析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderFeatureSerializer : public RenderSysAlloc
    {
    public:
        RenderFeatureSerializer();
        ~RenderFeatureSerializer();

        /** 读取数据流
        @version NIIEngine 3.0.0
        */
        void read(DataStream * stream);

        /** 写入到文件
        @param[out] filename 文件
        @version NIIEngine 3.0.0
        */
        void write(const RenderFeature * caps, const String & name, const String & filename);

        /** 写入到字符串
        @version NIIEngine 3.0.0
        */
        String write(const RenderFeature * caps, const String & name);
    protected:
        enum DataType
        {
            DT_Unknow = 0,
            DT_Bool,
            DT_Int16,
            DT_Int32,
            DT_Float,
            DT_String,
            DT_Feature,
            DT_GpuInfo,
            DT_ShaderLang
        };

        typedef void (RenderFeature::*SetBoolMethod)(bool);
        typedef void (RenderFeature::*SetInt16Method)(Nui16);
        typedef void (RenderFeature::*SetInt32Method)(Nui32);
        typedef void (RenderFeature::*SetFloatMethod)(NIIf);
        typedef void (RenderFeature::*SetStringMethod)(const String &);
        typedef map<String, DataType>::type KeyTypeList;
        typedef map<String, SetStringMethod>::type StringTableMap;
        typedef map<String, SetInt16Method>::type Int16TableMap;
        typedef map<String, SetInt32Method>::type Int32TableMap;
        typedef map<String, SetBoolMethod>::type BoolTableMap;
        typedef map<String, SetFloatMethod>::type FloatTableMap;
        typedef map<String, GpuFeature>::type FeatureMap;
        typedef vector<std::pair<String, NIIi> >::type CollectList;
    protected:
        /** 添加关键字到关键类型的映射
        @version NIIEngine 3.0.0
        */
        void addKeyDataType(const String & key, DataType type);

        /** 通过关键字获取调用类型
        @version NIIEngine 3.0.0
        */
        DataType getKeyDataType(const String & key) const;

        /** 添加枚举映射
        @version NIIEngine 3.0.0
        */
        void add(const String & name, GpuFeature cap);

        /** 绑定关键字到调用方法(String级别)
        @version NIIEngine 3.0.0
        */
        void add(const String & key, SetStringMethod method);

        /** 绑定关键字到调用方法(Int16级别)
        @version NIIEngine 3.0.0
        */
        void add(const String & key, SetInt16Method method);

        /** 绑定关键字到调用方法(Int32级别)
        @version NIIEngine 3.0.0
        */
        void add(const String & key, SetInt32Method method);

        /** 绑定关键字到调用方法(Bool级别)
        @version NIIEngine 3.0.0
        */
        void add(const String & key, SetBoolMethod method);

        /** 绑定关键字到调用方法(Float级别)
        @version NIIEngine 3.0.0
        */
        void add(const String & key, SetFloatMethod method);

        /** 调用设置 (enum GpuFeature)是否用
        @version NIIEngine 3.0.0
        */
        void setEnable(const String & name, bool val);

        /** 由关键字调用具体的方法(Int16级别)
        @version NIIEngine 3.0.0
        */
        void exec(const String & key, Nui16 val);

        /** 由关键字调用具体的方法(Int32级别)
        @version NIIEngine 3.0.0
        */
        void exec(const String & key, Nui32 val);

        /** 由关键字调用具体的方法(Int级别)
        @version NIIEngine 3.0.0
        */
        void exec(const String & key, bool val);

        /** 由关键字调用具体的方法(Int级别)
        @version NIIEngine 3.0.0
        */
        void exec(const String & key, NIIf val);

        /** 由关键字调用具体的方法(String级别)
        @version NIIEngine 3.0.0
        */
        void exec(const String & key, String & val);

        /** 解析单元
        @version NIIEngine 3.0.0
        */
        void parseUnit(CollectList & list);

        /** 记录错误
        @version NIIEngine 3.0.0
        */
        void logError(const String & error) const;
    protected:
        DataStream * mStream;
        GPUInfo * mGpuInfo;
        RenderFeature * mFeature;
        KeyTypeList mKeyTypeList;
        StringTableMap mStringTableMap;
        Int16TableMap mInt16TableMap;
        Int32TableMap mInt32TableMap;
        BoolTableMap mBoolTableMap;
        FloatTableMap mFloatTableMap;
        FeatureMap mFeatureMap;
        String * mLine;
        NIIi mNumber;
    };
}
#endif