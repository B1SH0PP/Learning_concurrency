#include <iostream>
#include <string>
#include <thread>

/* 绑定类成员函数 */
class X {
 public:
  void do_lengthy_work() { std::cout << "--- do lengthy work" << std::endl; }
};

void oops_bind_classfunc() {
  X myX;
  std::thread t(&X::do_lengthy_work, &myX);
  // 如果`do_lengthy_work`有参数:
  // std::thread t(&X::do_lengthy_work, &myX, [param1],...);
  t.join();
}

int main() {
  /* 11.线程绑定类的成员函数 */
  oops_bind_classfunc();
}
