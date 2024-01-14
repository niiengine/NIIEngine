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

#ifndef _NII_LogManager_H_
#define _NII_LogManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiString.h"

namespace NII
{
    /** 该日志的等级
    @version NIIEngine 3.0.0
    */
    enum LogDetail
    {
        LL_LOW = 1,
        LL_NORMAL = 2,
        LL_DETAIL = 3
    };

    /** 一个记录信息的重要性
    @version NIIEngine 3.0.0
    */
    enum LogLevel
    {
        LL_Trace = 1,
        LL_Info = 2,
        LL_Warning = 3,
        LL_Debug = 4,
        LL_Error = 5,
        LL_Fatal = 6
    };

    /** Log监听日志类
    @version NIIEngine 3.0.0
    */
    class LogListener
    {
    public:
        virtual ~LogListener() {};

        /** 这将会被调用当该日志收到一条消息,并准备记录下来
        @param[in] msg 被记录下来的消息
        @param[in] ll 日志使用的消息级别的
        @param[in] debug 我们是否要打印到控制台
        @param[in] name 日志名字
        @return 信息是否需要被记录
        @version NIIEngine 3.0.0
        */
        virtual bool message(const String & msg, LogLevel ll, bool debug, const String & name) = 0;
    };

    /** 日志基本类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Log : public LogAlloc
    {
        friend class LogManager;
    public:
        /** 流对象
        @version NIIEngine 3.0.0
        */
        class Stream
        {
        public:
            /// 简单类型,表示流的一冲刷到日志
            struct Flush {};

            Stream(Log * target, LogLevel lml, bool maskDebug) :
                mTarget(target), mLevel(lml), mMaskDebug(maskDebug) {}

            Stream(const Stream & rhs) :
                mTarget(rhs.mTarget), mLevel(rhs.mLevel), mMaskDebug(rhs.mMaskDebug)
            {
                mCache.str(rhs.mCache.str());
            }

            ~Stream()
            {
                if (mCache.tellp() > 0)
                {
                    mTarget->log(mCache.str(), mLevel, mMaskDebug);
                }
            }

            template <typename T> Stream & operator<< (const T& v)
            {
                mCache << v;
                return *this;
            }

            Stream & operator<< (const Flush& v)
            {
                (void)v;
                mTarget->log(mCache.str(), mLevel, mMaskDebug);
                mCache.str(StrUtil::WBLANK);
                return *this;
            }
        protected:
            Log * mTarget;
            LogLevel mLevel;
            StringStream mCache;
            bool mMaskDebug;
        };
    public:
        Log(const String & name, bool debugOutput = true, bool logic = false);
        ~Log();

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const   { return mName; }

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        inline Nidx getIndex() const            { return mIndex; }

        /** 设置是否是否调试输出
        @remark 启用或禁用输出log信息到调试器
        @version NIIEngine 3.0.0
        */
        void setDebugOutput(bool set);

        /** 获取是否调试输出
        @version NIIEngine 3.0.0
        */
        inline bool isDebugOutput() const       { return mDebugOut; }

        /** 获取输出到文件
        @version NIIEngine 3.0.0
        */
        inline bool isLogicOutput() const       { return mLogic; }

        /** 设置是否输出时间标签
        @remark 启用或禁用时间标签
        @version NIIEngine 3.0.0
        */
        void setTimeStamp(bool set);

        /** 获取是否输出时间标签
        @version NIIEngine 3.0.0
        */
        inline bool isTimeStamp() const         { return mTimeStamp; }

        /** 记录信息到日志文件
        @version NIIEngine 3.0.0
        */
        void log(const String & msg, LogLevel lml = LL_Info, bool debug = false);

        /** 产生针对这个日志的流对象
        @version NIIEngine 3.0.0
        */
        Stream stream(LogLevel lml = LL_Info, bool debug = false);

        /** 设置日志的详细程度
        @version NIIEngine 3.0.0
        */
        void setLogDetail(LogDetail ll);

        /** 获取日志的详细程度
        @version NIIEngine 3.0.0
        */
        inline LogDetail getLogDetail() const { return mLogLevel; }

        /** 注册监听器
        @param[in] obj 监听器派生类
        @version NIIEngine 3.0.0
        */
        void add(LogListener * obj);

