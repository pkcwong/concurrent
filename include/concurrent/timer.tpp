#pragma once

#include <concurrent/timer.h>

namespace Concurrent {

  template <typename ClockT, typename DurationT>
  Timer<ClockT, DurationT>::Timer(std::unique_ptr<ThreadPoolInterface> threadPool)
    : ThreadPool{std::move(threadPool)}
  {
    ThreadPool->ScaleThreads(1);
    ScheduleWorker();
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::Stop(bool cancelTasks)
  {
    bool expectedStoppingState{false};
    if (!IsStopping.compare_exchange_strong(expectedStoppingState, true))
      return;

    if (cancelTasks)
    {
      std::unique_lock<std::mutex> lock{JobsMutex};
      CancelAllInternal();
    }

    JobsAwait.notify_all();

    {
      std::unique_lock<std::mutex> lock{JobsMutex};
      JobsAwait.wait(
        lock,
        [this] ()
        {
          return Jobs.empty();
        });
    }

    ThreadPool->Stop(false);
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::CancelAll()
  {
    std::unique_lock<std::mutex> lock{JobsMutex};
    CancelAllInternal();

    JobsAwait.notify_all();
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::ExpiresFromNow(
    DurationT timeout,
    ThreadPoolInterface::WorkT&& work)
  {
    std::unique_lock<std::mutex> lock{JobsMutex};

    Job job{
      ClockT::now() + timeout,
      std::move(work)};

    Jobs.push(std::move(job));

    JobsAwait.notify_all();
  }

  template <typename ClockT, typename DurationT>
  bool Timer<ClockT, DurationT>::Job::operator>(const Job& job) const
  {
    return ExpiryTimepoint > job.ExpiryTimepoint;
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::CancelAllInternal()
  {
    while (!Jobs.empty())
      Jobs.pop();
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::ScheduleWorker()
  {
    ThreadPool->Post(
      [this] ()
      {
        {
          std::unique_lock<std::mutex> lock{JobsMutex};
          JobsAwait.wait(
            lock,
            [this] ()
            {
              return IsStopping || !Jobs.empty();
            });
        }

        Worker();
      });
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::ScheduleWorker(TimepointT timepoint)
  {
    ThreadPool->Post(
      [this, timepoint] ()
      {
        {
          std::unique_lock<std::mutex> lock{JobsMutex};
          JobsAwait.wait_until(
            lock,
            timepoint,
            [this] ()
            {
              return Jobs.empty();
            });
        }

        Worker();
      });
  }

  template <typename ClockT, typename DurationT>
  void Timer<ClockT, DurationT>::Worker()
  {
    std::unique_lock<std::mutex> lock{JobsMutex};
    auto now = ClockT::now();

    while (!Jobs.empty() && Jobs.top().ExpiryTimepoint <= now)
    {
      Jobs.top().Work();
      Jobs.pop();

      JobsAwait.notify_all();
    }

    if (Jobs.empty())
    {
      if (!IsStopping)
        ScheduleWorker();

      return;
    }

    ScheduleWorker(Jobs.top().ExpiryTimepoint);
  }

}
