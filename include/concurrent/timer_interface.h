#pragma once

#include <concurrent/thread_pool_interface.h>
#include <functional>

namespace Concurrent {

  template <typename DurationT>
  class TimerInterface
  {
  public:

    virtual ~TimerInterface() = default;

    /**
     * @brief Stops the timer.
     * @param cancelTasks should cancel all tasks
     */
    virtual void Stop(bool cancelTasks = true) = 0;

    /**
     * @brief Cancels all tasks.
     */
    virtual void CancelAll() = 0;

    /**
     * @brief Posts a job to be executed after timeout.
     * @param timeout expiry timeout
     * @param work job
     */
    virtual void ExpiresFromNow(
      DurationT timeout,
      ThreadPoolInterface::WorkT&& work) = 0;
  };

}
