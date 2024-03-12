#include <iostream>
#include <mutex>

/* 锁的使用 */
/*
互斥量`mutex`:我理解的互斥量就像存储的宝箱,用来存放共享数据以便上锁管理
*/
std::mutex mtx1;
int shared_data = 1024;

void use_lock_1() {
  while (true) {
    // 对共享区数据访问前先加锁
    mtx1.lock();
    shared_data++;
    std::cout << "current thread id is: " << std::this_thread::get_id()
              << std::endl;
    std::cout << "shared data is: " << shared_data << std::endl;
    mtx1.unlock();
    // 防止死循环抢占cpu时间片,睡眠一会儿
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

// 方式一:不使用`lock_guard`,手动上锁解锁(即普通的锁,互斥量自带的锁)
void use_lock_2() {
  std::thread t1(use_lock_1);
  std::thread t2([]() {
    while (true) {
      mtx1.lock();  // 直接对互斥量lock上锁
      shared_data--;
      std::cout << "current thread id is: " << std::this_thread::get_id()
                << std::endl;
      std::cout << "shared data is: " << shared_data << std::endl;
      mtx1.unlock();  // 直接对互斥量unlock解锁
      // 防止死循环抢占cpu时间片,睡眠一会儿
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  });

  t1.join();
  t2.join();
}

// 方式二:使用`lock_guard`,自动解锁(一种特殊的锁)
void use_lock_3() {
  std::thread t1(use_lock_1);
  std::thread t2([]() {
    while (true) {
      /*
      使用局部作用域`{}`来封装`lock_guard`,当作用域结束后调用析构自动释放.
      这是一种良好的编程习惯.
      */
      {
        std::lock_guard<std::mutex> lk_gd(mtx1);  // lock_guard
        shared_data--;
        std::cout << "current thread id is: " << std::this_thread::get_id()
                  << std::endl;
        std::cout << "shared data is: " << shared_data << std::endl;
      }
      // 防止死循环抢占cpu时间片,睡眠一会儿
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  });

  t1.join();
  t2.join();
}

int main() {
  /* 1.锁的使用 */
  // 方式一:不使用`lock_guard`,手动上锁解锁
  // use_lock_2();

  // 方式二:使用`lock_guard`,自动解锁
  use_lock_3();
}
