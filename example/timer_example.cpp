#include <concurrent/simple_thread_pool.h>
#include <concurrent/simple_timer.h>
#include <iostream>
#include <future>

int main(int argc, char* argv[])
{
  auto threadPool = std::make_unique<Concurrent::SimpleThreadPool>();
  auto timer = std::make_unique<Concurrent::SimpleTimer>(std::move(threadPool));

  timer->ExpiresFromNow(
    std::chrono::seconds{2},
    [] ()
    {
      std::cout << "Hello World 2!" << std::endl;
    });

  timer->ExpiresFromNow(
    std::chrono::seconds{1},
    [] ()
    {
      std::cout << "Hello World 1!" << std::endl;
    });

  timer->ExpiresFromNow(
    std::chrono::seconds{0},
    [] ()
    {
      std::cout << "Hello World 0!" << std::endl;
    });

  timer->Stop(false);
}
