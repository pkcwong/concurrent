#include <concurrent/simple_thread_pool.h>
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
  std::unique_ptr<Concurrent::ThreadPoolInterface> threadPool = std::make_unique<Concurrent::SimpleThreadPool>();

  threadPool->ScaleThreads(2);

  threadPool->Post(
    [] ()
    {
      std::this_thread::sleep_for(std::chrono::seconds{2});
      std::cout << "Hello World 2!" << std::endl;
    });

  threadPool->Post(
    [] ()
    {
      std::this_thread::sleep_for(std::chrono::seconds{1});
      std::cout << "Hello World 1!" << std::endl;
    });

  threadPool->Post(
    [] ()
    {
      std::this_thread::sleep_for(std::chrono::seconds{0});
      std::cout << "Hello World 0!" << std::endl;
    });

  threadPool->Stop(false);
}
