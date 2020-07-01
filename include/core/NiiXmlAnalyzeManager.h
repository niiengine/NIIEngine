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

#ifndef _NII_XmlAnalyzeManager_H_
#define _NII_XmlAnalyzeManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiXmlAnalyzeFactory.h"

namespace NII
{
    /** Xml分析管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyzeManager : public Singleton<XmlAnalyzeManager>, public ManagerAlloc
    {
    public:
        XmlAnalyzeManager(LangID lid = N_PrimaryLang);
        ~XmlAnalyzeManager();

        /** 添加工厂类
        @version NIIEngine 3.0.0
        */
        bool addFactory(XmlAnalyzeFactory * f);

        /** 移去工厂类
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** 获取工厂类
        @version NIIEngine 3.0.0
        */
        XmlAnalyzeFactory * getFactory(FactoryID fid) const;

        /** 创建xml分析器
        @version NIIEngine 3.0.0
        */
        XmlAnalyze * create(FactoryID fid, LangID lid);

        /** 删除xml分析器
        @version NIIEngine 3.0.0
        */
        void destroy(XmlAnalyze * obj);

        /// @copydetails Singleton::getOnly
        static XmlAnalyzeManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static XmlAnalyzeManager * getOnlyPtr();
    protected:
        typedef map<FactoryID, XmlAnalyzeFactory *>::type XmlAnalyzeFactoryList;
        XmlAnalyzeFactoryList mXmlAnalyzes;
        LangID mLangID;
    };
}
#endif