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

#ifndef _NII_AUTOMATIONSERIALISER_H_
#define _NII_AUTOMATIONSERIALISER_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 注解 这个类存储自动化类的阶段存储
    */
    class AutomationSerialiser
    {
    public:
        /**
        自动处理事件类集
        */
#if (NII_VOLUME==NII_SMALL)
        typedef map<Nui16, Automation*>::type AutomationMap;
#elif (NII_VOLUME==NII_MEDIUM)
        typedef map<Nui32, Automation*>::type AutomationMap;
#elif (NII_VOLUME==NII_BIG)
        typedef map<Nui32, Automation*>::type AutomationMap;
#endif
        typedef vector<Automation>::type AutomationList;
        void addRule();
       //核心发出命令规律
        void addCoreRule();
        //核心AI自动命令规律
        void addCoreAttributeRule();
        //资源管理器发出命令规律
        void addResourceManagerRule();
        //外部系统发出命令规律 如网络 键盘
        void addExternalRule();
        //用户发出命令规律 如 键盘 鼠标 等
        void addUserRule();
        //1级命令规律
        void addLevel_1Rule();
        //2级命令规律
        void addLevel_2Rule();
        //3级命令规律
        void addLevel_3Rule();
        //4级命令规律
        void addLevel_4Rule();
        //5级命令规律
        void addLevel_5Rule();
        //6级命令规律
        void addLevel_6Rule();
        //7级命令规律
        void addLevel_7Rule();
    protected:

    };
}
}
#endif //