#include "test_76.hpp"

#include <iostream>

void threadFunc(int& m) {
  m = 1024;
  std::cout << "threadFunc is running!" << std::endl;
}

int main() {
  int m = 0;

  // 示例一:`m`不会被同步修改
  // ThreadPool::instance().commit([](int& m) {  // 使用`commit`投递任务
  //   m = 1024;
  //   std::cout << "inner thread's m is: " << m << std::endl;
  //   std::cout << "m address: " << &m << std::endl; },
  //                               m);
  // std::this_thread::sleep_for(std::chrono::seconds(3));
  // std::cout << "main thread's m is: " << m << std::endl;
  // std::cout << "m address: " << &m << std::endl;

  // 示例二:`m`会被同步修改
  // ThreadPool::instance().commit([](int& m) {
  //   m = 1024;
  //   std::cout << "inner thread's m is: " << m << std::endl;
  //   std::cout << "m address: " << &m << std::endl; },
  //                               std::ref(m));
  // std::this_thread::sleep_for(std::chrono::seconds(3));
  // std::cout << "main thread's m is: " << m << std::endl;
  // std::cout << "m address: " << &m << std::endl;

  // 示例三:
  ThreadPool::instance().commit(threadFunc, std::ref(m));
  std::cout << "main thread's m is: " << m << std::endl;
  return 0;
}