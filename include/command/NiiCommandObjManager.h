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

#ifndef _NII_COMMANDOBJECTMANAGER_H_
#define _NII_COMMANDOBJECTMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiCommandObj.h"

namespace NII
{
    using namespace NII_COMMAND;
    class CommandObjectManager : public Singleton<CommandObjectManager>
    {
    public:
        enum Level
        {
            Core,                           ///< 核心发出命令   //1
            CoreAttribute,                    ///< 核心AI自动命令  //2
            CoreResourceManager,            ///< 资源管理器发出命令 //3
            CL_External,                    ///< 外部系统发出命令 如网络 //4
            CL_User,                        ///< 用户发出命令 如 键盘 鼠标 等 //5
            CL_Level_1,                        ///< 1级  //6
            CL_Level_2,                        ///< 2级  //7
            CL_Level_3,                        ///< 3级  //8
            CL_Level_4,                        ///< 4级  //9
            CL_Level_5,                        ///< 5级  //10
            CL_Level_6,                        ///< 6级  //11
            CL_Level_7,                        ///< 7级 基元 如 NIIb NIIi NIIf 等 //12
            CL_Unknow                        ///< 13
        };
    public:
        CommandObjectManager();
        ~CommandObjectManager();
        
        void add(CommandObj *);
        NII_COMMAND::CommandObj * get(Nid id);
        void remove(Nid id);
        void findUnder(Nid pid, Nid cid);
    protected:
        typedef map<Nid, CommandObj *>::type CommandObjectMap;
        typedef vector<CommandObjectMap>::type CommandObjectList;
        typedef map<Nid, CommandObjectList>::type ClassCommandObjects;

    private:
        static CommandObjectMap mCommandObjList;
    };
}
#endif