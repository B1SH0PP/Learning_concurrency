#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

/* C++版的CSP设计模式 */
/*
CSP(Communicating Sequential Process),通信顺序进程
*/

template <typename T>
class Channel {
 private:
  std::queue<T> queue_;                  // 队列
  std::mutex mtx_;                       // 维护队列安全
  std::condition_variable cv_producer_;  // 防止队列满了使生产者线程挂起
  std::condition_variable cv_consumer_;  // 防止队列空了使消费者线程挂起
  size_t capacity_;                      // 队列容量
  bool closed_ = false;                  // channel是否关闭

 public:
  Channel(size_t capacity = 0) : capacity_(capacity) {}

  bool send(T value) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_producer_.wait(lock, [this]() {
      // 对于无缓冲的channel，我们应该等待直到有消费者准备好
      return (capacity_ == 0 && queue_.empty()) || queue_.size() < capacity_ || closed_;
    });
    if (closed_) {
      return false;
    }
    queue_.push(value);
    cv_consumer_.notify_one();
    return true;
  }

  bool receive(T& value) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_consumer_.wait(lock, [this]() { return !queue_.empty() || closed_; });
    if (closed_ && queue_.empty()) {
      return false;
    }
    value = queue_.front();
    queue_.pop();
    cv_producer_.notify_one();
    return true;
  }

  void close() {
    std::unique_lock<std::mutex> lock(mtx_);
    closed_ = true;
    cv_producer_.notify_all();
    cv_consumer_.notify_all();
  }
};