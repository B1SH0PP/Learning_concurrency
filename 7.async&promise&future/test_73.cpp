#include <future>
#include <iostream>
#include <string>

/* `std::promise`&`std::future`的用法 */

/* 1.设置值 */
// 线程回调函数,接收一个`std::promise`对象
void set_value(std::promise<int> prom) {
  // 设置promise的值
  /*
  `std::promise`的好处是不用等异步任务执行完(return)才能接收到返回值,而是直接将运行的结果绑定给外部
  传入的`std::promise`对象,这样的话外部直接通过这个`promise`对象
  这里`std::promise`对象绑定的`int`型数据,所以假设设为10;
  */
  prom.set_value(10);
  std::cout << "--- promise set value success" << std::endl;

  // 通过睡眠模拟子线程后续还有任务在执行,运行时也能体现子线程没结束时就能拿到内部的运行结果
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "--- subthread has finished !!!" << std::endl;
}

void use_set_value() {
  // 创建`promise`对象
  std::promise<int> prom;

  // 创建`std::future`对象来接收异步任务未来的结果
  std::future<int> fut = prom.get_future();

  // 在新线程中绑定`promise`的值
  /*
  `std::promise`对象和`std::packaged_task`一样没有拷贝构造/赋值(源码都用的右值引用,确实不支持拷贝),
  所以这里用`std::move`调用移动操作
  */
  std::thread t1(set_value, std::move(prom));
  std::cout << "--- waiting for the thread to set the value ..." << std::endl;
  /*
  `fut.get`会阻塞主线程,直到线程函数中通过`set_value`设置了`promise`的值.
  但`get`并不会阻塞等待子线程全部执行完,一旦`promise`的值被设置，fut.get()就会返回结果，并且子线程可能还在继续执行
  */
  std::cout << "--- value set by the thread: " << fut.get() << std::endl;

  //`join`是因为子线程虽然传出了值但是后续可能还有任务没执行完,所以要join起来.
  t1.join();
}

/* 2.设置异常 */
void set_exception(std::promise<void> prom) {  // 没有默认,必须显式指定void
  try {
    throw std::runtime_error("an error occurred !");
  } catch (...) {
    // 设置异常状态
    prom.set_exception(std::current_exception());
  }
}

void use_set_exception() {
  std::promise<void> prom;

  // 创建`std::future`对象来接收异步任务未来的结果
  std::future<void> fut = prom.get_future();

  std::thread t1(set_exception, std::move(prom));  //`std::promise`对象没有拷贝构造/赋值

  // 获取`future`接收的异常
  /*
  如果子线程中使用`std::promise`传出异常,主线程中必须用`try|catch`捕获,不然会崩溃.
  */
  try {
    std::cout << "--- waiting for the thread to set the exception...\n";
    fut.get();
  } catch (const std::exception& e) {
    std::cout << "--- exception set by the thread: " << e.what() << '\n';
  }
  t1.join();
}

/* 使用`promise`的注意点,这样不会报错 */
void use_promise_notice() {
  std::thread t1;
  std::future<int> fut;
  {
    std::promise<int> prom;
    fut = prom.get_future();
    t1 = std::thread(set_value, std::move(prom));  //使用`move`转移
  }
  /*
  问题:
  即使局部作用域结束后`promise`对象会被回收,但是因为我们使用了`std::move`转移了其所有权到线程参数里,
  所以当我们使用与其绑定的`future`对象也不会出错.
  */
  std::cout << "--- waiting for the thread to set the value ..." << std::endl;
  std::cout << "--- value set by the thread t1: " << fut.get() << std::endl;
  t1.join();
}

int main() {
  /* 3.`std::promise`&`std::future`的用法 */
  // 3.1
  //  use_set_value();

  // 3.2
  //  use_set_exception();

  // 3.3
  use_promise_notice();
}
