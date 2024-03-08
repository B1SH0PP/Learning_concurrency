#include <iostream>
#include <string>
#include <thread>

/* 线程对象重复绑定的危险性 */
void someFunction() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void someOtherFunction() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void dangerous_use() {
  //`T1`绑定`someFunction`
  std::thread T1(someFunction);
  // 将`T1`管理的线程转移给`T2`,转移后`T1`无效
  std::thread T2 = std::move(T1);
  // 此时`T1`并没有回收,所以可以给`T1`赋其他线程
  /*
  使用`std::thread()`函数接收一个可调用对象(线程函数),
  返回一个局部变量,因为是函数的返回值所以肯定是个右值
  */
  T1 = std::thread(someOtherFunction);
  // 创建线程T3
  std::thread T3;
  // 转移`T2`给`T3`
  T3 = std::move(T2);
  // 转移`T3`给`T1`(引发崩溃)
  T1 = std::move(T3);

  // 睡2000s依然立刻就崩溃证明是第上步导致的崩溃
  std::this_thread::sleep_for(std::chrono::seconds(2000));  // s
}

/* 返回一个线程 */
std::thread f() {
  //内部会优先寻找这个类的拷贝构造函数,如果没有就会使用这个类的移动构造函数
  return std::thread(someFunction);
}

void param_function(int a) {
  while (true) {
    std::cout << "--- param is " << a << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

std::thread g() {
  std::thread t(param_function, 1024);
  return t;
}

int main() {
  /* 1.线程归属权 */
  // dangerous_use();

  /* 返回一个线程 */
  std::thread T1 = f();
  std::thread T2 = g();
  T1.join();
  T2.join();
}