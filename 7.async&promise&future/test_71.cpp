#include <future>
#include <iostream>
#include <mutex>
#include <string>

/* async的用法 */
// 定义一个异步任务
std::string fetchDataFromDB(std::string query) {
  // 模拟异步任务,从数据库取数据
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return query;
}

void use_async() {
  /*
  @brief:调用`std::async`单独开一个线程去执行`fetchDataFromDB`函数
  @param-std::launch::async:异步线程启动策略,有2种选择: {async, deferred}
                            async:立刻执行线程函数;
                            deferred:调用`std::future::get()`时再执行,即需要结果时再执行;
      `std::launch::async|std::launch::deferred`:上面两种的组合,按CPU是否繁忙等实际资源条件判断是否立刻执行,让系统判断.
      `std::async`默认使用`std::launch::async|std::launch::deferred`,即交由编译器判断;
  @param-"Hello World!":线程函数实参;
  @return:std::future<T>:代表了一个将来可能会获得结果的值,它用于守候并操纵何时获取该值.
  */
  std::future<std::string> resultFromDB = std::async(std::launch::async, fetchDataFromDB, "Hello World!");

  // 主线程中继续执行其他事
  std::cout << "do something else ..." << std::endl;

  /*
  @param-std::future::get:获取`std::future`对象的数值或异常.
      如果异步任务没有完成,`get`会阻塞当前线程直至线程函数完成;
      `get`只能调用一次,因为其会移动`std::future`对象值;
  @param-std::future::wait:不返回异步函数的结果,只是使当前线程等待线程函数完成,如果完成了立刻返回.
      `std::future::wait`可以多次调用,不会消耗`std::future`的状态;
  */
  std::string data = resultFromDB.get();
  std::cout << "get the data is: " << data << std::endl;
}

int main() {
  /* 1.async的用法 */
  use_async();
}
