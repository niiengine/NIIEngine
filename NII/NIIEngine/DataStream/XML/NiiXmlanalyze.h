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

#ifndef _NII_XML_Analyze_H_
#define _NII_XML_Analyze_H_

#include "NiiPreInclude.h"
#include "NiiScriptProperty.h"

namespace NII
{        
    typedef void (XmlAnalyze::*PrcCB)(const PropertyData * pdc);
    /** xml成分器
    @remark 多语言版本属性节点,<?xml version="1.0" encoding="UTF-8"?>,并且xml的文本编码存
        储必须使用和申明相同的utf-8.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyze : public ScriptProperty, public ScriptAlloc
    {
    public:
        typedef map<PropertyID, PrcCB>::type PrcCBMap;
    public:
        XmlAnalyze(VersionID version, ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~XmlAnalyze();

        /** 设置是否不处理其他(子)成分器
        @note 默认处理其他成分器
        @version NIIEngine 3.0.0
        */
        void setSkipOther(bool b);

        /** 获取是否不处理其他成分器
        @note 默认处理其他成分器
        @version NIIEngine 3.0.0
        */
        bool isSkipOther() const;

        /** 设置开始处理函数
        @param[in] pid 属性
        @param[in] prc 处理回调
        @version NIIEngine 3.0.0
        */
        void setBeginPrc(PropertyID pid, PrcCB prc);

        /** 解除开始处理函数
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unBeginPrc(PropertyID pid);

        /** 设置结束处理函数
        @param[in] pid 属性
        @param[in] prc 处理回调
        @version NIIEngine 3.0.0
        */
        void setEndPrc(PropertyID pid, PrcCB prc);

        /** 解除结束处理函数
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unEndPrc(PropertyID pid);

        /** 处理标签
        @param[in] parent 父标签
        @param[in] pid 标签属性
        @param[in] pdc 标签数据成分
        @version NIIEngine 3.0.0
        */
        virtual void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** 处理标签
        @param[in] parent 父标签
        @param[in] pid 属性ID
        @version NIIEngine 3.0.0
        */
        virtual void onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** 处理数据
        @param data 属性值
        @version NIIEngine 3.0.0
        */
        virtual void onData(const String & data);

        /** 处理数据
        @param data 属性值
        @version NIIEngine 3.0.0
        */
        virtual void onData(const Nutf8 * data);

        /** 获取Xml类型定义文件
        @return 文件(包含文件名扩展名)
        @version NIIEngine 3.0.0
        */
        virtual const String & getXSD() const;

        /** 获取Xml成分ID
        @note 子类必须实现
        @version NIIEngine 3.0.0
        */
        virtual XmlUnitID getUnit() const = 0;

        /** 获取群组ID
        @note 子类必须实现
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const = 0;
    protected:
        /** 处理本节开始
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitCB(const PropertyData * pdc) = 0;

        /** 处理本节结束
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitECB(const PropertyData * pdc);
    protected:
        PrcCBMap mBeginMap;
        PrcCBMap mEndMap;
        XmlAnalyze * mTag;
        VersionID mVersion;
        bool mAutoTag;
        bool mSkipOther;
    };
}
#endif