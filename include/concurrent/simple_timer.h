#pragma once

#include <concurrent/timer.h>

namespace Concurrent {

  using SimpleTimer = Timer<std::chrono::system_clock, std::chrono::nanoseconds>;

}