        /** 注销监听器
        @param[in] obj 监听器派生类
        @version NIIEngine 3.0.0
        */
        void remove(LogListener * obj);
    protected:
        N_mutex1
        typedef vector<LogListener *>::type mtLogListener;
    protected:
        Nidx mIndex;
        Nofstream mLog;
        LogDetail mLogLevel;
        String mName;
        mtLogListener mListeners;
        bool mDebugOut;
        bool mLogic;
        bool mTimeStamp;
    };

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

        /** 设置输入的日志为默认日志
        @return 前一个默认log
        @version NIIEngine 3.0.0
        */
        Log * setDefaultLog(Log * newLog);

        /** 记录消息到默认日志
        @version NIIEngine 3.0.0
        */
        inline void log(const String & message, LogLevel lml = LL_Info, bool debug = false)
        {
            log(message, 0, lml, debug);
        }

        /** 记录消息到默认日志
        @version NIIEngine 3.0.0
        */
        void log(const String & message, Nidx idx, LogLevel lml = LL_Info, bool debug = false);

        /** 记录消息到默认日志
        @version NIIEngine 3.0.0
        */
        inline void log(LogLevel lml, const String & msg, bool debug = false)
        { 
            log(msg, lml, debug);
        }

        /** 记录消息到默认日志
        @version NIIEngine 3.0.0
        */
        inline void log(LogLevel lml, const String & msg, Nidx idx, bool debug = false)
        {
            log(msg, idx, lml, debug);
        }

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void trace(Nidx idx, const Ntchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void warning(Nidx idx, const Ntchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void fatal(Nidx idx, const Ntchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void debug(Nidx idx, const Ntchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void info(Nidx idx, const Ntchar * msgformat, ...);

        /** 记录消息到默认日志
        @version NIIEngine 4.0.0
        */
        void error(Nidx idx, const Ntchar * msgformat, ...);

        /** 获取在默认日志的的流
        @version NIIEngine 3.0.0
        */
        Log::Stream stream(LogLevel ll = LL_Info, bool debug = false);

        /** 设置默认详细程度
        @version NIIEngine 3.0.0
        */
        void setLogDetail(LogDetail ll, Nidx idx = 0);
    protected:
        N_mutex1
        typedef map<String, Log *>::type LogMapList;
        typedef vector<Log *>::type LogList;
    protected:
        LogMapList mLogs;
        LogList mLogList;
        Log * mDefaultLog;
    };

#define N_log(x) N_Only(Log).log(x)

#if N_PLAT == N_PLAT_WIN32
    #ifdef N_DEBUG
        #define N_trace(msg, ...)           N_Only(Log).trace   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_warning(msg, ...)         N_Only(Log).warning (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_fatal(msg, ...)           N_Only(Log).fatal   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_debug(msg, ...)           N_Only(Log).debug   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_info(msg, ...)            N_Only(Log).info    (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_error(msg, ...)           N_Only(Log).error   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_trace(idx, msg, ...) N_Only(Log).trace   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_warning(idx,msg, ...)N_Only(Log).warning (idx,_T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_fatal(idx,msg, ...)  N_Only(Log).fatal   (idx,_T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_debug(idx,msg, ...)  N_Only(Log).debug   (idx,_T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_info(idx,msg, ...)   N_Only(Log).info    (idx,_T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_error(idx,msg, ...)  N_Only(Log).error   (idx,_T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
    #else
        #define N_trace(msg, ...)           N_Only(Log).trace   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_warning(msg, ...)         N_Only(Log).warning (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_fatal(msg, ...)           N_Only(Log).fatal   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_debug(msg, ...)           N_Only(Log).debug   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_info(msg, ...)            N_Only(Log).info    (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_error(msg, ...)           N_Only(Log).error   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_trace(idx, msg, ...)      N_Only(Log).trace   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_warning(idx, msg, ...)    N_Only(Log).warning (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_fatal(idx, msg, ...)      N_Only(Log).fatal   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_debug(idx, msg, ...)      N_Only(Log).debug   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_info(idx, msg, ...)       N_Only(Log).info    (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
        #define N_Type_error(idx, msg, ...)      N_Only(Log).error   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##__VA_ARGS__)
    #endif
#else
    #ifdef N_DEBUG
        #define N_trace(msg, args...)       N_Only(Log).trace   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_warning(msg, args...)     N_Only(Log).warning (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_fatal(msg, args...)       N_Only(Log).fatal   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_debug(msg, args...)       N_Only(Log).debug   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_info(msg, args...)        N_Only(Log).info    (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_error(msg, args...)       N_Only(Log).error   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_trace(idx, msg, args...)   N_Only(Log).trace   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_warning(idx, msg, args...) N_Only(Log).warning (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_fatal(idx, msg, args...)   N_Only(Log).fatal   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_debug(idx, msg, args...)   N_Only(Log).debug   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_info(idx, msg, args...)    N_Only(Log).info    (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_error(idx, msg, args...)   N_Only(Log).error   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
    #else
        #define N_trace(msg, args...)       N_Only(Log).trace   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_warning(msg, args...)     N_Only(Log).warning (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_fatal(msg, args...)       N_Only(Log).fatal   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_debug(msg, args...)       N_Only(Log).debug   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_info(msg, args...)        N_Only(Log).info    (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_error(msg, args...)       N_Only(Log).error   (0, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_trace(idx, msg, args...)   N_Only(Log).trace   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_warning(idx, msg, args...) N_Only(Log).warning (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_fatal(idx, msg, args...)   N_Only(Log).fatal   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_debug(idx, msg, args...)   N_Only(Log).debug   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_info(idx, msg, args...)    N_Only(Log).info    (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
        #define N_Type_error(idx, msg, args...)   N_Only(Log).error   (idx, _T("<%s>|<%d>|<%s>,")msg, N_FLF, ##args)
    #endif
#endif
}
#endif