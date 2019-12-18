/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-29

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        @param[in] own 父标签
        @param[in] pid 标签属性
        @param[in] pdc 标签数据成分
        @version NIIEngine 3.0.0
        */
        virtual void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** 处理标签
        @param[in] own 父标签
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