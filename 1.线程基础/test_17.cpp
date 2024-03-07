#include <iostream>
#include <string>
#include <thread>

/* 绑定引用 */
void change_param(int& param) { param++; }

void oops_ref(int someParam) {
  std::cout << "--- before change, param is: " << someParam << std::endl;
  /*
  需要显式引用转换:`std::ref()`
  因为`change_param`接收一个"左值引用"但传入的是一个"右值".
  `std::thread`的构造函数在处理参数时,会进行参数的复制,而不是引用.(具体看源码,目前看不懂
  ???)
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
