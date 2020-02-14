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

#ifndef _NII_Log_H_
#define _NII_Log_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 该日志的等级
    @version NIIEngine 3.0.0
    */
    enum LogDetail
    {
        LL_LOW      = 1,
        LL_NORMAL   = 2,
        LL_BOREME   = 3
    };

    /** 一个记录信息的重要性
    @version NIIEngine 3.0.0
    */
    enum LogLevel
    {
        LL_Trace    = 1,
        LL_Info     = 2,
        LL_Warning  = 3, 
        LL_Debug    = 4,
        LL_Error    = 5,
        LL_Fatal    = 6
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
    public:
        /** 流对象
        @version NIIEngine 3.0.0
        */
        class Stream
        {
        public:
            /// 简单类型,表示流的一冲刷到日志
            struct Flush {};

            Stream(Log * target, LogLevel lml, bool maskDebug):
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
        inline const String & getName() const { return mLogName; }

        /** 设置是否是否调试输出
        @remark 启用或禁用输出log信息到调试器
        @version NIIEngine 3.0.0
        */
        void setDebugOutput(bool set);

        /** 获取是否调试输出
        @version NIIEngine 3.0.0
        */
        inline bool isDebugOutput() const { return mDebugOut; }

        /** 获取输出到文件
        @version NIIEngine 3.0.0
        */
        inline bool isLogicOutput() const { return mLogic; }

        /** 设置是否输出时间标签
        @remark 启用或禁用时间标签
        @version NIIEngine 3.0.0
        */
        void setTimeStamp(bool set);

        /** 获取是否输出时间标签
        @version NIIEngine 3.0.0
        */
        inline bool isTimeStamp() const { return mTimeStamp; }

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
        Nofstream mLog;
        LogDetail mLogLevel;
        String mLogName;
        mtLogListener mListeners;
        bool mDebugOut;
        bool mLogic;
        bool mTimeStamp;
    };
}
#endif