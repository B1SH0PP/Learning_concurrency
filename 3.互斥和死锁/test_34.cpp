#include <iostream>
#include <mutex>
#include <stack>

/* 1.同时加锁:如果想在一个函数内对2个互斥量同时加锁,并且避免死锁 */
std::mutex mtx1;
std::mutex mtx2;
int m_1 = 0;
int m_2 = 1;

class bigObject {
 public:
  bigObject(int data) : _data(data) {}
  // 拷贝构造
  bigObject(const bigObject& obj) : _data(obj._data) {}
  // 移动构造
  bigObject(bigObject&& obj) : _data(std::move(obj._data)) {}

  // 拷贝赋值
  bigObject& operator=(const bigObject& obj) {
    // 先判断是否是自己
    if (this == &obj) {
      return *this;
    }

    _data = obj._data;
    return *this;
  }

  // 移动赋值
  bigObject& operator=(const bigObject&& obj) {
    //_data = obj._data;
    _data = std::move(obj._data);
    return *this;
  }

  // 重载<<
  friend std::ostream& operator<<(std::ostream& os, bigObject& obj) {
    os << obj._data;
    return os;
  }

  // 交换数据
  friend void swap(bigObject& obj1, bigObject& obj2) {
    bigObject temp = std::move(obj1);
    obj1 = std::move(obj2);
    obj2 = std::move(temp);
  }

 private:
  int _data;
};

// 假设这个class包含了复杂的对象和一个互斥量
class bigObject_manager {
 public:
  bigObject_manager(int data = 0) : _obj(data) {}
  void printinfo() {
    std::cout << "current obj data is: " << _obj << std::endl;
  }

  friend void danger_swap(bigObject_manager& objm1, bigObject_manager& objm2);
  friend void safe_swap(bigObject_manager& objm1, bigObject_manager& objm2);
  friend void safe_swap_scope(bigObject_manager& objm1, bigObject_manager& objm2);

 private:
  std::mutex _mtx;
  bigObject _obj;
};

void danger_swap(bigObject_manager& objm1, bigObject_manager& objm2) {
  std::cout << "thread [" << std::this_thread::get_id() << "] begin" << std::endl;
  if (&objm1 == &objm2) {
    return;
  }
  std::lock_guard<std::mutex> lk_gd_1(objm1._mtx);
  std::this_thread::sleep_for(std::chrono::seconds(1));  // 为了导致死锁,先睡一会儿
  std::lock_guard<std::mutex> lk_gd_2(objm2._mtx);
  swap(objm1._obj, objm2._obj);
  std::cout << "thread [" << std::this_thread::get_id() << "] end" << std::endl;
}

void test_danger_swap() {
  bigObject_manager objm1(5);
  bigObject_manager objm2(100);
  objm1.printinfo();  // 打印交换前
  objm2.printinfo();

  /*
  死锁原因:`t1`和`t2`传入线程的顺序不同,`t1`先传入的`objm1`,所以在`danger_swap`内先将
          `objm1._mtx`上锁,而线程`t2`先传的`objm2`,所以先将`objm2._mtx`上锁,最终两线
          程函数陷入相互等待上接下来锁的死锁中.
  */
  std::thread t1(danger_swap, std::ref(objm1), std::ref(objm2));  // 引用`std::ref`传入
  std::thread t2(danger_swap, std::ref(objm2), std::ref(objm1));
  t1.join();
  t2.join();

  objm1.printinfo();  // 打印交换后
  objm2.printinfo();
}

void safe_swap(bigObject_manager& objm1, bigObject_manager& objm2) {
  std::cout << "thread [" << std::this_thread::get_id() << "] begin" << std::endl;
  if (&objm1 == &objm2) {
    return;
  }
  //一次上2个锁,有一个加不上都无法继续
  std::lock(objm1._mtx, objm2._mtx);
  /*
  使用"领养锁"管理互斥量的释放 (只管释放,不管上锁,故名领养锁)
  在使用领养锁`std::adopt_lock`之前必须上锁,因为作用域结束后会强制解锁,没锁上则会崩溃 !
  */
  std::lock_guard<std::mutex> lk_gd_1(objm1._mtx, std::adopt_lock);
  std::this_thread::sleep_for(std::chrono::seconds(1));  // 为了导致死锁,先睡一会儿
  std::lock_guard<std::mutex> lk_gd_2(objm2._mtx, std::adopt_lock);
  swap(objm1._obj, objm2._obj);
  std::cout << "thread [" << std::this_thread::get_id() << "] end" << std::endl;
}

