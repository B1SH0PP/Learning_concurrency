#include <future>
#include <iostream>
#include <string>
#include <thread>

/* 异常处理 */
void may_throw() {
  throw std::runtime_error("oops! something went wrong ...");
}

void use_future_exception() {
  /*
  之前展示了`promise`设置并传出异常,这里展示`std::async`异常也可以用同样的`try|catch`方式捕获.
  */
  // std::future<void> result = std::async(std::launch::async, may_throw);  //一样
  std::future<void> result(std::async(std::launch::async, may_throw));

  try {
    result.get();
  } catch (const std::exception& e) {
    std::cout << "caught exception: " << e.what() << std::endl;
  }
}

int main() {
  /* 5.异常处理 */
  use_future_exception();
}
