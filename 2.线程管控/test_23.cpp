#include <iostream>
#include <string>
#include <thread>
#include <vector>

void param_function(int a) {
  while (true) {
    std::cout << "--- param is " << a << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

/*
当我们想将线程对象插入vec容器时,不能用`push_back`其底层会调用拷贝或移动构造,所以应使用`emplace_back`它只
会调用构造函数.
*/
void use_vector() {
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < 3; ++i) {
    // threads.push_back(param_function, i);  //错误
    threads.emplace_back(param_function, i);
  }
  for (auto& entry : threads) {
    entry.join();
  }
}

int main() {
  /* 3.容器存储 */
  use_vector();
}