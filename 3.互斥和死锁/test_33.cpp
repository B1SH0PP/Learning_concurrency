#include <iostream>
#include <mutex>
#include <stack>

/* 死锁 */
std::mutex mtx1;
std::mutex mtx2;
int m_1 = 0;
int m_2 = 1;

void dead_lock1() {
  while (true) {
    std::cout << "dead lock1 begin" << std::endl;
    mtx1.lock();
    m_1 = 1024;
    mtx2.lock();
    m_2 = 2048;
    mtx2.unlock();
    mtx1.unlock();
    std::cout << "dead lock1 end" << std::endl;
  }
}

void dead_lock2() {
  while (true) {
    std::cout << "dead lock2 begin" << std::endl;
    mtx2.lock();
    m_1 = 1024;
    mtx1.lock();
    m_2 = 2048;
    mtx1.unlock();
    mtx2.unlock();
    std::cout << "dead lock2 end" << std::endl;
  }
}

void test_dead_lock() {
  std::thread t1(dead_lock1);
  std::thread t2(dead_lock2);
  t1.join();
  t2.join();
}

/*
解决方法:将上锁-修改数据-解锁封装成原子操作
就是封装成一个函数,一块儿执行掉,封装成不可拆分的最小步骤)
*/
void atomic_lock1() {
  std::cout << "lock1 lock\n";
  mtx1.lock();
  m_1 = 1024;
  mtx1.unlock();
  std::cout << "lock1 unlock\n";
}

void atomic_lock2() {
  std::cout << "lock2 lock\n";
  mtx2.lock();
  m_2 = 2048;
  mtx2.unlock();
  std::cout << "lock2 unlock\n";
}

void safe_lock1() {
  while (true) {
    atomic_lock1();
    atomic_lock2();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void safe_lock2() {
  while (true) {
    atomic_lock2();
    atomic_lock1();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void test_safe_lock() {
  std::thread t1(safe_lock1);
  std::thread t2(safe_lock2);
  t1.join();
  t2.join();
}

main() {
  /* 3.死锁 */
  // test_dead_lock();
  test_safe_lock();
}
