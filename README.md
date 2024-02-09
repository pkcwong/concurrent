# concurrent

A concurrency library implementing `ThreadPool` and `Timer`.

## Thread Pool

`ThreadPool` manages a pool of threads which can be used to execute posted jobs.

### Constructing a Thread Pool

This library provides `SimpleThreadPool` for easy usage.

```cpp
#include <concurrent/simple_thread_pool>

auto threadPool = std::make_unique<Concurrent::SimpleThreadPool>();
```

`SimpleThreadPool` spawns `std::thread` internally.

A more generic class `ThreadPool` is also available for integration with custom `Thread` implementation. Simply implement a thread wrapper class with destructor invoking `join` on thread.

Sample code to implement custom Thread Pool with `boost::thread`.

```cpp
#include <concurrent/thread_pool.h>

class BoostThread
{
public:

  template <typename... ArgsT>
  BoostThread(ArgsT&&... args)
    : Worker{std::forward<ArgsT>(args)...}
  {
  }

  BoostThread(BoostThread&&) noexcept = default;

  ~BoostThread()
  {
    if (Worker.joinable())
      Worker.join();
  }

private:

  boost::thread Worker;
};

using CustomThreadPool = ThreadPool<BoostThread>;
```

### Scaling number of threads

Set the number of threads in the Thread Pool by `ScaleThreads`. Should be called after constructing a Thread Pool, else no threads will be spawned.

```cpp
threadPool->ScaleThreads(4); // spawns a total of 4 threads
```

`ScaleThreads` may be called by any thread running **outside** of the thread pool. The function blocks until threads are either spawned or joined to match target number of threads.

### Posting a job

Post a job to the Thread Pool via `Post`. The job will be placed in queue, and picked up by the next available thread. A job is a `std::function` or lambda that will be executed in the Thread Pool.

```cpp
threadPool->Post(
  [] ()
  {
    // work
  });
```

To retrieve return result of the job, use `std::promise` or `std::condition_variable` or other async handling methods.

### Stopping a Thread Pool

A thread pool must be stopped before destructing. By default, prevents worker threads from processing next jobs in queue. Invoke `Stop` with parameter set to `false` in order to process all tasks in queue before exiting.

```cpp
// stops the thread pool and joining threads on next opportunity
threadPool->Stop();
```
```cpp
// stops the thread pool after executing all jobs in queue
threadPool->Stop(false);
```

## Timer

`Timer` manages a thread which executes posted jobs after a specified delay.

### Constructing a Timer

This library provides `SimpleTimer` for easy usage. `Timer` makes use of `ThreadPool` to executing jobs and performing timer countdowns.

```cpp
#include <concurrent/simple_timer.h>

auto threadPool = std::make_unique<Concurrent::SimpleThreadPool>();
auto timer = std::make_unique<Concurrent::SimpleTimer>(std::move(threadPool));
```

### Posting a job

Post a job by invoking `ExpiresFromNow`, it accepts a `std::chrono::duration` for the delay which to execute the job.

```cpp
timer->ExpiresFromNow(
  std::chrono::seconds{5},
  [] ()
  {
    // work
  });
```

### Stopping a timer

A timer must be stopped before destructing.

By default, prevents worker threads from processing next jobs in queue. Invoke `Stop` with parameter set to `false` in order to process all tasks in queue before exiting.

```cpp
// stops the timer and joining thread on next opportunity
timer->Stop();
```
```cpp
// stops the timer after executing all jobs in queue
timer->Stop(false);
```
