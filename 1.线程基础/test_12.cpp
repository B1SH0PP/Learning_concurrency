#include <iostream>
#include <string>
#include <thread>

class backgroundTask {
 public:
  void operator()() { std::cout << "--- backgroundTask called" << std::endl; }
};

int main() {
  /* 3.仿函数作为线程函数 */
  /*
  下面这样传入仿函数线程函数是错误的.
  会被认为`t21`是函数对象,`std::thread`被解释为返回值,`backgroundTask()`看做函数指针,`backgroundTask`被解释为
  返回值,参数void, 如下:
  std::thread (*)(backgroundTask (*)())
  */
  // std::thread t21(backgroundTask());
  // t21.join();

  // 方法一:多加一层`()`
  std::thread t22((backgroundTask()));
  t22.join();

  // 方法二:改用`{}`,代表初始化
  std::thread t23{backgroundTask()};
  t23.join();

  // 方法三:因为类下重载了`()`定义了仿函数,所以可以创建类对象来使用
  backgroundTask bgT;
  std::thread t24(bgT);  //直接传入对象,自动判断使用的类下仿函数
  t24.join();
}