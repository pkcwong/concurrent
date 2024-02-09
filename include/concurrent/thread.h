#pragma once

#include <thread>

namespace Concurrent {

  class Thread
  {
  public:

    template <typename... ArgsT>
    Thread(ArgsT&&... args)
      : Worker{std::forward<ArgsT>(args)...}
    {
    }

    Thread(Thread&&) noexcept = default;

    ~Thread()
    {
      if (Worker.joinable())
        Worker.join();
    }

  private:

    std::thread Worker;
  };

}
