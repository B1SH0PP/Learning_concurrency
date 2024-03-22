#include <atomic>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/* 内存序(字节序) --- 宽松内存序(std::memory_order_relaxed) */
/*
`std::memory_order_relaxed`的性质:
* 1 作用于原子变量;

* 2 不具有`synchronizes-with`(同步)关系,即不具备"全局同步性",因此在多线程环境下,对共享变量的修改不会立即
    对其他线程可见.
    按照:`store buffer` -> `cache` -> `memory`(一级缓存 -> 二级 -> 三级)的顺序,同一个`bank`内的`cpu-core`
    只要没将`storebuffer`内的数据同步到`cache`(二级缓存),通一个`bank`内其他`core`也是看不到的,并且,如果没将
    `bank`内`cache`的数据同步到`memory`,其他`bank`的`core`也是看不到的.

* 3-1 对于同一个原子变量,在同一个线程中具有`happens-before`关系;
      即:
        A.store(1,std::memory_order_relaxed);
        A.load(std::memory_order_relaxed);
      在"单线程"中对"同一个原子变量"的修改一定是按顺序执行的,后者一定是能读到最新的值的.

  3-2 对于同一线程中不同的原子变量不具有`happens-before`关系,可以乱序执行;
      即:
        A.store(1,std::memory_order_relaxed);
        B.store(2,std::memory_order_relaxed);
      在"单线程"中对"不同的原子变量"的修改"不一定"是按顺序执行的,B有可能会在A之前修改.

* 4 多线程情况下不具有`happens-before`关系。
*/

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
  /*
  对应性质3第二句,因为这里选用的宽松内存序,所以底层不一定会按照先`x`后`y`的执行顺序(因为`x`和`y`不是耦合的).
  */
  x.store(true, std::memory_order_relaxed);  // 1
  y.store(true, std::memory_order_relaxed);  // 2
}

void read_y_then_x() {
  /*
  若发生当`write_x_then_y`下的`y`先于`x`执行了,原本一直卡在`while`循环的线程`t2`会退出,若此时`x`也还没来
  得及置为`true`,便不会执行`++z`,此时才有可能触发断言.
  */
  while (!y.load(std::memory_order_relaxed)) {  // 3
    std::cout << "y load false" << std::endl;
  }

  if (x.load(std::memory_order_relaxed)) {  // 4
    ++z;
  }
}

void test_memory_order_relaxed() {
  x, y = false;
  z = 0;

  std::thread t1(write_x_then_y);
  std::thread t2(read_y_then_x);
  t1.join();
  t2.join();

  assert(z.load() != 0);  // 5
}

void test_memory_order_relaxed_2() {
  std::atomic<int> a{0};
  std::vector<int> v3, v4;

  // 存入偶数
  std::thread t1([&a] {
    for (int i = 0; i < 10; i += 2) {
      a.store(i, std::memory_order_relaxed);
    }
  });

  // 存入奇数
  std::thread t2([&a] {
    for (int i = 1; i < 10; i += 2) {
      a.store(i, std::memory_order_relaxed);
    }
  });

  // 取值
  std::thread t3([&v3, &a] {
    for (int i = 0; i < 10; ++i) {
      v3.push_back(a.load(std::memory_order_relaxed));
    }
  });

  // 取值
  std::thread t4([&v4, &a] {
    for (int i = 0; i < 10; ++i) {
      v4.push_back(a.load(std::memory_order_relaxed));
    }
  });

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  for (int i : v3) {
    std::cout << i << " ";
  }

  std::cout << std::endl;
  for (int i : v4) {
    std::cout << i << " ";
  }
}

int main() {
  // 要执行很多次才会触发
  // test_memory_order_relaxed();

  //
  test_memory_order_relaxed_2();
}