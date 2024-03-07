#include <iostream>
#include <string>
#include <thread>

int main() {
  std::string helloStr = "hello world!";
  
  /* 4.lambda表达式作为线程函数 */
  std::thread t3(
      [](std::string str) { std::cout << "--- str is " << str << std::endl; },
      helloStr);
  t3.join();
}