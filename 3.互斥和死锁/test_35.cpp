#include <iostream>
#include <mutex>
#include <stack>

/* 1.`unique_lock` */
std::mutex mtx;
int shared_data = 0;

/* 1.1上锁&解锁 */
void use_unique_lock() {
  /*
  `unique_lock`可以自动解锁(作用域结束),也可以手动解锁
  区别于`lock_guard`,它不可以手动解锁
  */
  std::unique_lock<std::mutex> ul(mtx);
  std::cout << "--- lock success" << std::endl;
  shared_data++;
  ul.unlock();  // 手动解锁
  std::cout << "--- unlock" << std::endl;
}

/* 1.2可判断是否占有锁 */
void use_owns_lock() {
  std::unique_lock<std::mutex> ul(mtx);
  shared_data++;
  if (ul.owns_lock()) {
    std::cout << "--- owns lock" << std::endl;
  } else {
    std::cout << "--- doesn't own lock" << std::endl;
  }
  ul.unlock();
  if (ul.owns_lock()) {
    std::cout << "--- owns lock" << std::endl;
  } else {
    std::cout << "--- doesn't own lock" << std::endl;
  }
}

/* 1.3延迟加锁 */
void use_defer_lock() {
  /*
  设置了`std::defer_lock`后就会延迟加锁,`mtx`不会在随后的作用域内立刻上锁,而是等我们手动上锁;
  锁的释放还是一样,手动和自动都可以.
  */
  std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
  // 加锁
  ul.lock();
  // 手动解锁
  ul.unlock();
}

/* 1.4同时使用`owns_lock`和`defer_lock` */
void use_owns_lock_and_defer_lock() {
  std::unique_lock<std::mutex> ul_1(mtx);
  // 判断是否拥有锁
  if (ul_1.owns_lock()) {
    std::cout << "--- main thread own the lock" << std::endl;
  } else {
    std::cout << "--- main thread doesn't own the lock" << std::endl;
  }

  std::thread t1([]() {
    std::unique_lock<std::mutex> ul_2(mtx, std::defer_lock);
    if (ul_2.owns_lock()) {
      std::cout << "--- t1 thread own the lock" << std::endl;
    } else {
      std::cout << "--- t1 thread doesn't own the lock" << std::endl;
    }
    /*
    无法加锁,因为互斥量`mtx`已在上面被`ul_1`锁上
    */
    ul_2.lock();
    if (ul_2.owns_lock()) {
      std::cout << "--- t1 thread own the lock" << std::endl;
    } else {
      std::cout << "--- t1 thread doesn't own the lock" << std::endl;
    }
    ul_2.unlock();
  });
  t1.join();
}

/* 1.5领养操作 */
void use_owns_lock_and_adopt_lock() {
  // 上锁
  mtx.lock();
  // 领养(被领养前必须确保已上锁,不然出作用域时强制解锁时会崩溃)
  std::unique_lock<std::mutex> ul_3(mtx, std::adopt_lock);
  if (ul_3.owns_lock()) {
    std::cout << "--- own the lock" << std::endl;
  } else {
    std::cout << "--- doesn't own the lock" << std::endl;
  }
}

/* 1.6 demo */
int a = 10;
int b = 1024;
std::mutex mtx_1;
std::mutex mtx_2;

void safe_swap() {
  std::lock(mtx_1, mtx_2);
  std::unique_lock<std::mutex> ul_4(mtx_1, std::adopt_lock);
  std::unique_lock<std::mutex> ul_5(mtx_2, std::adopt_lock);
  std::swap(a, b);

  /*
  错误用法1:`mtx_1`和`mtx_2`已经交由`ul_4`和`ul_5`管理(上了特种锁),所以不能再用其自身来解
           锁,不知为何能通过编译
  */
  mtx_1.unlock();
  mtx_2.unlock();
  // 改为:
  ul_4.unlock();
  ul_5.unlock();

  /*
  错误用法2:`mtx_1`和`mtx_2`已经交由`ul_4`和`ul_5`管理(上了特种锁),不知为何能通过编译,但是
            运行会报错
  */
  // std::lock(mtx_1, mtx_2);
  // 改为:
  std::lock(ul_4, ul_5);
}

/* 1.7转移互斥量所有权 */
/*
`mutex`是不可拷贝或移动的,但是`unique_lock`是支持移动构造/赋值(不支持拷贝构造/赋值)的,返回`unique_lock`
对象即意味着将其绑定的互斥量`mtx`加/解锁的权限给了外部调用者.
`unique_lock`在多个函数间调用传递后是不变的,因为其底层调用的是"移动"操作
*/
std::unique_lock<std::mutex> get_lock() {
  std::unique_lock<std::mutex> ul_6(mtx);
  shared_data++;
  return ul_6;
}

void use_return() {
  std::unique_lock<std::mutex> ul_7(get_lock());
  shared_data++;
}

/* 1.8锁的粒度:一种思想 */
void precison_lock() {
  std::unique_lock<std::mutex> ul_8(mtx);
  shared_data++;
  ul_8.unlock();
  // 不涉及共享数据的耗时操作不要放在锁内执行
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // 再用时再加锁
  ul_8.lock();
  shared_data++;
}

int main() {
  /* 5.unique_lock */
  // use_unique_lock();
  // use_owns_lock();
  // use_owns_lock_and_defer_lock();
  // use_owns_lock_and_adopt_lock();
  // safe_swap();
  // use_return();
  precison_lock();
}
