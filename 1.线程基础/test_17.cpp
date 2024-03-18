#include <iostream>
#include <string>
#include <thread>

/* 绑定引用 */
void change_param(int& param) {
  param++;
}

void oops_ref(int someParam) {
  std::cout << "--- before change, param is: " << someParam << std::endl;
  /*
  当线程回调函数需要引用接收参数时,我们都需要将传入的参数显式引用转换`std::ref()`;
  `std::thread`的构造函数在传参时,默认会将参数转为右值,默认"值拷贝"而不是引用.(具体看源码,目前看不懂???)
  */
  // std::thread t2(change_param, someParam);  //错误
  std::thread t2(change_param, std::ref(someParam));
  t2.join();
  std::cout << "--- after change, param is: " << someParam << std::endl;
}

int main() {
  /* 10.绑定引用 */
  oops_ref(10);
}
