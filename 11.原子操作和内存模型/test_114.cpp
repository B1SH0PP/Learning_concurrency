#include <atomic>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/* 内存序(字节序):C++提供了6种内存序,构成了3种内存模型 */

/*
本节介绍"获取发行模型(acquire-release ordering)",会用到3种内存序:
`memory_order_acquire`,`memory_order_release`和`memory_order_acq_rel`

1.`load`操作:`memory_order_acquire`
2.`store`操作:`memory_order_release`
3.`read-modify-write`操作:`memory_order_acquire`,`memory_order_release`和`memory_order_acq_rel`
*/

/*
`acquire-release`模型可以实现`synchronizes-with`的关系.

`acquire-release`模型主要影响编译器和处理器的"优化行为",而不是直接在代码中插入额外的指令.

当一个线程对一个原子变量执行`release`操作后,编译器会确保其后所有"读或写操作"都不会被重排到该`release`操作之前,
同样使用了`acquire`操作后,编译器会确保其之前的所有"读或写操作"都不会被重排到该`acquire`之后.
即:即使在多线程场景下,在对同一个原子变量的`release`和`acquire`之间的所有"读或写操作"(夹在中间的"读或写操作")都
不会被编译器重排.
如果编译器违反了这个规则,就破坏了`acquire-release`的语义.

这种方式(acquire-release ordering)以相对全局同步(sequencial consistent ordering)更小的开销实现了`happens-before`
关系,从而实现所需的同步行为.
*/

std::atomic<bool> x, y;
std::atomic<int> z;

/*
C++语义层面解释:(晦涩难懂)
  因为 #2 和 #3 使用了`memory_order_release`和`memory_order_acquire`(acquire-release顺序模型),所以构成了
  同步关系,又因为"#1 sequence-before #2",所以"#1 happens-before #3",又因为单线程内"#3 sequence-before #4",
  所以"#1 happens-before #4",所以可以得出肯定不会触发断言.

编译器原则层面的解释:
  由于`memory_order_release`和`memory_order_acquire`组成的`a-r`内存模型的约束,`release`之后的读或写不会被
  重排到它之前,`acquire`之前的操作不会被重排到它之后,所以 #1 必然在 #2 之前执行,
*/
void test_release_acquire() {
  std::atomic<bool> rx, ry;

  std::thread t1([&]() {
    rx.store(true, std::memory_order_relaxed);  // 1
    ry.store(true, std::memory_order_release);  // 2
  });

  std::thread t2([&]() {
    while (!ry.load(std::memory_order_acquire)) {
    }                                            // 3
    assert(rx.load(std::memory_order_relaxed));  // 4
  });

  t1.join();
  t2.join();
}

/*
危险示例:
多个线程`t1`和`t2`都对同一个原子变量`yd`执行`release`操作,另一个线程`t3`对这个变量`acquire`,那么最终只能有一个
线程的`release`操作和这个`acquire`线程构成同步关系,这就导致了未定义的行为(数据竞争)

很有可能发生: #3 和 #4 构成同步关系,此时 #1 很可能没有来得及将`xd`置为`1`,此时先执行到`assert`处,最终触发 #5 的
断言.(概率很低很低,但不是没有)
*/
void danger_release_acquire() {
  std::atomic<int> xd{0}, yd{0};
  std::atomic<int> zd;

  std::thread t1([&]() {
    xd.store(1, std::memory_order_release);  // 1
    yd.store(1, std::memory_order_release);  // 2
  });

  std::thread t2([&]() {
    yd.store(2, std::memory_order_release);  // 3
  });

  std::thread t3([&]() {
    while (!yd.load(std::memory_order_acquire)) {  // 4
    }
    assert(xd.load(std::memory_order_acquire) == 1);  // 5
  });

  t1.join();
  t2.join();
  t3.join();
}

int main() {
  // 1.
  // test_release_acquire();

  // 2.危险操作
  danger_release_acquire();
}