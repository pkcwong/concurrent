#pragma once

#include <concurrent/timer_interface.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace Concurrent {

  template <typename ClockT, typename DurationT>
  class Timer : public TimerInterface<DurationT>
  {
  public:

    using TimepointT = std::chrono::time_point<ClockT, DurationT>;

    Timer(std::unique_ptr<ThreadPoolInterface> threadPool);

    virtual void Stop(bool cancelTasks = true) override;

    virtual void CancelAll() override;

    virtual void ExpiresFromNow(
      DurationT timeout,
      ThreadPoolInterface::WorkT&& work) override;

  private:

    struct Job
    {
      TimepointT ExpiryTimepoint;
      ThreadPoolInterface::WorkT Work;

      bool operator>(const Job& job) const;
    };

    void CancelAllInternal();

    void ScheduleWorker();

    void ScheduleWorker(TimepointT timepoint);

    void Worker();

    std::unique_ptr<ThreadPoolInterface> ThreadPool;
    std::atomic_bool IsStopping{false};
    std::priority_queue<Job, std::vector<Job>, std::greater<Job>> Jobs;
    std::mutex JobsMutex;
    std::condition_variable JobsAwait;
  };

}

#include <concurrent/timer.tpp>
