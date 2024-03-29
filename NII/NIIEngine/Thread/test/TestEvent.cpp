/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "threads/Event.h"
#include "threads/ThreadMain.h"
#include "threads/test/TestHelpers.h"

#include <memory>
#include <stdio.h>

using namespace XbmcThreads;

//=============================================================================
// Helper classes
//=============================================================================

class waiter : public ThreadMain
{
  ThreadEvent& event;
public:
  bool& result;

  volatile bool waiting;

  waiter(ThreadEvent& o, bool& flag) : event(o), result(flag), waiting(false) {}

  void Run() override
  {
    waiting = true;
    result = event.Wait();
    waiting = false;
  }
};

class timed_waiter : public ThreadMain
{
  ThreadEvent& event;
  unsigned int waitTime;
public:
  int& result;

  volatile bool waiting;

  timed_waiter(ThreadEvent& o, int& flag, int waitTimeMillis) : event(o), waitTime(waitTimeMillis), result(flag), waiting(false) {}

  void Run() override
  {
    waiting = true;
    result = 0;
    result = event.WaitMSec(waitTime) ? 1 : -1;
    waiting = false;
  }
};

class group_wait : public ThreadMain
{
  ThreadEventGroup& event;
  int timeout;
public:
  ThreadEvent* result;
  bool waiting;

  group_wait(ThreadEventGroup& o) : event(o), timeout(-1), result(NULL), waiting(false) {}
  group_wait(ThreadEventGroup& o, int timeout_) : event(o), timeout(timeout_), result(NULL), waiting(false) {}

  void Run() override
  {
    waiting = true;
    if (timeout == -1)
      result = event.wait();
    else
      result = event.wait((unsigned int)timeout);
    waiting = false;
  }
};

//=============================================================================

TEST(TestEvent, General)
{
  ThreadEvent event;
  bool result = false;
  waiter w1(event,result);
  thread waitThread(w1);

  EXPECT_TRUE(waitForWaiters(event,1,10000));

  EXPECT_TRUE(!result);

  event.Set();

  EXPECT_TRUE(waitThread.timed_join(10000));

  EXPECT_TRUE(result);
}

TEST(TestEvent, TwoWaits)
{
  ThreadEvent event;
  bool result1 = false;
  bool result2 = false;
  waiter w1(event,result1);
  waiter w2(event,result2);
  thread waitThread1(w1);
  thread waitThread2(w2);

  EXPECT_TRUE(waitForWaiters(event,2,10000));

  EXPECT_TRUE(!result1);
  EXPECT_TRUE(!result2);

  event.Set();

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));

  EXPECT_TRUE(result1);
  EXPECT_TRUE(result2);

}

TEST(TestEvent, TimedWaits)
{
  ThreadEvent event;
  int result1 = 10;
  timed_waiter w1(event,result1,100);
  thread waitThread1(w1);

  EXPECT_TRUE(waitForWaiters(event,1,10000));

  EXPECT_TRUE(result1 == 0);

  event.Set();

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));

  EXPECT_TRUE(result1 == 1);
}

TEST(TestEvent, TimedWaitsTimeout)
{
  ThreadEvent event;
  int result1 = 10;
  timed_waiter w1(event,result1,50);
  thread waitThread1(w1);

  EXPECT_TRUE(waitForWaiters(event,1,100));

  EXPECT_TRUE(result1 == 0);

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));

  EXPECT_TRUE(result1 == -1);
}

TEST(TestEvent, Group)
{
  ThreadEvent event1;
  ThreadEvent event2;

  ThreadEventGroup group{&event1,&event2};

  bool result1 = false;
  bool result2 = false;

  waiter w1(event1,result1);
  waiter w2(event2,result2);
  group_wait w3(group);

  thread waitThread1(w1);
  thread waitThread2(w2);
  thread waitThread3(w3);

  EXPECT_TRUE(waitForWaiters(event1,1,10000));
  EXPECT_TRUE(waitForWaiters(event2,1,10000));
  EXPECT_TRUE(waitForWaiters(group,1,10000));

  EXPECT_TRUE(!result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(w3.waiting);
  EXPECT_TRUE(w3.result == NULL);

  event1.Set();

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
  SleepMillis(10);

  EXPECT_TRUE(result1);
  EXPECT_TRUE(!w1.waiting);
  EXPECT_TRUE(!result2);
  EXPECT_TRUE(w2.waiting);
  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == &event1);

  event2.Set();

  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));

}

