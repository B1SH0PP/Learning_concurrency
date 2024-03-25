#include <atomic>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/* 内存序(字节序):C++提供了6种内存序,构成了3种内存模型 */

/*
本节介绍由顺序一致内存序(std::memory_order_seq_cst)组成的"全局顺序一致模型(Sequencial consistent ordering)"
原子操作的"默认"模型.
*/
/*
以`std::memory_order_seq_cst`全局顺序一致模型进行"写操作"时,cpu核心会先读取要执行的数据到一级缓存`storeBuffer`内
进行修改,此时其他核心就无法读取该数据(无法读取),等到本核心写入完成后同步到三级缓存`cache`后,其他核心才能读取或写入.

这种模型开销比"锁"要小一些,但是相比其他内存模型是要大的.

可用于`store`,`load`和`read-modify-write`操作;
*/
std::atomic<bool> x, y;
std::atomic<int> z;

/*
在函数`write_x_then_y`中一定是按顺序先执行`x`后执行`y`,并且执行顺序在其他线程看来也是一致的,所以在线程`t2`的线程函
数`read_y_then_x`中观察到的顺序一定是`x`先于`y`执行的,所以当`while`退出时,`x`必然已经执行完了(true),所以这个例子
肯定不会出发断言.
*/
void write_x_then_y() {
  x.store(true, std::memory_order_seq_cst);  // 1
  y.store(true, std::memory_order_seq_cst);  // 2
}

void read_y_then_x() {
  while (!y.load(std::memory_order_seq_cst)) {  // 3
    std::cout << "y load false" << std::endl;
  }

  if (x.load(std::memory_order_seq_cst)) {  // 4
    ++z;
  }
}

void test_order_seq_cst() {
  std::thread t1(write_x_then_y);
  std::thread t2(read_y_then_x);
  t1.join();
  t2.join();
  assert(z.load() != 0);  // 5
}
int main() {
  //
  test_order_seq_cst();
}