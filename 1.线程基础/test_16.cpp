#include <iostream>
#include <string>
#include <thread>

/* 慎用隐式转换 */
void print_str(int i, const std::string& s) {
  std::cout << "--- i is " << i << " str is " << std::endl;
}

void oops_danger(int som_param) {
  char buffer[1024];
  sprintf(buffer, "%i", som_param);
  // 在线程内部将char const* 转化为std::string
  std::thread t(print_str, 3, buffer);  // 3和buffer是`print_str`的实参
  t.detach();
  std::cout << "danger oops finished " << std::endl;
}

int main() {
  /* 9.慎用隐式转换 (没听懂,先搁置 ???) */
  oops_danger(1024);
}
