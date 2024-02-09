#pragma once

#include <functional>

namespace Concurrent {

  class ThreadPoolInterface
  {
  public:

    using WorkT = std::function<void(void)>;

    virtual ~ThreadPoolInterface() = default;

    /**
     * @brief Scales the thread pool to a number of threads. Blocks until scaling is complete. Must be called outside of thread pool threads.
     * @param numThreads target number of threads
     */
    virtual void ScaleThreads(size_t numThreads) = 0;

    /**
     * @brief Stops the thread pool. Blocks until all threads are joined.
     * @param cancelTasks should cancel all tasks
     */
    virtual void Stop(bool cancelTasks = true) = 0;

    /**
     * @brief Cancels all tasks.
     */
    virtual void CancelAll() = 0;

    /**
     * @brief Posts a job to be executed.
     * @param work job
     */
    virtual void Post(WorkT&& work) = 0;
  };

}
