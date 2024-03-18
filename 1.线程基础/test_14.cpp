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

struct func_sp {  // smart pointer
  std::shared_ptr<int> _sp;
  func_sp(std::shared_ptr<int> sp) : _sp(sp) {}
  void operator()() {
    for (int i = 0; i < 3; i++) {
      *_sp = i;
      std::cout << "---[func_sp] _sp is " << *_sp << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
};

void oops() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread(myfunc);
  /*
  将线程与`functhread`对象分离,即将线程的执行与对象`functhread`解耦,即使`functhread`被回收也不会影响线程执行.
  这里有一个隐患!
    线程函数`myfunc`(即其仿函数)内部的`_i`是`someLocalState`的引用,且`_i=1`会修改,所以
    线程函数一直会用到`someLocalState`,但是当`oops`执行完后局部变量`someLocalState`会
    被回收,导致意外发生.
  */
  functhread.detach();
}

void oops_sp() {
  auto someLocalState = std::make_shared<int>(0);
  func_sp myfunc(someLocalState);
  std::thread functhread(myfunc);
  functhread.detach();
}

void oops_usejoin() {
  int someLocalState = 0;
  func myfunc(someLocalState);
  std::thread functhread(myfunc);
  /*
  这里将原本detach的线程改为join,让主线程等待子线程结束.
  */
  functhread.join();
}

int main() {
  /* 5.detach的注意事项 */
  // oops();
  // std::this_thread::sleep_for(std::chrono::seconds(1));

  // 方法一:使用智能指针包装变量(没生效还是有问题,先搁置 ???)
  // oops_sp();

  /* 6.join用法 */
  oops_usejoin();
}