#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

/* `std::packaged_task`&`std::future`的用法 */
int Task() {
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "--- Task function run 5s" << std::endl;
  return 1024;
}

/*
`std::packaged_task`相较`std::async`的好处:
    1.用`std::packaged_task`封装的任务对象可以多次调用(可复用).
      `std::async`适用一次性的异步任务.
    2.用`std::packaged_task`封装灵活性更好.可以将封装的任务指定线程对象`std::thread t1`,可自定义执行和回收.
*/
void use_packaged_task() {
  // 1.创建一个包装了`Task`任务(函数)的`std::packaged_task`对象`pt`
  /*
  @param-<int()>:`<>`内传入函数签名;`int`代表返回类型;`()`代表参数类型(ie:(int,int));
  */
  std::packaged_task<int()> pt(Task);

  // 2.创建一个`std::future`对象来接收异步任务未来的结果
  std::future<int> result = pt.get_future();

  // 3.在另一个线程上执行任务
  /*
  `std::future`对象没有拷贝构造/赋值(源码都用的右值引用,确实不支持拷贝),所以这里用`std::move`调用移动操作
  */
  std::thread t1(std::move(pt));
  t1.detach();  // 线程分离(即线程的执行与`t1`对象解耦,即使`t1`被释放线程也不会受影响)

  /*
  `std::packaged_task`内部重载了`()`运算符,所以当其绑定好了异步任务后,可以直接通过`()`当做仿函数来调用.
  */
  // pt();

  // 4.等待任务完成获取结果
  int value = result.get();
  std::cout << "--- the result is: " << value << std::endl;
}

int main() {
  /* 2.`std::packaged_task`&`std::future`的用法 */
  use_packaged_task();
}
