#pragma once

#include <concurrent/thread_pool.h>

namespace Concurrent {

  template <typename ThreadT>
  ThreadPool<ThreadT>::ThreadPool()
    : ThreadFactory{
        [] (auto&& worker)
        {
          return ThreadT{std::move(worker)};
        }}
  {
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::ScaleThreads(size_t numThreads)
  {
    std::unique_lock<std::mutex> lock{ThreadsMutex};
    ScaleThreadsInternal(numThreads);
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::Stop(bool cancelTasks)
  {
    std::unique_lock<std::mutex> threadsLock{ThreadsMutex};

    {
      std::unique_lock<std::mutex> jobsLock{JobsMutex};

      if (cancelTasks)
        CancelAllInternal();

      JobsAwait.wait(
        jobsLock,
        [this] ()
        {
          return Jobs.empty();
        });
    }

    ScaleThreadsInternal(0);
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::CancelAll()
  {
    std::unique_lock<std::mutex> jobsLock{JobsMutex};
    CancelAllInternal();
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::Post(WorkT&& work)
  {
    std::unique_lock<std::mutex> lock{JobsMutex};
    Jobs.emplace(std::move(work));

    JobsAwait.notify_one();
  }

  template <typename ThreadT>
  ThreadPool<ThreadT>::ThreadPool(ThreadFactoryT threadFactory)
    : ThreadFactory{std::move(threadFactory)}
  {
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::SpawnThread()
  {
    Threads.emplace_back(
      ThreadFactory(
        [this, threadId = Threads.size()] ()
        {
          Worker(threadId);
        }));
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::CancelAllInternal()
  {
    while (!Jobs.empty())
      Jobs.pop();
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::ScaleThreadsInternal(size_t numThreads)
  {
    NumThreads = numThreads;

    while (Threads.size() < NumThreads)
      SpawnThread();

    JobsAwait.notify_all();

    while (Threads.size() > NumThreads)
      Threads.pop_back();
  }

  template <typename ThreadT>
  void ThreadPool<ThreadT>::Worker(size_t threadId)
  {
    auto shouldTerminate =
      [this, &threadId] ()
      {
        return threadId >= NumThreads;
      };

    while (!shouldTerminate())
    {
      WorkT work;

      {
        std::unique_lock<std::mutex> lock{JobsMutex};
        JobsAwait.wait(
          lock,
          [this, &shouldTerminate, &work] ()
          {
            auto shouldExit = shouldTerminate();
            if (!shouldExit && !Jobs.empty())
            {
              work = std::move(Jobs.front());
              Jobs.pop();
              JobsAwait.notify_all();
            }
            return shouldExit || work;
          });
      }

      if (work)
        work();
    }
  }

}
