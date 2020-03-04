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
        @param[in] default_ 如果为真,这是一个默认的log输出
        @param[in] debugger 如果为真,此日志输出也将传给调试器的输出窗
        @param[in] logic 仅仅逻辑记录,不输出到文件
        @version NIIEngine 3.0.0
        */
        Log * create(const String & name, bool default_ = false, bool debugger = true, bool logic = false);

        /** 检索这个类管理的日志
        @version NIIEngine 3.0.0
        */
        Log * get(const String & name = N_StrBlank);

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
        void log(const String & message, LogLevel lml = LL_Info, bool debug = false);

        /** 记录消息到默认日志
        @version NIIEngine 3.0.0
        */
        inline void log(LogLevel lml, const String & msg, bool debug = false) 
        { 
            log(msg, lml, debug);
        }

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void trace(const Nchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void warning(const Nchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void fatal(const Nchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void debug(const Nchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void info(const Nchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void error(const Nchar * msgformat, ...);

        /** 获取在默认日志的的流
        @version NIIEngine 3.0.0
        */
        Log::Stream stream(LogLevel ll = LL_Info, bool debug = false);

        /** 设置默认详细程度
        @version NIIEngine 3.0.0
        */
        void setLogDetail(LogDetail ll);

        /// @copydetails Singleton::getOnly
        static LogManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static LogManager * getOnlyPtr();
    protected:
        N_mutex1
        typedef map<String, Log *>::type LogList;
    protected:
        LogList mLogs;
        Log * mDefaultLog;
    };

#define N_log(x) N_Only(Log).log(x)

#if N_PLAT == N_PLAT_WIN32
    #ifdef N_DEBUG
        #define N_trace(msg, ...)       N_Only(Log).trace   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_warning(msg, ...)     N_Only(Log).warning ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_fatal(msg, ...)       N_Only(Log).fatal   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_debug(msg, ...)       N_Only(Log).debug   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_info(msg, ...)        N_Only(Log).info    ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_error(msg, ...)       N_Only(Log).error   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
    #else
        #define N_trace(msg, ...)       N_Only(Log).trace   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_warning(msg, ...)     N_Only(Log).warning ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_fatal(msg, ...)       N_Only(Log).fatal   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_debug(msg, ...)       N_Only(Log).debug   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_info(msg, ...)        N_Only(Log).info    ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
        #define N_error(msg, ...)       N_Only(Log).error   ("<%s>|<%d>|<%s>,"msg, N_FLF, ##__VA_ARGS__)
    #endif
#else
    #ifdef N_DEBUG
        #define N_trace(msg, args...)   N_Only(Log).trace   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_warning(msg, args...) N_Only(Log).warning ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_fatal(msg, args...)   N_Only(Log).fatal   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_debug(msg, args...)   N_Only(Log).debug   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_info(msg, args...)    N_Only(Log).info    ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_error(msg, args...)   N_Only(Log).error   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
    #else
        #define N_trace(msg, args...)   N_Only(Log).trace   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_warning(msg, args...) N_Only(Log).warning ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_fatal(msg, args...)   N_Only(Log).fatal   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_debug(msg, args...)   N_Only(Log).debug   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_info(msg, args...)    N_Only(Log).info    ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
        #define N_error(msg, args...)   N_Only(Log).error   ("<%s>|<%d>|<%s>," msg, N_FLF, ##args)
    #endif
#endif
}
#endif