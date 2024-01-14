/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "threads/Event.h"
#include "threads/ThreadMain.h"
#include "threads/SharedSection.h"
#include "threads/SingleLock.h"
#include "threads/test/TestHelpers.h"

#include <stdio.h>

//=============================================================================
// Helper classes
//=============================================================================

template<bool wL>
class locker : public ThreadMain
{
  ThreadRWMutex& sec;
  ThreadEvent* wait;

  std::atomic<long>* mutex;
public:
  volatile bool haslock;
  volatile bool obtainedlock;

  inline locker(ThreadRWMutex& o, std::atomic<long>* mutex_ = NULL, ThreadEvent* wait_ = NULL) :
    sec(o), wait(wait_), mutex(mutex_), haslock(false), obtainedlock(false) {}

  inline locker(ThreadRWMutex& o, ThreadEvent* wait_ = NULL) :
    sec(o), wait(wait_), mutex(NULL), haslock(false), obtainedlock(false) {}

  void Run() override
  {
    AtomicGuard g(mutex);
    ScopeRWLock lock(sec, wL);
    haslock = true;
    obtainedlock = true;
    if (wait)
      wait->Wait();
    haslock = false;
  }
};

TEST(TestCritSection, General)
{
  ThreadMutex sec;

  ScopeLock l1(sec);
  ScopeLock l2(sec);
}

TEST(TestSharedSection, General)
{
  ThreadRWMutex sec;

  ScopeRWLock l1(sec);
  ScopeRWLock l2(sec);
}

TEST(TestSharedSection, GetSharedLockWhileTryingExclusiveLock)
{
  std::atomic<long> mutex(0L);
  ThreadEvent event;

  ThreadRWMutex sec;

  ScopeRWLock l1(sec); // get a shared lock

  locker<true> l2(sec,&mutex);
  thread waitThread1(l2); // try to get an exclusive lock

  EXPECT_TRUE(waitForThread(mutex,1,10000));
  SleepMillis(10);  // still need to give it a chance to move ahead

  EXPECT_TRUE(!l2.haslock);  // this thread is waiting ...
  EXPECT_TRUE(!l2.obtainedlock);  // this thread is waiting ...

  locker<false> l3(sec,&mutex,&event);
  thread waitThread3(l3); // try to get a shared lock
  EXPECT_TRUE(waitForThread(mutex,2,10000));
  SleepMillis(10);
  EXPECT_TRUE(l3.haslock);

  event.Set();
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));

  // l3 should have released.
  EXPECT_TRUE(!l3.haslock);

  // but the exclusive lock should still not have happened
  EXPECT_TRUE(!l2.haslock);  // this thread is waiting ...
  EXPECT_TRUE(!l2.obtainedlock);  // this thread is waiting ...

  // let it go
  l1.Leave(); // the last shared lock leaves.

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));

  EXPECT_TRUE(l2.obtainedlock);  // the exclusive lock was captured
  EXPECT_TRUE(!l2.haslock);  // ... but it doesn't have it anymore
}

TEST(TestSharedSection, TwoCase)
{
  ThreadRWMutex sec;

  ThreadEvent event;
  std::atomic<long> mutex(0L);

  locker<false> l1(sec,&mutex,&event);

  {
    ScopeRWLock lock(sec);
    thread waitThread1(l1);

    EXPECT_TRUE(waitForWaiters(event,1,10000));
    EXPECT_TRUE(l1.haslock);

    event.Set();

    EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  }

  locker<false> l2(sec,&mutex,&event);
  {
    ScopeRWLock lock(sec, true); // get exclusive lock
    thread waitThread2(l2); // thread should block

    EXPECT_TRUE(waitForThread(mutex,1,10000));
    SleepMillis(10);

    EXPECT_TRUE(!l2.haslock);

    lock.Leave();

    EXPECT_TRUE(waitForWaiters(event,1,10000));
    SleepMillis(10);
    EXPECT_TRUE(l2.haslock);

    event.Set();

    EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
  }
}

TEST(TestMultipleSharedSection, General)
{
  ThreadRWMutex sec;

  ThreadEvent event;
  std::atomic<long> mutex(0L);

  locker<false> l1(sec,&mutex, &event);

  {
    ScopeRWLock lock(sec);
    thread waitThread1(l1);

    EXPECT_TRUE(waitForThread(mutex,1,10000));
    SleepMillis(10);

    EXPECT_TRUE(l1.haslock);

    event.Set();

    EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  }

  locker<false> l2(sec,&mutex,&event);
  locker<false> l3(sec,&mutex,&event);
  locker<false> l4(sec,&mutex,&event);
  locker<false> l5(sec,&mutex,&event);
  {
    ScopeRWLock lock(sec, true);
    thread waitThread1(l2);
    thread waitThread2(l3);
    thread waitThread3(l4);
    thread waitThread4(l5);

    EXPECT_TRUE(waitForThread(mutex,4,10000));
    SleepMillis(10);

    EXPECT_TRUE(!l2.haslock);
    EXPECT_TRUE(!l3.haslock);
    EXPECT_TRUE(!l4.haslock);
    EXPECT_TRUE(!l5.haslock);

    lock.Leave();

    EXPECT_TRUE(waitForWaiters(event,4,10000));

    EXPECT_TRUE(l2.haslock);
    EXPECT_TRUE(l3.haslock);
    EXPECT_TRUE(l4.haslock);
    EXPECT_TRUE(l5.haslock);

    event.Set();

    EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
    EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
    EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
    EXPECT_TRUE(waitThread4.timed_join(MILLIS(10000)));
  }
}

