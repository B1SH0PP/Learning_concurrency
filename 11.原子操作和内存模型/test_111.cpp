#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/* 自旋锁 */

class Spinlock {
  /*
  这里`lock`和`unlock`的内存序选择`std::memory_order_acquire`和`std::memory_order_release`配合使用,能达到一种
  "同步"效果.
  */

 public:
  void lock() {
    /*
    `std::atomic_flag`的`test_and_set`成员函数是一个原子操作,他会先检查`atomic_flag`对象当前的状态是否被设置过:
        * 如果没被设置过(比如初始状态或者清除后),则会将`std::atomic_flag`当前的状态设置为`true`,并返回`false`;
          代表之前没上锁,`test_and_set`后`atomic_flag`置为`true`,并返回`false`,退出`while`循环;
        * 如果被设置过则直接返回`ture`;
          代表之前已上锁,则一直陷入循环不断判断;
    用这种方式来代表上锁的状态(flag))
    */
    while (flag.test_and_set(std::memory_order_acquire)) {
    }
  }

  void unlock() {
    /*
    将`atomic_flag`状态置位`false`,即如上面说的"将状态清除".
    */
    flag.clear(std::memory_order_release);
  }

 private:
  std::atomic_flag flag = ATOMIC_FLAG_INIT;  // 初始化为`{0}`
};

void test_SpinLock() {
  Spinlock spinlock;

  std::thread t1([&spinlock]() {
    spinlock.lock();  // 上锁
    for (int i = 0; i < 3; i++) {
      std::cout << "--- thread 1 ";
    }
    std::cout << std::endl;
    spinlock.unlock();  // 解锁
  });

  std::thread t2([&spinlock]() {
    spinlock.lock();
    for (int i = 0; i < 3; i++) {
      std::cout << "--- thread 2 ";
    }
    std::cout << std::endl;
    spinlock.unlock();
  });

  t1.join();
  t2.join();
}

int main() {
  test_SpinLock();
}