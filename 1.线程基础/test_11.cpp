#include <iostream>
#include <string>
#include <thread>

void thread_work_1(std::string str) {
  std::cout << "--- str is: " << str << std::endl;
}

int main() {
  std::string helloStr = "hello world!";
  /* 1.通过()创建一个线程`t1`,并初始化线程函数 */
  std::thread t10(thread_work_1, helloStr);
  // 普通函数前面加不加`&`都行,因为函数名会退化为地址
  std::thread t11(&thread_work_1, helloStr);

  /*
  试图让主线程睡眠来等待是无效的,因为线程有默认优化:
  当主线程结束时,就会调用子线程的析构函数,析构函数内部会调用`terminate`函数,这会触发`assert`断言导致崩溃.
  */
  // std::this_thread::sleep_for(std::chrono::seconds(1));  //睡眠1s

  /* 2.使用`join()`让主线程等待子线程结束 */
  t10.join();
  t11.join();
}
