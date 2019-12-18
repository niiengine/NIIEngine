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

#ifndef _NII_ScriptModule_H_
#define _NII_ScriptModule_H_

#include "NiiPreInclude.h"
#include "NiiCommandObj.h"

namespace NII
{
	/** 脚本模块
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI ScriptModule : public UIAlloc
	{
	public:
		ScriptModule();
		virtual ~ScriptModule();

		/** 加载脚本模块
		@version NIIEngine 3.0.0
		*/
		virtual void load();

		/** 卸载脚本模块
		@version NIIEngine 3.0.0
		*/
		virtual void unload();

		/** 脚本模块ID
		@version NIIEngine 3.0.0
		*/
		ScriptModuleID getID() const;

		/** 执行脚本文件
		@version NIIEngine 3.0.0
		*/
		virtual	void executeFile(const String & file, GroupID gid = 0) = 0;

		/** 执行脚本函数
		@version NIIEngine 3.0.0
		*/
		virtual int	executeFunc(const String & func) = 0;

		/** 执行脚本函数
		@param func 函数
		@param arg 参数
		@version NIIEngine 3.0.0
		*/
		virtual	bool executeFunc(const String & func, const EventArgs & arg) = 0;

		/** 执行脚本
		@version NIIEngine 3.0.0
		*/
		virtual void executeScript(const String & script) = 0;

		/** 捆绑脚本函数到命令对象中
		@param obj 命令对象
		@param eid 事件ID
		@param func 脚本函数
		@version NIIEngine 3.0.0
		*/
		virtual SignalPtr add(NII_COMMAND::CommandObj * obj, EventID eid, const String & func) = 0;

		/** 脚本模块
		@version NIIEngine 3.0.0
		*/
		static void setGroup(GroupID gid);

		/** 脚本模块资源组
		@version NIIEngine 3.0.0
		*/
		static GroupID getGroup();
	protected:
		ScriptModuleID mID;
		static GroupID mResourceGroup;
	};

	class _EngineAPI ScriptFunctor : public SysAlloc
	{
	public:
		ScriptFunctor(ScriptModuleID mid, const String & fc) :mModuleID(mid), mFuncName(fc) {}
		ScriptFunctor(const ScriptFunctor & o) : mFuncName(o.mFuncName) {}
		bool operator()(const EventArgs & e) const;
	private:
		ScriptFunctor & operator=(const ScriptFunctor & o);

		String mFuncName;
		ScriptModuleID mModuleID;
	};
}
#endif