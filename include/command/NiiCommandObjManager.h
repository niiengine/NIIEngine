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
			CoreAttribute,					///< 核心AI自动命令  //2
			CoreResourceManager,			///< 资源管理器发出命令 //3
			CL_External,					///< 外部系统发出命令 如网络 //4
			CL_User,                        ///< 用户发出命令 如 键盘 鼠标 等 //5
			CL_Level_1,						///< 1级  //6
			CL_Level_2,						///< 2级  //7
			CL_Level_3,						///< 3级  //8
			CL_Level_4,						///< 4级  //9
			CL_Level_5,						///< 5级  //10
			CL_Level_6,						///< 6级  //11
			CL_Level_7,						///< 7级 基元 如 NIIb NIIi NIIf 等 //12
			CL_Unknow						///< 13
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