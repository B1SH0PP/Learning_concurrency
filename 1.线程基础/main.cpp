#include <iostream>
#include <string>
#include <thread>

void thread_work_1(std::string str) {
  std::cout << "--- str is: " << str << std::endl;
}

class backgroundTask {
 public:
  void operator()() { std::cout << "--- backgroundTask called" << std::endl; }
};

struct func {
  int& _i;
  func(int& i) : _i(i) {}
  void operator()() {
    for (int i = 0; i < 3; i++) {
      _i = 1;
      std::cout << "_i is " << _i << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

void oops() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread(myfunc);
  /*
  这里将线程`functhread`分离(独立于主线程运行),这里有一个隐患!
  线程函数`myfunc`(即其仿函数)内部的`_i`是`someLocalState`的引用,且`_i = 1`会修改,所以线程函数一直会
  用到`someLocalState`,但是当`oops`执行完后局部变量`someLocalState`会被回收,导致意外发生.
  */
  functhread.detach();
}

// void move_oops() {
//   auto p = std::make_unique<int>(100);
//   std::thread t(deal_unique, std::move(p));
//   t.join();
// }

int main() {
  std::string helloStr = "hello world!";
  /* 1.通过()创建一个线程`t1`,并初始化线程函数 */
  std::thread t1(thread_work_1, helloStr);
  /*
  试图让主线程睡眠来等待是无效的,因为线程有默认优化:
  当主线程结束时,就会调用子线程的析构函数,析构函数内部会调用`terminate`函数,这会触发`assert`断言导致崩溃.
  */
  // std::this_thread::sleep_for(std::chrono::seconds(1));  //睡眠1s

  /* 2.使用`join()`让主线程等待子线程结束 */
  t1.join();

  /* 3.仿函数作为线程函数 */
  /*
  这里传入仿函数对象是错误的.
  会被认为`t21`是函数对象,`std::thread`被解释为返回值,`backgroundTask()`看做函数指针,`backgroundTask`被解释为
  返回值,参数void, 如下:
  std::thread (*)(backgroundTask (*)())
  */
  // std::thread t21(backgroundTask());
  // t21.join();

  // 方法一:多加一层()
  std::thread t22((backgroundTask()));
  t22.join();

  // 方法二:改用{},代表初始化
  std::thread t23{backgroundTask()};
  t23.join();

  // 方法三:因为类下重载了`()`定义了仿函数,所以可以直接传入其对象
  backgroundTask bgT;
  std::thread t24(bgT);
  t24.join();

  /* 4.lambda表达式作为线程函数 */
  std::thread t3(
      [](std::string str) { std::cout << "--- str is" << str << std::endl; },
      helloStr);
  t3.join();

  /* 5.detach的注意事项 */
  oops();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}