/* Test disabled for now, because it deadlocks
TEST(TestEvent, GroupLimitedGroupScope)
{
  ThreadEvent event1;
  ThreadEvent event2;

  {
    ThreadEventGroup group(&event1,&event2,NULL);

    bool result1 = false;
    bool result2 = false;

    waiter w1(event1,result1);
    waiter w2(event2,result2);
    group_wait w3(group);

    thread waitThread1(w1);
    thread waitThread2(w2);
    thread waitThread3(w3);

    EXPECT_TRUE(waitForWaiters(event1,1,10000));
    EXPECT_TRUE(waitForWaiters(event2,1,10000));
    EXPECT_TRUE(waitForWaiters(group,1,10000));

    EXPECT_TRUE(!result1);
    EXPECT_TRUE(!result2);

    EXPECT_TRUE(w3.waiting);
    EXPECT_TRUE(w3.result == NULL);

    event1.Set();

    EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
    EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
    SleepMillis(10);

    EXPECT_TRUE(result1);
    EXPECT_TRUE(!w1.waiting);
    EXPECT_TRUE(!result2);
    EXPECT_TRUE(w2.waiting);
    EXPECT_TRUE(!w3.waiting);
    EXPECT_TRUE(w3.result == &event1);
  }

  event2.Set();

  SleepMillis(50); // give thread 2 a chance to exit
}*/

TEST(TestEvent, TwoGroups)
{
  ThreadEvent event1;
  ThreadEvent event2;

  ThreadEventGroup group1{&event1,&event2};
  ThreadEventGroup group2{&event1,&event2};

  bool result1 = false;
  bool result2 = false;

  waiter w1(event1,result1);
  waiter w2(event2,result2);
  group_wait w3(group1);
  group_wait w4(group2);

  thread waitThread1(w1);
  thread waitThread2(w2);
  thread waitThread3(w3);
  thread waitThread4(w4);

  EXPECT_TRUE(waitForWaiters(event1,1,10000));
  EXPECT_TRUE(waitForWaiters(event2,1,10000));
  EXPECT_TRUE(waitForWaiters(group1,1,10000));
  EXPECT_TRUE(waitForWaiters(group2,1,10000));

  EXPECT_TRUE(!result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(w3.waiting);
  EXPECT_EQ(w3.result,(void*)NULL);
  EXPECT_TRUE(w4.waiting);
  EXPECT_EQ(w4.result,(void*)NULL);

  event1.Set();

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread4.timed_join(MILLIS(10000)));
  SleepMillis(10);

  EXPECT_TRUE(result1);
  EXPECT_TRUE(!w1.waiting);
  EXPECT_TRUE(!result2);
  EXPECT_TRUE(w2.waiting);
  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == &event1);
  EXPECT_TRUE(!w4.waiting);
  EXPECT_TRUE(w4.result == &event1);

  event2.Set();

  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
}

TEST(TestEvent, AutoResetBehavior)
{
  ThreadEvent event;

  EXPECT_TRUE(!event.WaitMSec(1));

  event.Set(); // event will remain signaled if there are no waits

  EXPECT_TRUE(event.WaitMSec(1));
}

TEST(TestEvent, ManualReset)
{
  ThreadEvent event(true);
  bool result = false;
  waiter w1(event,result);
  thread waitThread(w1);

  EXPECT_TRUE(waitForWaiters(event,1,10000));

  EXPECT_TRUE(!result);

  event.Set();

  EXPECT_TRUE(waitThread.timed_join(MILLIS(10000)));

  EXPECT_TRUE(result);

  // with manual reset, the state should remain signaled
  EXPECT_TRUE(event.WaitMSec(1));

  event.Reset();

  EXPECT_TRUE(!event.WaitMSec(1));
}

TEST(TestEvent, InitVal)
{
  ThreadEvent event(false,true);
  EXPECT_TRUE(event.WaitMSec(50));
}

TEST(TestEvent, SimpleTimeout)
{
  ThreadEvent event;
  EXPECT_TRUE(!event.WaitMSec(50));
}

TEST(TestEvent, GroupChildSet)
{
  ThreadEvent event1(true);
  ThreadEvent event2;

  event1.Set();
  ThreadEventGroup group{&event1,&event2};

  bool result1 = false;
  bool result2 = false;

  waiter w1(event1,result1);
  waiter w2(event2,result2);
  group_wait w3(group);

  thread waitThread1(w1);
  thread waitThread2(w2);
  thread waitThread3(w3);

  EXPECT_TRUE(waitForWaiters(event2,1,10000));
  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
  SleepMillis(10);

  EXPECT_TRUE(result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == &event1);

  event2.Set();

  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
}

TEST(TestEvent, GroupChildSet2)
{
  ThreadEvent event1(true,true);
  ThreadEvent event2;

  ThreadEventGroup group{&event1,&event2};

  bool result1 = false;
  bool result2 = false;

  waiter w1(event1,result1);
  waiter w2(event2,result2);
  group_wait w3(group);

  thread waitThread1(w1);
  thread waitThread2(w2);
  thread waitThread3(w3);

  EXPECT_TRUE(waitForWaiters(event2,1,10000));
  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));
  SleepMillis(10);

  EXPECT_TRUE(result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == &event1);

  event2.Set();

  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
}

