#pragma once

#include <concurrent/thread_pool_interface.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

namespace Concurrent {

  template <typename ThreadT>
  class ThreadPool final : public ThreadPoolInterface
  {
  public:

    using WorkerT = std::function<void(void)>;
    using ThreadFactoryT = std::function<ThreadT(WorkerT&&)>;

    ThreadPool();

    virtual void ScaleThreads(size_t numThreads) override;

    virtual void Stop(bool cancelTasks = true) override;

    virtual void CancelAll() override;

    virtual void Post(WorkT&& work) override;

  private:

    ThreadPool(ThreadFactoryT threadFactory);

    void ScaleThreadsInternal(size_t numThreads);

    void SpawnThread();

    void CancelAllInternal();

    void Worker(size_t threadId);

    ThreadFactoryT ThreadFactory;
    std::atomic_size_t NumThreads;
    std::vector<ThreadT> Threads;
    std::mutex ThreadsMutex;
    std::queue<WorkT> Jobs;
    std::mutex JobsMutex;
    std::condition_variable JobsAwait;
  };

}

#include <concurrent/thread_pool.tpp>
