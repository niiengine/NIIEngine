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