TEST(TestEvent, GroupWaitResetsChild)
{
  ThreadEvent event1;
  ThreadEvent event2;

  ThreadEventGroup group{&event1,&event2};

  group_wait w3(group);

  thread waitThread3(w3);

  EXPECT_TRUE(waitForWaiters(group,1,10000));

  EXPECT_TRUE(w3.waiting);
  EXPECT_TRUE(w3.result == NULL);

  event2.Set();

  EXPECT_TRUE(waitThread3.timed_join(MILLIS(10000)));

  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == &event2);
  // event2 should have been reset.
  EXPECT_TRUE(event2.WaitMSec(1) == false);
}

TEST(TestEvent, GroupTimedWait)
{
  ThreadEvent event1;
  ThreadEvent event2;
  ThreadEventGroup group{&event1,&event2};

  bool result1 = false;
  bool result2 = false;

  waiter w1(event1,result1);
  waiter w2(event2,result2);

  thread waitThread1(w1);
  thread waitThread2(w2);

  EXPECT_TRUE(waitForWaiters(event1,1,10000));
  EXPECT_TRUE(waitForWaiters(event2,1,10000));

  EXPECT_TRUE(group.wait(20) == NULL); // waited ... got nothing

  group_wait w3(group,50);
  thread waitThread3(w3);

  EXPECT_TRUE(waitForWaiters(group,1,10000));

  EXPECT_TRUE(!result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(w3.waiting);
  EXPECT_TRUE(w3.result == NULL);

  // this should end given the wait is for only 50 millis
  EXPECT_TRUE(waitThread3.timed_join(MILLIS(200)));

  EXPECT_TRUE(!w3.waiting);
  EXPECT_TRUE(w3.result == NULL);

  group_wait w4(group,50);
  thread waitThread4(w4);

  EXPECT_TRUE(waitForWaiters(group,1,10000));

  EXPECT_TRUE(w4.waiting);
  EXPECT_TRUE(w4.result == NULL);

  event1.Set();

  EXPECT_TRUE(waitThread1.timed_join(MILLIS(10000)));
  EXPECT_TRUE(waitThread4.timed_join(MILLIS(10000)));
  SleepMillis(10);

  EXPECT_TRUE(result1);
  EXPECT_TRUE(!result2);

  EXPECT_TRUE(!w4.waiting);
  EXPECT_TRUE(w4.result == &event1);

  event2.Set();

  EXPECT_TRUE(waitThread2.timed_join(MILLIS(10000)));
}

#define TESTNUM 100000l
#define NUMTHREADS 100l

ThreadEvent* g_event = NULL;
std::atomic<long> g_mutex;

class mass_waiter : public ThreadMain
{
public:
  ThreadEvent& event;
  bool result;

  volatile bool waiting = false;

  mass_waiter() : event(*g_event) {}

  void Run() override
  {
    waiting = true;
    AtomicGuard g(&g_mutex);
    result = event.Wait();
    waiting = false;
  }
};

class poll_mass_waiter : public ThreadMain
{
public:
  ThreadEvent& event;
  bool result;

  volatile bool waiting = false;

  poll_mass_waiter() : event(*g_event) {}

  void Run() override
  {
    waiting = true;
    AtomicGuard g(&g_mutex);
    while ((result = event.WaitMSec(0)) == false);
    waiting = false;
  }
};

template <class W> void RunMassEventTest(std::vector<std::shared_ptr<W>>& m, bool canWaitOnEvent)
{
  std::vector<std::shared_ptr<thread>> t(NUMTHREADS);
  for(size_t i=0; i<NUMTHREADS; i++)
    t[i].reset(new thread(*m[i]));

  EXPECT_TRUE(waitForThread(g_mutex,NUMTHREADS,10000));
  if (canWaitOnEvent)
  {
    EXPECT_TRUE(waitForWaiters(*g_event,NUMTHREADS,10000));
  }

  SleepMillis(100);// give them a little more time

  for(size_t i=0; i<NUMTHREADS; i++)
  {
    EXPECT_TRUE(m[i]->waiting);
  }

  g_event->Set();

  for(size_t i=0; i<NUMTHREADS; i++)
  {
    EXPECT_TRUE(t[i]->timed_join(MILLIS(10000)));
  }

  for(size_t i=0; i<NUMTHREADS; i++)
  {
    EXPECT_TRUE(!m[i]->waiting);
    EXPECT_TRUE(m[i]->result);
  }
}


TEST(TestMassEvent, General)
{
  g_event = new ThreadEvent();

  std::vector<std::shared_ptr<mass_waiter>> m(NUMTHREADS);
  for(size_t i=0; i<NUMTHREADS; i++)
    m[i].reset(new mass_waiter());

  RunMassEventTest(m,true);
  delete g_event;
}

TEST(TestMassEvent, Polling)
{
  g_event = new ThreadEvent(true); // polling needs to avoid the auto-reset

  std::vector<std::shared_ptr<poll_mass_waiter>> m(NUMTHREADS);
  for(size_t i=0; i<NUMTHREADS; i++)
    m[i].reset(new poll_mass_waiter());

  RunMassEventTest(m,false);
  delete g_event;
}

