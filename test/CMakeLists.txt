cmake_minimum_required(VERSION 3.2)


set(This Tester)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

set(Sources
  my_archive_test.cc
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
  gtest_main
  gmock_main
)

add_test(
  NAME ${This}
  COMMAND ${This}
)