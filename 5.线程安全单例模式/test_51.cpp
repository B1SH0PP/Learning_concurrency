#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

/* 利用局部静态变量实现单例 */
/*
C++11之后的单例基本都是使用下面这种创建方式了,即利用"局部静态变量"创建单例对象.
通过调用`getInst`得到单例对象来调用,6:17~往后的没看,介绍的C++11之前是怎么实现的.
*/
class Single2 {
 private:
  Single2() {}
  Single2(const Single2&) = delete;
  Single2& operator=(Single2&) = delete;

 public:
  static Single2& getInst() {
    static Single2 single;
    return single;
  }
};

int main() {
  /* 1. */
}
