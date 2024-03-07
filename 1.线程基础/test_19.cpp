#include <iostream>
#include <string>
#include <thread>

/* 线程函数参数是unique类型 */
void deal_unique(std::unique_ptr<int> p) {
  std::cout << "--- unique ptr is: " << *p << std::endl;
  (*p)++;
  std::cout << "--- after unique ptr is: " << *p << std::endl;
}

void oops_move() {
  auto p = std::make_unique<int>(100);
  /*
  `std::thread`会对传入的参数进行"拷贝"或"移动",所以当传入`std::unique_ptr
  p`时,`p`会被拷贝到 内部线程中,又因`unique_ptr`不可拷贝,所以会出错.
  这时必须要用`std::move`标记进行移动操作.
  */
  std::thread t(deal_unique, std::move(p));
  t.join();
  //`move`后就不能再使用`p`了,已被`move`废弃(无法输出了p了)
  std::cout << "after unique data is " << *p << std::endl;
}

int main() {
  /* 12.线程函数参数是unique类型 */
  oops_move();
}
