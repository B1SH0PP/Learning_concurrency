#include <future>
#include <iostream>
#include <string>
#include <thread>

/* `std::shared_future`的用法 */
// 适用有多个线程等待(共享)同一个异步任务的执行结果.

void myFunction(std::promise<int>&& promise) {
  // 模拟一些工作
  std::this_thread::sleep_for(std::chrono::seconds(1));
  promise.set_value(1024);
}

void threadFunction(std::shared_future<int> future) {
  try {
    int result = future.get();
    std::cout << "result is:" << result << std::endl;
  } catch (const std::future_error& e) {
    std::cout << "future error is: " << e.what() << std::endl;
  }
}

void use_shared_future() {
  std::promise<int> promise;
  std::shared_future<int> sharedFuture = promise.get_future();

  /*
  `thread_1`将要传出的结果`1024`绑定给`std::promise`对象,之后,线程`thread_2`和`thread_3`都可以通过
  `future.get()`访问到结果.
  区别于普通的`std::future`其获取结果后就不能再次调用`future.get()`重复获取结果.
  */
  std::thread thread_1(myFunction, std::move(promise));
  /*
  `std::shared_future`不同于`std::future`是支持拷贝操作的,所以下面可以直接传入,不用`std::move`
  */
  std::thread thread_2(threadFunction, sharedFuture);
  std::thread thread_3(threadFunction, sharedFuture);

  thread_1.join();
  thread_2.join();
  thread_3.join();
}

//错误:
void use_shared_future_error() {
  std::promise<int> promise;
  std::shared_future<int> sharedFuture = promise.get_future();

  std::thread thread_1(myFunction, std::move(promise));
  /*
  如果这里采用`std::move`,第一个线程(`thread_2`)没问题,但是第二次(`thread_3`)再使用时,因为`sharedFuture`
  已被移动,状态为空了.
  */
  std::thread thread_2(threadFunction, std::move(sharedFuture));
  std::thread thread_3(threadFunction, std::move(sharedFuture));

  thread_1.join();
  thread_2.join();
  thread_3.join();
}

int main() {
  /* 4.`std::shared_future`的用法 */

  // 1.
  // use_shared_future();

  // 2.
  use_shared_future_error();  // 结果:future error is: no state
}
