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
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_LogManager_H_
#define _NII_LogManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiLog.h"

namespace NII
{
    /** 这个日志管理器为应用程序处理创建/检索日志
	@version NIIEngine 3.0.0
    */
    class _EngineAPI LogManager : public Singleton<LogManager>, public LogAlloc
    {
    public:
        LogManager();

        ~LogManager();

        /** 给定名字创建一个新的日志
        @param[in] name 给定log的名字 如'Nii.log'
        @param[in] defaultLog 如果为真,这是一个默认的log输出
        @param[in] debugger 如果为真,此日志输出也将传给调试器的输出窗
        @param[in] logicOutput 仅仅逻辑记录,不输出到文件
		@version NIIEngine 3.0.0
        */
        Log * createLog(const String & name, bool defaultLog = false, bool debugger = true,
            bool logicOutput = false);

        /** 检索这个类管理的日志
		@version NIIEngine 3.0.0
		*/
		Log * getLog(const String & name);

        /** 返回默认日志的一个指针
		@version NIIEngine 3.0.0
		*/
		Log * getLog();

        /** 关闭删除关闭日志
		@version NIIEngine 3.0.0
		*/
		void destroyLog(const String & name);

        /** 关闭删除关闭日志
		@version NIIEngine 3.0.0
		*/
		void destroyLog(Log * log);

        /** 设置输入的日志为默认日志
        @return 前一个默认log
		@version NIIEngine 3.0.0
        */
        Log * setDefaultLog(Log * newLog);

        /** 记录消息到默认日志
		@version NIIEngine 3.0.0
		*/
		void log(const String & message, LogLevel lml = LML_NORMAL, bool debug = false);

        /** 记录消息到默认日志
		@version NIIEngine 3.0.0
		*/
		inline void log(LogLevel lml, const String & msg, bool maskDebug = false) 
        { 
            log(msg, lml, maskDebug);
        }

        /** 获取在默认日志的的流
		@version NIIEngine 3.0.0
		*/
		Log::Stream stream(LogLevel lml = LML_NORMAL, bool maskDebug = false);

        /** 设置默认详细程度
		@version NIIEngine 3.0.0
		*/
		void setLogDetail(LogLife ll);

        /// @copydetails Singleton::getOnly
        static LogManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static LogManager * getOnlyPtr();
    protected:
		N_mutex1
        typedef map<String, Log *>::type LogList;
	protected:
        LogList mLogs;
        Log * mDefaultLog;  ///< 默认的日志,其输出已完成
    };
}
#endif