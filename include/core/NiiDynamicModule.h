/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-30

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
#ifndef _NII_DynamicModule_H_
#define _NII_DynamicModule_H_

#include "NiiPreInclude.h"

#if N_PLAT == N_PLAT_WIN32
#define DYNLIB_HANDLE hInstance
#define DYNLIB_EXT ".dll"
#define DYNLIB_PATH String(_T("../bin/"))
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_ANDROID
#define DYNLIB_HANDLE void *
#define DYNLIB_EXT ".so"
#elif N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
#define DYNLIB_HANDLE void*
#define DYNLIB_EXT ".dylib"
#define DYNLIB_PATH String("@executable_path/../Frameworks/")
#elif N_PLAT == N_PLAT_NACL
#define DYNLIB_HANDLE void*
#define DYNLIB_EXT
#define DYNLIB_PATH String("")
#endif 

namespace NII
{
	/** 动态库
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DynamicModule : public DynamicModuleAlloc
	{
	public:
		DynamicModule(const String & name);
		~DynamicModule();

		/** 加载库
		@version NIIEngine 3.0.0
		*/
		void load();

		/** 卸载库
		@version NIIEngine 3.0.0
		*/
		void unload();

		/** 获取库的名字
		@version NIIEngine 3.0.0
		*/
		const String & getName() const;

		/** 返回符号地址
		@param[in] name 查找符号的名字
		@return 如果这个函数成功.返回符号的句柄值,否则返回0
		@version NIIEngine 3.0.0
		*/
		void * getSymbol(const VString & name) const throw();
	protected:
		/** 获取最后加载错误
		@version NIIEngine 3.0.0
		*/
		String getLastError();
	protected:
		String mName;
		DYNLIB_HANDLE mInst;
	};
}
#endif