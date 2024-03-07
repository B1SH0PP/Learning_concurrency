#include <iostream>
#include <string>
#include <thread>

struct func {
  int& _i;
  func(int& i) : _i(i) {}
  void operator()() {
    for (int i = 0; i < 3; i++) {
      _i = i;
      std::cout << "---[func] _i is " << _i << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

/*
异常捕获(try/catch),来防止主线程崩溃导致子线程也崩溃.
*/
void catch_exception() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread{myfunc};
  try {
    // 本线程做一些事情,可能引发崩溃
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } catch (std::exception& e) {
    functhread.join();
    throw;
  }
}

/*
线程守卫.用try/catch过于臃肿.
使用`RAII`技术,用于管理`std::thread`对象生命周期,
保证线程对象析构的时候等待线程运行结束再回收资源.
*/
class thread_guard {
 private:
  std::thread& _t;

 public:
  explicit thread_guard(std::thread& t) : _t(t) {}
  ~thread_guard() {
    if (_t.joinable()) {
      _t.join();
    }
  }
};

void auto_guard() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread t1(myfunc);
  /*
  将t1线程作为参数传递给类对象`tg`
  当`auto_guard`函数执行完后会回收局部对象`tg`,此时触发`thread_guard`的析构,析构内会判断`t1`线程是否
  可`join`,如果可以则等待其执行完毕.以起到防止资源泄露的作用.
  */
  thread_guard tg(t1);
  //...本线程做一些事情
  /*
  主线程会先执行下面这段输出,随后依然有子线程的输出`_i is
  ..`输出,代表成功守卫了子线程,让主线程 等待(join)子线程结束后才回收资源.
  */
  /*
  因为这里对象是创建在堆区上的,所以"先创建后析构",所以这里`t1`不会出现比`tg`先执行析构的场景,所以
  不用担心`t1`线程先执行完后执行析构.
  */
  std::cout << "=== auto_guard function has finished " << std::endl;
}

int main() {
  /* 7.try/catch 捕获异常 */
  // catch_exception();

  /* 8.自动守卫 */
  auto_guard();
}
