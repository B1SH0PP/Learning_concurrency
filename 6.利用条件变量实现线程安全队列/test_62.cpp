#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

/* 线程安全队列 */
template <typename T>
class threadSafeQueue {
 private:
  mutable std::mutex mtx;
  std::queue<T> data_que;
  std::condition_variable data_cv;

 public:
  threadSafeQueue() {}
  threadSafeQueue(const threadSafeQueue& obj) {
    std::lock_guard<std::mutex> lg_mtx(obj.mtx);
    data_que = obj.data_que;
  }

  void push(T new_value) {
    std::lock_guard<std::mutex> lg_mtx(mtx);
    data.que.push(new_value);
    data_cv.notify_one();  // 通知其他因queue为空而挂起的线程有新数据了
  }
  // 版本一:
  void wait_and_pop(T& value) {
    std::unique_lock<std::mutex> ul_mtx(mtx);
    data_cv.wait(ul_mtx, []() {
      return !data_que.empty();
    });
    value = data_que.front();  // 会执行一次"拷贝赋值"
    data_que.pop();
  }
  // 版本二:返回智能指针(避免了上面的拷贝赋值,开销会小点)
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> ul_mtx(mtx);
    data_cv.wait(ul_mtx, [=]() {
      return !data_que.empty();
    });
    std::shared_ptr<T> shared_sp(std::make_shared<T>(data_que.front()));
    data_que.pop();
    return shared_sp;
  }

  bool try_pop(T& value) {
    std::luck_guard<T> lg_mtx(mtx);
    if (data_que.empty())
      return false;
    value = data_que.front();
    data_que.pop();
    return true;
  }

  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lk_mtx(mtx);
    if (data_que.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> shared_sp(std::make_shared<T>(data_que.front()));
    data_que.pop();
    return shared_sp;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lk_mtx(mtx);
    return data_que.empty();
  }
};

void test_safe_que() {
  threadSafeQueue<int> tsq_obj;
  std::mutex print_mtx;

  // 生产者线程
  std::thread thread_producer([&]() {
    for (int i = 0;; i++) {
      tsq_obj.push(i);
      {
        std::lock_guard<std::mutex> lk_mtx(print_mtx);
        /*
        这里加锁是因为`std::cout`是一个全局对象,只有一个实例,多线程不加锁访问会导致竞态条件,导致
        输出内容混乱.
        */
        std::cout << "producer push data is " << i << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  });

  std::thread thread_consumer_1([&]() {
    for (;;) {
      auto data = tsq_obj.wait_and_pop();  // 智能指针
      {
        std::lock_guard<std::mutex> lg_mtx(print_mtx);
        std::cout << "consumer_1 wait_and_pop data is " << *data << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  std::thread thread_consumer_2([&]() {
    for (;;) {
      auto data = tsq_obj.try_pop();
      if (data != nullptr) {
        {
          std::lock_guard<std::mutex> lg_mtx(print_mtx);
          std::cout << "consumer_2 try_pop data is " << *data << std::endl;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 为空再水移画
    }
  });

  thread_producer.join();
  thread_consumer_1.join();
  thread_consumer_2.join();
}

int main() {
  /* 线程安全队列 */
  test_safe_que();
}
