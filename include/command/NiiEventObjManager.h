/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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