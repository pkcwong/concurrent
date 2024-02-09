#pragma once

#include <concurrent/thread.h>
#include <concurrent/thread_pool.h>

namespace Concurrent {

  using SimpleThreadPool = ThreadPool<Thread>;

}
