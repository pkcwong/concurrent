add_executable(
  thread_pool_example
    thread_pool_example.cpp
)

target_include_directories(
  thread_pool_example
    PUBLIC
      ${concurrent_includes}
)

add_executable(
  timer_example
    timer_example.cpp
)

target_include_directories(
  timer_example
    PUBLIC
      ${concurrent_includes}
)
