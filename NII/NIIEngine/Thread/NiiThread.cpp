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
#include "NiiThread.h"
#include "NiiLogManager.h"
#include "NiiTimer.h"

#define __STDC_FORMAT_MACROS
#include <limits>
#include <inttypes.h>

namespace NII
{
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// crts_caller
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
#if N_PLAT == N_PLAT_WIN32
	static const wchar_t* const s_listOfCrts[] =
	{
	  { L"msvcrt.dll" },   // Visual Studio 6.0 / MinGW[-w64]
	  { L"msvcr70.dll" },  // Visual Studio 2002
	  { L"msvcr71.dll" },  // Visual Studio 2003
	  { L"msvcr80.dll" },  // Visual Studio 2005
	  { L"msvcr90.dll" },  // Visual Studio 2008
	#ifdef _DEBUG
	  { L"msvcr90d.dll" }, // Visual Studio 2008 (debug)
	#endif
	  { L"msvcr100.dll" }, // Visual Studio 2010
	#ifdef _DEBUG
	  { L"msvcr100d.dll" },// Visual Studio 2010 (debug)
	#endif
	  { L"msvcr110.dll" }, // Visual Studio 2012
	#ifdef _DEBUG
	  { L"msvcr110d.dll" },// Visual Studio 2012 (debug)
	#endif
	  { L"msvcr120.dll" }, // Visual Studio 2013
	#ifdef _DEBUG
	  { L"msvcr120d.dll" },// Visual Studio 2013 (debug)
	#endif
	  { L"msvcr130.dll" },
	#ifdef _DEBUG
	  { L"msvcr130d.dll" },
	#endif
	  { L"msvcr140.dll" }, // Visual Studio 2017
	#ifdef _DEBUG
	  { L"msvcr140d.dll" },// Visual Studio 2017 (debug)
	#endif
	};
	class crts_caller
	{
	public:
		crts_caller(const char* func_name) 
		{
			assert(func_name);
			assert(func_name[0]);
			if (func_name == NULL)
				return;

			for (const wchar_t* const crtName : s_listOfCrts)
			{
				HMODULE hCrt = NULL;
				if (!GetModuleHandleExW(0, crtName, &hCrt) || hCrt == NULL) // Flag 0 ensures that CRL will not be unloaded while we are using it here
					continue; // Module not loaded

				void* func_ptr = GetProcAddress(hCrt, func_name);
				if (func_ptr != NULL)
				{
					m_crts.push_back(hCrt);
					m_funcPointers.push_back(func_ptr);
				}
				else
					FreeLibrary(hCrt); // this CRT will not be used
			}
		}
		const std::vector<void*>& get_pointers(void)
		{
			return m_funcPointers;
		}
		~crts_caller()
		{
			for (void* hCrt : m_crts)
				FreeLibrary((HMODULE)hCrt);
		}

		template <typename ret_type, typename... param_types>
		static typename ret_type call_in_all_crts(const char* func_name, ret_type(*cur_fnc_ptr) (param_types...), param_types... params)
		{
			typedef ret_type(*ptr_type)(param_types...);

			if (cur_fnc_ptr == NULL)
				return (ret_type)0; // cur_fnc_ptr must point to process default CRT function

			crts_caller crts(func_name);
			for (void* func_ptr : crts.m_funcPointers)
			{
				ptr_type func = (ptr_type)func_ptr;
				if (func != cur_fnc_ptr)
					(void)func(params...); // ignoring result of function call
			}

			return cur_fnc_ptr(params...); // return result of calling process's CRT function
		}

		static std::vector<std::wstring> getCrtNames()
		{
			return std::vector<std::wstring>(s_listOfCrts, s_listOfCrts + (sizeof(s_listOfCrts) / sizeof(s_listOfCrts[0])));
		}
	private:
		std::vector<void*> m_funcPointers;
		std::vector<void*> m_crts; // actually contains HMODULE
	};

