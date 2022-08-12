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

#ifndef _NII_EVENTMANAGER_H_
#define _NII_EVENTMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiEventObj.h"
#include "NiiCommandObj.h"

namespace NII
{
    using namespace NII_COMMAND;
    /** 这个类是用来管理对象的,这个对象有一个属性是可以接收事件
    @remark
    */
    class _EngineAPI EventManager : public Singleton<EventManager>
    {
    public:
        /// 新事件类型
        enum EventCmd
        {
            EC_Generate_Object,     ///< 新建对象的事件
            EC_Delete_Object,       ///< 删除对象的事件
            EC_Adjust_Object,       ///< 调整对象的事件
            EC_Generate_Event,      ///< 生成事件的事件
            EC_Delete_Event,        ///< 删除事件的事件
            EC_Adjust_Event         ///< 调整事件的事件
        };

        /// 等级
        enum Level
        {
            Core,                   ///< 核心发出命令   //1
            CoreAttribute,          ///< 核心AI自动命令  //2
            CoreResourceManager,    ///< 资源管理器发出命令 //3
            CL_External,            ///< 外部系统发出命令 如网络 //4
            CL_User,                ///< 用户发出命令 如 键盘 鼠标 等 //5
            CL_Level_1,             ///< 1级  //6
            CL_Level_2,             ///< 2级  //7
            CL_Level_3,             ///< 3级  //8
            CL_Level_4,             ///< 4级  //9
            CL_Level_5,             ///< 5级  //10
            CL_Level_6,             ///< 6级  //11
            CL_Level_7,             ///< 7级 基元 如 NIIb NIIi NIIf 等 //12
            CL_Unknow               ///< 13
        };

        N_mutex1
        typedef map<Nid, EventObj *>::type EventObjectMap;
        typedef map<Nid, CommandObj *>::type CommandObjectMap;
    public:
        EventManager();
        ~EventManager();

        /**
        @version NIIEngine 3.0.0
        */
        void processEvent();

        /**
        @version NIIEngine 3.0.0
        */
        void add(EventObj * obj);

        /**
        @version NIIEngine 3.0.0
        */
        void add(CommandObj * obj);

        /**
        @version NIIEngine 3.0.0
        */
        void removeEvent(Nid id);

        /**
        @version NIIEngine 3.0.0
        */
        void removeCommand(Nid id);

        /**
        @param[id] id = 0 global EventObj mode.
        @version NIIEngine 3.0.0
        */
        EventObj * getEvent(Nid id = 0);

        /**
        @param[id] id = 0 global CommandObj mode.
        @version NIIEngine 3.0.0
        */
        CommandObj * getCommand(Nid id = 0);

        /**
        @version NIIEngine 3.0.0
        */
        void setGlobalEnable(bool set)                      { mGlobalEnable = set; }

        /**
        @version NIIEngine 3.0.0
        */
        bool isGlobalEnable() const                         { return mGlobalEnable; }

        /**
        @version NIIEngine 3.0.0
        */
        const EventObjectMap & getEventObjList() const      { return mEventObjects;  }

        /**
        @version NIIEngine 3.0.0
        */
        const CommandObjectMap & getCommandObjList() const  { return mCommandObjects; }

        void findUnder(Nid pid, Nid cid);
    protected:
        typedef vector<CommandObjectMap>::type CommandObjectList;
        typedef map<Nid, CommandObjectList>::type ClassCommandObjects;
    private:
        CommandObjectMap mCommandObjects;
        EventObjectMap mEventObjects;
        bool mGlobalEnable;
        static CommandObjectMap mCommandObjList;
    };
}
#endif