void test_safe_swap() {
  bigObject_manager objm1(5);
  bigObject_manager objm2(100);
  objm1.printinfo();  // 打印交换前
  objm2.printinfo();

  std::thread t1(safe_swap, std::ref(objm1), std::ref(objm2));  // 引用`std::ref`传入
  std::thread t2(safe_swap, std::ref(objm2), std::ref(objm1));
  t1.join();
  t2.join();

  objm1.printinfo();  // 打印交换后
  objm2.printinfo();
}

/* 2.扩展:使用C++17新增的`scoped_lock`方式,同时管理多把锁 */
void safe_swap_scope(bigObject_manager& objm1, bigObject_manager& objm2) {
  std::cout << "thread [" << std::this_thread::get_id() << "] begin" << std::endl;
  if (&objm1 == &objm2) {
    return;
  }

  // 使用`scoped_lock`管理多个互斥量的上锁和解锁,相当于升级版的`lock_guard`
  std::scoped_lock sl(objm1._mtx, objm2._mtx);
  swap(objm1._obj, objm2._obj);
  std::cout << "thread [" << std::this_thread::get_id() << "] end" << std::endl;
}

void test_safe_swap_scope() {
  bigObject_manager objm1(5);
  bigObject_manager objm2(100);
  objm1.printinfo();  // 打印交换前
  objm2.printinfo();

  std::thread t1(safe_swap_scope, std::ref(objm1), std::ref(objm2));  // 引用`std::ref`传入
  std::thread t2(safe_swap_scope, std::ref(objm2), std::ref(objm1));
  t1.join();
  t2.join();

  objm1.printinfo();  // 打印交换后
  objm2.printinfo();
}

/* 3.层级锁:不同时上2把锁但也能保证安全性 */
class hierarchical_mutex {
 public:
  explicit hierarchical_mutex(unsigned long value) : _hierarchy_value(value),
                                                     _previous_hierarchy_value(0) {}

  /*
  因为锁是不支持拷贝和移动的,所以这里这里直接删除拷贝构造和拷贝赋值
  知识点:因为我们没有定义移动构造/赋值,且移动操作是不会系统提供的,默认是走拷贝,而我们又删除了拷贝操作,
        所以此时已不再支持拷贝和移动.
  */
  hierarchical_mutex(const hierarchical_mutex&) = delete;
  hierarchical_mutex& operator=(const hierarchical_mutex&) = delete;

  void lock() {
    // 检查加锁的层级值是否满足加锁条件
    check_for_hierarchy_violation();
    _internal_mutex.lock();
  }

  void unlock() {
    // 判断是否是同一把锁
    if (_this_thread_hierarchy_value != _hierarchy_value) {
      throw std::logic_error("mutex hierarchy violated !");
    }
    _this_thread_hierarchy_value = _previous_hierarchy_value;
    _internal_mutex.unlock();
  }

 private:
  std::mutex _internal_mutex;
  // 当前层级值
  unsigned long const _hierarchy_value;
  // 上一级层级值
  unsigned long _previous_hierarchy_value;
  // 本线程层级值
  static thread_local unsigned long _this_thread_hierarchy_value;

 private:
  void check_for_hierarchy_violation() {
    if (_this_thread_hierarchy_value <= _hierarchy_value)
      // 判断当前线程锁的层级是否小于等于要加的锁的层级
      throw std::logic_error("mutex hierarchy violated !");
  }

  void update_hierarchy_value() {
    _previous_hierarchy_value = _this_thread_hierarchy_value;
    _this_thread_hierarchy_value = _hierarchy_value;
  }
};

int main() {
  /* 4.同时加锁 */
  // 1.错误
  // test_danger_swap();  // 启动后就卡死了(只打印2个begin)

  // 2.修改
  // test_safe_swap();

  // 3.使用C++17的`scoped_lock`
  test_safe_swap_scope();

  // 4.层级锁(没看完,以后用到再说 ???)

  /* `拷贝构造/赋值` 和 `移动构造/赋值` */
  // bigObject obj1(100);
  // bigObject obj2(200);
  // 1.拷贝赋值
  // obj2 = obj1;  // 当只有移动构造没有拷贝构造时,错误
  // 2.移动赋值
  // obj2 = std::move(obj1);
}
