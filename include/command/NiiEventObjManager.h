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

#ifndef _NII_EVENTOBJECTMANAGER_H_
#define _NII_EVENTOBJECTMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiEventObj.h"

namespace NII
{
    /** 这个类是用来管理对象的,这个对象有一个属性是可以接收事件
    @remark
    */
    class EventObjectManager : public Singleton<EventObjectManager>
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
        N_mutex1
        typedef map<Nid, NII_COMMAND::EventObj *>::type EventObjectMap;
    public:
        EventObjectManager();
        ~EventObjectManager();
        void add(NII_COMMAND::EventObj * obj);
        void remove(NII_COMMAND::EventObj * obj);
        const EventObjectMap & getList() const;

        /// @copydetails Singleton::getOnly
        static EventObjectManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static EventObjectManager * getOnlyPtr();
    private:
        EventObjectMap mEventObjects;
    };
}
#endif