# concurrent

A concurrency library implementing `ThreadPool` and `Timer`.

Check out `/example` for sample usages.

## Thread Pool

`ThreadPool` manages a pool of threads which can be used to execute posted jobs.

[Sample Code](https://github.com/pkcwong/concurrent/blob/master/example/thread_pool_example.cpp).

### Constructing a Thread Pool

This library provides `SimpleThreadPool` for easy usage. Set the number of threads in the Thread Pool by `ScaleThreads`.

```cpp
#include <concurrent/simple_thread_pool>

auto threadPool = std::make_unique<Concurrent::SimpleThreadPool>();
threadPool->ScaleThreads(4); // spawns a total of 4 threads
```

### Posting a job

Post a job to the Thread Pool via `Post`. The job will be placed in queue, and picked up by the next available worker thread. A job is a `std::function` or lambda that will be executed in the Thread Pool.

```cpp
threadPool->Post(
  [] ()
  {
    HeavyWork();
  });
```

### Stopping a Thread Pool

A thread pool must be stopped before destructing. By default, prevents worker threads from processing next jobs in queue and exit as early as possible.

```cpp
// stops the thread pool and joining threads on next opportunity
threadPool->Stop();
```

## Timer

`Timer` manages a thread which executes posted jobs after a specified delay.

[Sample Code](https://github.com/pkcwong/concurrent/blob/master/example/timer_example.cpp).

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
    WorkExecutedAfterTimeout();
  });
```

### Stopping a timer

A timer must be stopped before destructing. By default, prevents worker threads from processing next jobs in queue.

```cpp
// stops the timer and joining thread on next opportunity
timer->Stop();
```
