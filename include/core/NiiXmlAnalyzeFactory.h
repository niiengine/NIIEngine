/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_XmlAnalyzeFactory_H_
#define _NII_XmlAnalyzeFactory_H_

#include "NiiPreInclude.h"
#include "NiiXmlAnalyze.h"

#define N_XmlAnalyzeFactory(c, lid) PreDefineXmlAnalyzeFactory<c>(lid)

namespace NII
{
    /** xml分析工厂类
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyzeFactory : public XmlAlloc
    {
    public:
        XmlAnalyzeFactory(){}
        virtual ~XmlAnalyzeFactory(){}

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual XmlAnalyze * create(LangID lid) = 0;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(XmlAnalyze * obj) = 0;
    };

    /** 预定制UI单元工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineXmlAnalyzeFactory: public XmlAnalyzeFactory
    {
    public:
        PreDefineXmlAnalyzeFactory(FactoryID fid);

        /// @copydetails XmlAnalyzeFactory::getID
        FactoryID getID() const;

        /// @copydetails XmlAnalyzeFactory::create
        XmlAnalyze * create(LangID lid);

        /// @copydetails XmlAnalyzeFactory::destroy
        void destroy(XmlAnalyze * obj);

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        static void plugin();

        /** 从系统中移出
        @version NIIEngine 3.0.0
        */
        static void unplugin();
    protected:
        FactoryID mID;
    };
    //------------------------------------------------------------------------
    template <typename T> PreDefineXmlAnalyzeFactory<T>::PreDefineXmlAnalyzeFactory(FactoryID fid) :
        mID(fid)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> FactoryID PreDefineXmlAnalyzeFactory<T>::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    template <typename T> XmlAnalyze * PreDefineXmlAnalyzeFactory<T>::create(LangID lid)
    {
        return N_new T(lid);
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::destroy(XmlAnalyze * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::plugin()
    {
        if(N_OnlyPtr(XmlAnalyze))
        {
            XmlAnalyzeFactory * factory = N_new PreDefineXmlAnalyzeFactory<T>(mID);
            if(N_Only(XmlAnalyze).addFactory(factory) != true)
                N_delete factory;
        }
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::unplugin()
    {
        if(N_OnlyPtr(XmlAnalyze))
        {
            XmlAnalyzeFactory * factory = N_Only(XmlAnalyze).getFactory(mID);
            if(factory != 0)
            {
                N_Only(XmlAnalyze).removeFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
}
#endif