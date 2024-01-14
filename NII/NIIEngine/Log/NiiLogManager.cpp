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

#include "NiiPreProcess.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include <algorithm>

//需要输出时的阀值
#define NII_LOG_THRESHOLD 4
#define MAX_LOG_LENGTH   1024 * 10

namespace NII
{
    //-----------------------------------------------------------------------
    Log::Log(const String & name, bool debuggerOuput, bool logic) :
        mLogLevel(LL_NORMAL),
        mDebugOut(debuggerOuput),
        mLogic(logic),
        mTimeStamp(true),
        mName(name),
        mIndex(0)
    {
        if (!mLogic)
        {
            mLog.open(name.c_str());
        }
    }
    //-----------------------------------------------------------------------
    Log::~Log()
    {
        N_mutex1_lock
        if (!mLogic)
        {
            mLog.close();
        }
    }
    //-----------------------------------------------------------------------
    void Log::log(const String & msg, LogLevel ll, bool debug)
    {
        N_mutex1_lock
            if ((mLogLevel + ll) >= NII_LOG_THRESHOLD)
            {
                bool logthis = false;
                mtLogListener::iterator i, iend = mListeners.end();
                for (i = mListeners.begin(); i != iend; ++i)
                    logthis = (*i)->message(msg, ll, debug, mName);

                if (logthis)
                {
                    if (mDebugOut && !debug)
                        Ncerr << msg << std::endl;
                    // Write time into log
                    if (!mLogic)
                    {
                        if (mTimeStamp)
                        {
                            struct tm * pTime;
                            time_t ctTime; time(&ctTime);
                            pTime = localtime(&ctTime);
                            mLog << std::setw(2) << std::setfill(_T('0')) << pTime->tm_hour
                                << ":" << std::setw(2) << std::setfill(_T('0')) << pTime->tm_min
                                << ":" << std::setw(2) << std::setfill(_T('0')) << pTime->tm_sec
                                << ": ";
                        }
                        mLog << msg << std::endl;

                        // Flush stcmdream to ensure it is written (incase of a crash, we need log to be up to date)
                        mLog.flush();
                    }
                }
            }
    }
    //------------------------------------------------------------------------
    void Log::setTimeStamp(bool timeStamp)
    {
        N_mutex1_lock
            mTimeStamp = timeStamp;
    }
    //------------------------------------------------------------------------
    void Log::setDebugOutput(bool debugOutput)
    {
        N_mutex1_lock
            mDebugOut = debugOutput;
    }
    //------------------------------------------------------------------------
    void Log::setLogDetail(LogDetail ll)
    {
        N_mutex1_lock
            mLogLevel = ll;
    }
    //------------------------------------------------------------------------
    void Log::add(LogListener * listener)
    {
        N_mutex1_lock
            mListeners.push_back(listener);
    }
    //------------------------------------------------------------------------
    void Log::remove(LogListener * listener)
    {
        N_mutex1_lock
            mListeners.erase(std::find(mListeners.begin(), mListeners.end(), listener));
    }
    //------------------------------------------------------------------------
    Log::Stream Log::stream(LogLevel ll, bool debug)
    {
        return Stream(this, ll, debug);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> LogManager * Singleton<LogManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    LogManager::LogManager()
    {
        mDefaultLog = NULL;
    }
    //-----------------------------------------------------------------------
    LogManager::~LogManager()
    {
        N_mutex1_lock
        LogMapList::iterator i;
        for (i = mLogs.begin(); i != mLogs.end(); ++i)
        {
            N_delete i->second;
        }
    }
    //-----------------------------------------------------------------------
    Log * LogManager::create(const String & name, bool defaultLog, bool debugger, bool logic)
    {
        N_mutex1_lock

        Log * newLog = N_new Log(name, debugger, logic);

        if(!mDefaultLog || defaultLog)
        {
            setDefaultLog(newLog);
        }

        mLogs.insert(LogMapList::value_type(name, newLog));
        newLog->mIndex = mLogList.size();
        mLogList.push_back(newLog);
        return newLog;
    }
    //-----------------------------------------------------------------------
    Log * LogManager::setDefaultLog(Log * newLog)
    {
        N_mutex1_lock
        Log* oldLog = mDefaultLog;
        mDefaultLog = newLog;
        LogList::iterator i, iend = mLogList.end();
        for (i = mLogList.begin(); i != iend; ++i)
        {
            if(*i == oldLog)
                *i = mDefaultLog;
        }
        return oldLog;
    }
    //-----------------------------------------------------------------------
    Log * LogManager::get(const String & name)
    {
        N_mutex1_lock
        LogMapList::iterator i = mLogs.find(name);
        if (i != mLogs.end())
            return i->second;

        return mDefaultLog;
    }
    //-----------------------------------------------------------------------
    void LogManager::destroyLog(const String & name)
    {
        LogMapList::iterator i = mLogs.find(name);
        if (i != mLogs.end())
        {
            Log * temp = i->second;

            mLogs.erase(i);

            if (mDefaultLog == temp)
            {
                if (!mLogs.empty())
                    mDefaultLog = mLogs.begin()->second;
                else
                    mDefaultLog = 0;
            }

            if (mLogs.empty())
            {
                mLogList.clear();
            }
            else
            {
                mLogList[temp->getIndex()] = mDefaultLog;
            }

            N_delete temp;
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::log(const String & message, Nidx idx, LogLevel ll, bool debug)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            mLogList[idx]->log(message, ll, debug);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::trace(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
            Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Trace, true);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::warning(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
			Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Warning, false);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::fatal(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
			Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Fatal, false);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::debug(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
            if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
			Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Debug, true);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::info(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
			Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Info, false);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::error(Nidx idx, const Ntchar * msgformat, ...)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            va_list args;
            va_start(args, msgformat);
			Ntchar buf[MAX_LOG_LENGTH];
            Nvsntprintf(buf, sizeof(buf), msgformat, args);
            va_end(args);
            mLogList[idx]->log(buf, LL_Error, false);
        }
    }
    //-----------------------------------------------------------------------
    void LogManager::setLogDetail(LogDetail ll, Nidx idx)
    {
        N_mutex1_lock
        if (idx < mLogList.size())
        {
            mLogList[idx]->setLogDetail(ll);
        }
    }
    //----------------------------------------------------------------------
    Log::Stream LogManager::stream(LogLevel ll, bool debug)
    {
        N_mutex1_lock
        if (mDefaultLog)
            return mDefaultLog->stream(ll, debug);
        else
            N_EXCEPT(InvalidParam, _I18n("Default log not found. "));

    }
    //----------------------------------------------------------------------
}