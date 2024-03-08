#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

/* 识别线程 */
void getID() {
  std::thread t([]() {
    std::cout << "--- this thread id is: " << std::this_thread::get_id()
              << std::endl;
  });
  t.join();
}

int main() {
  /* 5.识别线程 */
  // 主线程id
  std::cout << "--- this thread id is: " << std::this_thread::get_id()
            << std::endl;
  // 子线程id
  getID();
}