	// Call function in all loaded CRTs
	// Function must have same return type and same parameters in all CRTs
	#define CALL_IN_CRTS(function,...) crts_caller::call_in_all_crts(#function,&(function),##__VA_ARGS__)
#endif
	static thread_local Thread * gCurrentThread;
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// ThreadEvent
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ThreadEvent::ThreadEvent() :
		mAutoReset(true),
		mSignal(false),
		mCount(0),
		mConditionEvent(mCondition, mSignal)
	{
	}
	//---------------------------------------------------------------------------
	ThreadEvent::ThreadEvent(bool autoreset, bool signal_) :
		mAutoReset(autoreset),
		mSignal(signal_),
		mCount(0),
		mConditionEvent(mCondition, mSignal)
	{
	}
	//---------------------------------------------------------------------------
	void ThreadEvent::addGroup(ThreadEventGroup * group)
	{
		ScopeLock lock(mGroupMutex);
		mGroupList.push_back(group);
	}
	//---------------------------------------------------------------------------
	void ThreadEvent::removeGroup(ThreadEventGroup * group)
	{
		ScopeLock lock(mGroupMutex);
		mGroupList.erase(std::remove(mGroupList.begin(), mGroupList.end(), group), mGroupList.end());
	}
	//---------------------------------------------------------------------------
	// locking is ALWAYS done in this order:
	//  ThreadEvent::mGroupMutex -> ThreadEventGroup::mMutex -> ThreadEvent::mMutex
	void ThreadEvent::set()
	{
		// Originally I had this without locking. Thanks to FernetMenta who
		// pointed out that this creates a race condition between setting
		// checking the signal and calling wait() on the Wait call in the
		// ThreadEvent class. This now perfectly matches the boost example here:
		// http://www.boost.org/doc/libs/1_41_0/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref
		{
			ScopeLock slock(mMutex);
			mSignal = true;
			mConditionEvent.notifyAll();
		}
		{
			ScopeLock l(mGroupMutex);
			for (auto * group : mGroupList)
				group->set(this);
		}
	}
	//---------------------------------------------------------------------------
	bool ThreadEvent::wait(Nui64 ms)
	{
		ScopeLock lock(mMutex);
		mCount++;
		mConditionEvent.wait(mMutex, ms);
		mCount--;
		bool ret = mSignal;
		if (mAutoReset && mCount == 0)
			mSignal = false;
		return ret;
	}
	//---------------------------------------------------------------------------
	bool ThreadEvent::wait()
	{
		ScopeLock lock(mMutex);
		mCount++;
		mConditionEvent.wait(mMutex);
		mCount--;
		bool ret = mSignal;
		if (mAutoReset && mCount == 0)
			mSignal = false;
		return ret;
	}
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// ThreadEventGroup
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ThreadEventGroup::ThreadEventGroup(std::initializer_list<ThreadEvent *> eventlist) :
		mCount(0),
		mEvent(0),
		mSignal(false),
		mConditionEvent(mCondition, mSignal),
		mEventList{ eventlist }
	{
		for (auto * e : mEventList)
		{
			e->addGroup(this);
		}
	}
	//---------------------------------------------------------------------------
	ThreadEventGroup::~ThreadEventGroup()
	{
		for (auto * e : mEventList)
		{
			e->removeGroup(this);
		}
	}
	//---------------------------------------------------------------------------
	ThreadEvent * ThreadEventGroup::wait()
	{
		return wait(std::numeric_limits<unsigned int>::max());
	}
	//---------------------------------------------------------------------------
	/**
	* This will block until any one of the CEvents in the group are
	* signaled or the timeout is reached. If an event is signaled then
	* it will return a pointer to that ThreadEvent, otherwise it will return
	* NULL.
	*/
	// locking is ALWAYS done in this order:
	//  ThreadEvent::mGroupMutex -> ThreadEventGroup::mMutex -> ThreadEvent::mMutex
	//
	// Notice that this method doesn't grab the ThreadEvent::mGroupMutex at all. This
	// is fine. It just grabs the ThreadEventGroup::mMutex and THEN the individual
	// ThreadEvent::mMutex's
	ThreadEvent * ThreadEventGroup::wait(Nui64 milliseconds)
	{
		ScopeLock lock(mMutex); // grab ThreadEventGroup::mMutex
		mCount++;

		// ==================================================
		// This block checks to see if any child mEventList are
		// signaled and sets 'signaled' to the first one it
		// finds.
		// ==================================================
		ThreadEvent* signaled = nullptr;
		for (auto* cur : mEventList)
		{
			ScopeLock lock2(cur->mMutex);
			if (cur->mSignal)
				signaled = cur;
		}
		// ==================================================

		if (!signaled)
		{
			// both of these release the ThreadEventGroup::mMutex
			if (milliseconds == std::numeric_limits<unsigned int>::max())
				mConditionEvent.wait(mMutex);
			else
				mConditionEvent.wait(mMutex, milliseconds);
		} // at this point the ThreadEventGroup::mMutex is reacquired
		mCount--;

		// signaled should have been set by a call to ThreadEventGroup::Set
		ThreadEvent * ret = signaled;
		if (mCount == 0)
		{
			if (signaled)
				// This acquires and releases the ThreadEvent::mMutex. This is fine since the
				//  ThreadEventGroup::mMutex is already being held
				signaled->wait(0); // reset the event if needed
			signaled = nullptr;  // clear the signaled if all the waiters are gone
		}
		return ret;
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	// ThreadMain
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	ThreadMain::~ThreadMain()
	{
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	// Thread
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X || N_PLAT == N_PLAT_OSX|| N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
#include <limits.h>
#if N_PLAT == N_PLAT_ANDROID
#include <unistd.h>
#else
#include <sys/syscall.h>
#endif
#include <sys/resource.h>
#include <string.h>
#if N_PLAT == N_PLAT_FREEBSD
#include <sys/param.h>
#include <pthread_np.h>
#endif

#include <signal.h>
#include "utils/log.h"

//-----------------------------------------------------------------------
	static pthread_mutexattr_t recursiveAttr;
	//-----------------------------------------------------------------------
	static bool SetRecursiveAttr()
	{
		static bool alreadyCalled = false; // initialized to 0 in the data segment prior to startup init code running
		if (!alreadyCalled)
		{
			pthread_mutexattr_init(&recursiveAttr);
			pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
#if N_PLAT != N_PLAT_ANDROID
			pthread_mutexattr_setprotocol(&recursiveAttr, PTHREAD_PRIO_INHERIT);
#endif
			alreadyCalled = true;
		}
		return true; // note, we never call destroy.
	}
	//-----------------------------------------------------------------------
	static bool recursiveAttrSet = SetRecursiveAttr();
	//-----------------------------------------------------------------------
	pthread_mutexattr_t * CRecursiveMutex::getRecursiveAttr()
	{
		if (!recursiveAttrSet) // this is only possible in the single threaded startup code
			recursiveAttrSet = SetRecursiveAttr();
		return &recursiveAttr;
	}
	//-----------------------------------------------------------------------
	static pid_t GetCurrentThreadPid_()
	{
#if N_PLAT == N_PLAT_FREEBSD
		return pthread_getthreadid_np();
#elif N_PLAT == N_PLAT_ANDROID
		return gettid();
#else
		return syscall(SYS_gettid);
#endif
	}
	//-----------------------------------------------------------------------
#ifdef RLIMIT_NICE
// We need to return what the best number than can be passed
// to setPriority is. It will basically be relative to the
// the main thread's nice level, inverted (since "higher" priority
// nice levels are actually lower numbers).
	static int GetUserMaxPriority(int maxPriority)
	{
		// if we're root, then we can do anything. So we'll allow
		// max priority.
		if (geteuid() == 0)
			return maxPriority;

		// get user max prio
		struct rlimit limit;
		if (getrlimit(RLIMIT_NICE, &limit) == 0)
		{
			const int appNice = getpriority(PRIO_PROCESS, getpid());
			const int rlimVal = limit.rlim_cur;

			// according to the docs, limit.rlim_cur shouldn't be zero, yet, here we are.
			// if a user has no entry in limits.conf rlim_cur is zero. In this case the best
			//   nice value we can hope to achieve is '0' as a regular user
			const int userBestNiceValue = (rlimVal == 0) ? 0 : (20 - rlimVal);

			//          running the app with nice -n 10 ->
			// e.g.         +10                 10    -     0   // default non-root user.
			// e.g.         +30                 10    -     -20 // if root with rlimits set.
			//          running the app default ->
			// e.g.          0                  0    -     0   // default non-root user.
			// e.g.         +20                 0    -     -20 // if root with rlimits set.
			const int bestUserSetPriority = appNice - userBestNiceValue; // nice is inverted from prio.
			return std::min(maxPriority, bestUserSetPriority); //
		}
		else
			// If we fail getting the limit for nice we just assume we can't raise the priority
			return 0;
	}
#endif
	//-----------------------------------------------------------------------
	std::uintptr_t Thread::getCurrentNativeHandle()
	{
#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_FREEBSD
		return reinterpret_cast<std::uintptr_t>(pthread_self());
#else
		return pthread_self();
#endif
	}
	//-----------------------------------------------------------------------
	Nui64 Thread::getCurrentNativeId()
	{
		return static_cast<Nui64>(GetCurrentThreadPid_());
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityMin()
	{
		// one level lower than application
		return -1;
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityMax()
	{
		// one level higher than application
		return 1;
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityNormal()
	{
		// same level as application
		return 0;
	}
	//-----------------------------------------------------------------------
	bool Thread::setPriority(NIIi iPriority)
	{
		bool bReturn = false;

		ScopeLock lockIt(mMutex);

		pid_t tid = static_cast<pid_t>(mID);

		if (!tid)
			bReturn = false;
#ifdef RLIMIT_NICE
		else
		{
			// get user max prio given max prio (will take the min)
			int userMaxPrio = GetUserMaxPriority(getPriorityMax());

			// keep priority in bounds
			int prio = iPriority;
			if (prio >= getPriorityMax())
				prio = userMaxPrio; // this is already the min of getPriorityMax and what the user can set.
			if (prio < getPriorityMin())
				prio = getPriorityMin();

			// nice level of application
			const int appNice = getpriority(PRIO_PROCESS, getpid());
			const int newNice = appNice - prio;

			if (setpriority(PRIO_PROCESS, mID, newNice) == 0)
				bReturn = true;
			else
				CLog::Log(LOGERROR, "%s: error %s", __FUNCTION__, strerror(errno));
		}
#endif

		return bReturn;
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriority() const
	{
		int iReturn;

		int appNice = getpriority(PRIO_PROCESS, getpid());
		int prio = getpriority(PRIO_PROCESS, mID);
		iReturn = appNice - prio;

		return iReturn;
	}
	//-----------------------------------------------------------------------
	Nui64 Thread::getAbsUsage()
	{
		ScopeLock lock(mMutex);

		if (!mThread)
			return 0;

		int64_t time = 0;
#if N_PLAT == N_PLAT_OSX|| N_PLAT == N_PLAT_IOS
		thread_basic_info threadInfo;
		mach_msg_type_number_t threadInfoCount = THREAD_BASIC_INFO_COUNT;

		kern_return_t ret = thread_info(pthread_mach_thread_np(static_cast<pthread_t>(mThread->native_handle())),
			THREAD_BASIC_INFO, (thread_info_t)&threadInfo, &threadInfoCount);

		if (ret == KERN_SUCCESS)
		{
			// User time.
			time = ((int64_t)threadInfo.user_time.seconds * 10000000L) + threadInfo.user_time.microseconds * 10L;

			// System time.
			time += (((int64_t)threadInfo.system_time.seconds * 10000000L) + threadInfo.system_time.microseconds * 10L);
		}

#else
		clockid_t clock;
		if (pthread_getcpuclockid(static_cast<pthread_t>(mThread->native_handle()), &clock) == 0)
		{
			struct timespec tp;
			clock_gettime(clock, &tp);
			time = (int64_t)tp.tv_sec * 10000000 + tp.tv_nsec / 100;
		}
#endif

		return time;
	}
	//-----------------------------------------------------------------------
	void term_handler(int signum)
	{
		CLog::Log(LOGERROR, "thread 0x%lx (%lu) got signal %d. calling OnException and terminating thread abnormally.", (long unsigned int) pthread_self(),
			(long unsigned int) pthread_self(), signum);
		Thread * t = Thread::getCurrent();
		if (t)
		{
			t->stop(false);
			t->OnException();
			if (t->isAutoDestroy())
				delete t;
		}
		pthread_exit(NULL);
	}
	/*
	void Thread::SetSignalHandlers()
	{
		struct sigaction action;
		action.sa_handler = term_handler;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		//sigaction (SIGABRT, &action, NULL);
		//sigaction (SIGSEGV, &action, NULL);
	}
	*/
#elif N_PLAT == N_PLAT_WIN32
#include <process.h>
#include <windows.h>

	std::uintptr_t Thread::getCurrentNativeHandle()
	{
		return reinterpret_cast<std::uintptr_t>(::GetCurrentThread());
	}
	//-----------------------------------------------------------------------
	uint64_t Thread::getCurrentNativeId()
	{
		return static_cast<uint64_t>(::GetCurrentThreadId());
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityMin()
	{
		return(THREAD_PRIORITY_IDLE);
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityMax()
	{
		return(N_ThreadPriorityHigh);
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriorityNormal()
	{
		return(N_ThreadPriorityNormal);
	}
	//-----------------------------------------------------------------------
	bool Thread::setPriority(NIIi iPriority)
	{
		bool bReturn = false;

		ScopeLock lock(mMutex);
		if (mThread)
			bReturn = SetThreadPriority(mID, iPriority) == TRUE;

		return bReturn;
	}
	//-----------------------------------------------------------------------
	NIIi Thread::getPriority() const
	{
		ScopeLock lock(mMutex);

		int iReturn = N_ThreadPriorityNormal;
		if (mThread)
			iReturn = GetThreadPriority(mID);
		return iReturn;
	}
	//-----------------------------------------------------------------------
	Nui64 Thread::getAbsUsage()
	{
#if N_PLAT == N_PLAT_WINRT
		// GetThreadTimes is available since 10.0.15063 only
		return 0;
#else
		ScopeLock lock(mMutex);

		if (mThread == nullptr)
			return 0;

		HANDLE tid = static_cast<HANDLE>(mID);

		if (!tid)
			return 0;

		uint64_t time = 0;
		FILETIME CreationTime, ExitTime, UserTime, KernelTime;
		if (GetThreadTimes(tid, &CreationTime, &ExitTime, &KernelTime, &UserTime))
		{
			time = (((uint64_t)UserTime.dwHighDateTime) << 32) + ((uint64_t)UserTime.dwLowDateTime);
			time += (((uint64_t)KernelTime.dwHighDateTime) << 32) + ((uint64_t)KernelTime.dwLowDateTime);
		}
		return time;
#endif
	}
	/*
	void Thread::SetSignalHandlers()
	{
	}
	*/
#endif
	//-----------------------------------------------------------------------
	Thread::Thread(const String & name) :
		mStop(false),
		mStopEvent(true, true),
		mStartEvent(true, true),
		mAutoDestroy(false),
		mMainAutoDestroy(false),
		mMain(0),
		mMainArg(0),
		mID(0),
		mLastUsage(0),
		mLastUsageTime(0),
		mLastRelUsage(0.0f),
		mThread(0)
	{
		mName = name;
	}
	//-----------------------------------------------------------------------
	Thread::Thread(const String & name, ThreadMain * main_, void * arg, bool autoDestroyMain) :
		mStop(false),
		mStopEvent(true, true),
		mStartEvent(true, true),
		mMain(main_),
		mMainArg(arg),
		mAutoDestroy(false),
		mMainAutoDestroy(autoDestroyMain),
		mID(0),
		mLastUsage(0),
		mLastUsageTime(0),
		mLastRelUsage(0.0f),
		mThread(0)
	{
		mName = name;
	}
	//-----------------------------------------------------------------------
	Thread::~Thread()
	{
		stop();
		if (mThread)
		{
			mThread->detach();
			delete mThread;
			mThread = 0;
		}
		if (mMainAutoDestroy && mMain)
		{
			N_delete mMain;
			mMain = 0;
		}
	}
	//-----------------------------------------------------------------------
	void Thread::create(bool autoDestroy)
	{
		if (mThread)
		{
			// if the thread exited on it's own, without a call to stop, then we can get here
			// incorrectly. We should be able to determine this by checking the promise.
			std::future_status stat = mRun.wait_for(std::chrono::milliseconds(0));
			// a status of 'ready' means the future contains the value so the thread has exited
			// since the thread can't exit without setting the future.
			if (stat == std::future_status::ready) // this is an indication the thread has exited.
				stop(true);  // so let's just clean up
			else
			{ // otherwise we have a problem.
				N_error("%s - fatal error creating thread %s - old thread id not null", mName.c_str());
				exit(1);
			}
		}
		mLastUsageTime = SystemTick() * 10000ULL;
		mLastUsage = 0;
		mLastRelUsage = 0.0f;
		mAutoDestroy = autoDestroy;
		mStop = false;
		mStopEvent.reset();
		mStartEvent.reset();

		// lock?
		//ScopeLock l(mMutex);

		std::promise<bool> prom;
		mRun = prom.get_future();

		{
			// The std::thread internals must be set prior to the lambda doing
			//   any work. This will cause the lambda to wait until mThread
			//   is fully initialized. Interestingly, using a std::atomic doesn't
			//   have the appropriate memory barrier behavior to accomplish the
			//   same thing so a full system mutex needs to be used.
			ScopeLock blockLambdaTillDone(mMutex);
			mThread = new std::thread([](Thread * t, std::promise<bool> promise)
			{
				try
				{

					{
						// Wait for the t->mThread internals to be set. Otherwise we could
						// get to a place where we're reading, say, the thread id inside this
						// lambda's call stack prior to the thread that kicked off this lambda
						// having it set. Once this lock is released, the Thread::Create function
						// that kicked this off is done so everything should be set.
						ScopeLock waitForThreadInternalsToBeSet(t->mMutex);
					}

					// This is used in various helper methods like GetCurrentThread so it needs
					// to be set before anything else is done.
					gCurrentThread = t;

					if (t == nullptr)
					{
						N_error("%s, sanity failed. thread is NULL.");
						promise.set_value(false);
						return;
					}

					std::stringstream ss;
					ss << std::this_thread::get_id();
					std::string id = ss.str();

					N_debug("Thread %s start, auto delete: %s", t->mName.c_str(), t->mAutoDestroy ? _T("true") : _T("false"));

					t->action();

					// lock during termination
					//{
					//    ScopeLock lock(t->mMutex);
					//    t->TermHandler();
					//}

					if (t->mAutoDestroy)
					{
						N_debug("Thread %s %s terminating , auto delete:", t->mName.c_str(), id.c_str());
						N_delete t;
						t = NULL;
					}
					else
					{
						N_debug("Thread %s %s terminating", t->mName.c_str(), id.c_str());
					}
				}
				catch (const std::exception& e)
				{
                    t->mMain->onPrcInterrupt(t->mMainArg);
					N_debug("Thread Terminating with Exception: %s", e.what());
				}
				catch (...)
				{
                    t->mMain->onPrcInterrupt(t->mMainArg);
					N_debug("Thread Terminating with Exception");
				}

				promise.set_value(true);
			},
				this, std::move(prom));
		} // let the lambda proceed

		mStartEvent.wait(); // wait for the thread just spawned to set its internals
	}
	//-----------------------------------------------------------------------
	bool Thread::isRun() const
	{
		if (mThread != nullptr)
		{
			// it's possible that the thread exited on it's own without a call to stop. If so then
			// the promise should be fulfilled.
			std::future_status stat = mRun.wait_for(std::chrono::milliseconds(0));
			// a status of 'ready' means the future contains the value so the thread has exited
			// since the thread can't exit without setting the future.
			if (stat == std::future_status::ready) // this is an indication the thread has exited.
				return false;
			return true; // otherwise the thread is still active.
		}
		else
			return false;
	}
	//-----------------------------------------------------------------------
	void Thread::stop(bool bWait)
	{
		mStartEvent.wait();

		mStop = true;
		mStopEvent.set();
		ScopeLock lock(mMutex);
		std::thread * t = mThread;
		if (t && bWait && !isCurrent())
		{
			lock.unlock();
			if (!join(0xFFFFFFFF)) // eh?
				t->join();
                
            mMain->onPrcStop(mMainArg);
			mThread = nullptr;
		}
	}
	//-----------------------------------------------------------------------
	void Thread::process()
	{
		if (mMain)
		{
			mMain->onPrcBegin(mMainArg);
			while (mMain->isLoop())
			{
				mMain->run(mMainArg);
			}
			mMain->onPrcBegin(mMainArg);
		}
	}
	//-----------------------------------------------------------------------
	NIIi Thread::abort(ThreadEvent & event, TimeDurMS timeout)
	{
		ThreadEventGroup group{ &event, &mStopEvent };
		ThreadEvent * result = timeout < 0 ? group.wait() : group.wait(timeout);
		return result == &event ? TWait_Signaled : (result == NULL ? TWait_Timeout : TWait_Interrupt);
	}
	//-----------------------------------------------------------------------
	bool Thread::isCurrent() const
	{
		Thread * t = gCurrentThread;
		if (t)
			return t == this;
		else
			return false;
	}
	//-----------------------------------------------------------------------
	Thread * Thread::getCurrent()
	{
		return gCurrentThread;
	}
	//-----------------------------------------------------------------------
	std::thread::id Thread::getCurrentId()
	{
		return std::this_thread::get_id();
	}
	//-----------------------------------------------------------------------
	//void Thread::TermHandler()
	//{
	//}
	//-----------------------------------------------------------------------
	void Thread::sleep(TimeDurMS ms)
	{
		if (ms > 10 && isCurrent())
			mStopEvent.wait(ms);
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        mMain->onPrcSuspend(mMainArg);
	}
	//-----------------------------------------------------------------------
	bool Thread::join(TimeDurMS ms)
	{
		ScopeLock l(mMutex);
		std::thread * lthread = mThread;
		if (lthread != nullptr)
		{
			if (isCurrent())
				return false;

			{
				ScopeUnlock exit(mMutex); // don't hold the thread lock while we're waiting
				std::future_status stat = mRun.wait_for(std::chrono::milliseconds(ms));
				if (stat != std::future_status::ready)
					return false;
			}

			// it's possible it's already joined since we released the lock above.
			if (lthread->joinable())
				mThread->join();
			return true;
		}
		else
			return false;
	}
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	void Thread::action()
	{
#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X || N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
		mID = GetCurrentThreadPid_();

#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
		pthread_setname_np(mName.c_str());
#elif (N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X) && defined(__GLIBC__)
		// mthread must be set by here.
		pthread_setname_np(mThread->native_handle(), mName.c_str());
#endif

#ifdef RLIMIT_NICE
		// get user max prio
		int userMaxPrio = GetUserMaxPriority(getPriorityMax());

		// if the user does not have an entry in limits.conf the following
		// call will fail
		if (userMaxPrio > 0)
		{
			// start thread with nice level of application
			int appNice = getpriority(PRIO_PROCESS, getpid());
			if (setpriority(PRIO_PROCESS, mID, appNice) != 0)
				CLog::Log(LOGERROR, "%s: error %s", __FUNCTION__, strerror(errno));
		}
#endif
#elif N_PLAT == N_PLAT_WIN32
		static VString tempstr;
		const unsigned int MS_VC_EXCEPTION = 0x406d1388;
		mID = mThread->native_handle();
	#pragma pack(push,8)
		struct THREADNAME_INFO
		{
			DWORD dwType; // must be 0x1000
			LPCSTR szName; // pointer to name (in same addr space)
			DWORD dwThreadID; // thread ID (-1 caller thread)
			DWORD dwFlags; // reserved for future use, most be zero
		} info;
	#pragma pack(pop)
		StrConv::conv(mName, tempstr);
		info.dwType = 0x1000;
		info.szName = tempstr.c_str();
		info.dwThreadID = reinterpret_cast<std::uintptr_t>(mID);
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}

		// avoid crashing with setlocale(), see https://connect.microsoft.com/VisualStudio/feedback/details/794122
		//const int param = true ? _ENABLE_PER_THREAD_LOCALE : _DISABLE_PER_THREAD_LOCALE;
		CALL_IN_CRTS(_configthreadlocale, _ENABLE_PER_THREAD_LOCALE);// != -1;
		//crts_caller::call_in_all_crts("_configthreadlocale", &(_configthreadlocale), _ENABLE_PER_THREAD_LOCALE);
#endif
		mStartEvent.set();
		actionImpl();
	}
	//-----------------------------------------------------------------------
	void Thread::actionImpl()
	{
		try
		{
			onStart();
			process();
			onExit();
		}
		catch (const Exception &)
		{
			if (mAutoDestroy)
				return;
		}
	}
    //-----------------------------------------------------------------------
    NIIf Thread::getRelUsage()
    {
        unsigned int iTime = SystemTick();
        iTime *= 10000; // convert into 100ns tics

        // only update every 1 second
        if (iTime < mLastUsageTime + 1000 * 10000)
            return mLastRelUsage;

        Nui64 iUsage = getAbsUsage();

        if (mLastUsage > 0 && mLastUsageTime > 0)
            mLastRelUsage = static_cast<float>(iUsage - mLastUsage) / static_cast<float>(iTime - mLastUsageTime);

        mLastUsage = iUsage;
        mLastUsageTime = iTime;

        return mLastRelUsage;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GroupThreadMain
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GroupThreadMain::GroupThreadMain() :
        mRun(true)
    {
    }
    //-----------------------------------------------------------------------
    GroupThreadMain::~GroupThreadMain()
    {
        if(mRun)
        {
            mRun = false;
            mThreadMainCondition.notifyAll();
        }
        mThreadMainMutex.lock();
        ThreadMainList::iterator i, iend = mThreadMainList.end();
        for(i = mThreadMainList.begin(); i != iend; ++i)
        {
            delete *i;
        }
        mThreadMainList.clear();
        mThreadMainMutex.unlock();
    }
    //-----------------------------------------------------------------------
    void GroupThreadMain::addMain(ThreadMain * task, void * arg, bool autoDestroy)
    {
        Task * temp = N_new Task(task, arg, autoDestroy);
        mThreadMainMutex.lock();
        mThreadMainList.push_back(temp);
        mThreadMainMutex.unlock();
        mThreadMainCondition.notify();
    }
    //-----------------------------------------------------------------------
    void GroupThreadMain::run(void * arg)
    {
        while(mRun)
        {
            mThreadMainMutex.lock();
            
            while(mThreadMainList.empty())
            {
                if(mRun)
                {
                    mThreadMainCondition.wait(mThreadMainMutex);
                }
                else
                {
                    mThreadMainMutex.unlock();
                    return;
                }
            }

            if(mThreadMainList.empty())
            {
                mThreadMainMutex.unlock();
                continue;
            }
            else
            {
                Task * tmptask = mThreadMainList.front();
				mThreadMainList.pop_front();
                mThreadMainMutex.unlock();

				ThreadMain * tmp = tmptask->mTask;
				tmp->run(tmptask->mArg);
				if(tmp->isLoop())
				{
					addMain(tmp);
				}
				else if(tmptask->mAutoDestroy)
				{
					N_delete tmp;
				}
				N_delete tmptask;
			}
        }
    }
    //-----------------------------------------------------------------------
    void GroupThreadMain::cancel()
    {
        mRun = false;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ThreadPool
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ThreadPool::~ThreadPool()
    {
        shutdown();
    }
    //-----------------------------------------------------------------------
    int ThreadPool::setup(NCount threadcnt)
    {
        GroupThreadMain * tempmain;
        Thread * tempthread;
        for(NCount i = 0; i > threadcnt; ++i)
        {
            tempmain = N_new GroupThreadMain();
            tempthread = N_new Thread(_T(""), tempmain);
            tempthread->create();
            mThreadList.push_back(tempthread);
        }
		return threadcnt;
    }
    //-----------------------------------------------------------------------
    void ThreadPool::addTask(ThreadMain * task, void * arg, bool autoDestroy)
    {
        int i = Math::randomi() % mThreadList.size();
        ThreadList::iterator j = mThreadList.begin();
        std::advance(j, i);
        static_cast<GroupThreadMain *>((*j)->getThreadMain())->addMain(task, arg, autoDestroy);
    }
    //-----------------------------------------------------------------------
    void ThreadPool::shutdown()
    {
        ThreadList::iterator i, iend = mThreadList.end();
        for(i = mThreadList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mThreadList.clear();
    }
    //-----------------------------------------------------------------------
}