#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

/* 线程池 */
class ThreadPool {
 public:
  // 删除拷贝操作
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // 使用局部静态变量实现单例
  static ThreadPool& instance() {
    static ThreadPool instance;
    return instance;
  }

  using Task = std::packaged_task<void()>;

  template <typename F, typename... Args>
  auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    using ReturnType = decltype(f(args...));
    // 线程池是否停止
    if (stop_.load())
      // 返回一个空`future`;`{}`是赋值操作
      return std::future<ReturnType>{};

    /*
    @brief:创建智能指针`task`指向一个`std::packaged_task`对象;`ReturnType()`是返回类型和参数类型;
    @param-std::bind:这里使用`std::bind`将参数
    @param-<ReturnType()>:函数签名,`ReturnType`是返回类型,`()`是`std::bind`生成的无参函数的参数类型,
        所以是个空括号.
    @param-std::forward<Args>(args)...:加`...`意为将参数包`args`展开,参数依次打包给可调用对象`f`,而
        不是将整个参数打包进去.
        注意`...`的位置;
    */
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<ReturnType> futureVal = task->get_future();
    {
      std::lock_guard<std::mutex> lg_mtx(cv_mtx_);
      /*
      1.这里lambda表达式使用"值传递"在`[]`中传入`std::shared_ptr`,值拷贝导致其引用记数会`+1`,所以不用担心异常;
      2.`(*task)()`是通过`*`对智能指针解引用出`std::packaged_task`对象,其封装了一个`bind`好的可调用对象(函数),
      之后再通过`std::packaged_task`重载的`()`运算符,将其当做仿函数来调用,执行封装好的异步任务.
      3.这里使用`emplace`调用构造函数,将lambda表达式作为`std::packaged_task`的构造函数要绑定的异步任务来创建
      对象再插入.
      */
      tasks_.emplace([task] {
        (*task)();
      });
    }
    // 虽然可能有多个线程挂起(wait),这里开启谁好像是根据编译器的调度策略来的.
    cv_lock_.notify_one();
    return futureVal;
  }

  // 暂时用不上
  // int idleThreadCount() {
  //   return thread_num_;
  // }

 private:
  ThreadPool(unsigned int thread_num = 2) : stop_(false) {  // 暂时先开2个线程
    {
      if (thread_num < 1)
        thread_num_ = 1;
      else
        thread_num_ = thread_num;
    }
    start();
  }

  ~ThreadPool() {
    stop();
  }

  void start() {
    for (int i = 0; i < thread_num_; ++i) {
      /*
      用`emplace_back`减少一次拷贝/移动构造,走的普通构造,性能开销会好一点;
      其次,使用`emplace_back`时传入的lambda表达式会直接作为`std::thread`构造函数的参数,直接创建线程并插入.
      */
      pool_.emplace_back([this]() {
        /*
        这个`while`就是每个线程内的无限循环,不断的从队列读取任务并执行,若没任务则挂起(通过条件变量wait)
        */
        while (!this->stop_.load()) {
          Task task;  // std::packaged_task<void()> task
          {
            std::unique_lock<std::mutex> ul_mtx(cv_mtx_);
            this->cv_lock_.wait(ul_mtx, [this] {
              return this->stop_.load() || !this->tasks_.empty();
            });
            if (this->tasks_.empty())
              return;

            task = std::move(this->tasks_.front());
            this->tasks_.pop();
          }
          this->thread_num_--;
          task();  // 运行封装好的lambda函数
          this->thread_num_++;
        }
      });
    }
  }

  void stop() {
    stop_.store(true);
    cv_lock_.notify_all();
    for (auto& td : pool_) {
      if (td.joinable()) {
        std::cout << "join thread: " << td.get_id() << std::endl;
        td.join();
      }
    }
  }

 private:
  std::mutex cv_mtx_;
  std::condition_variable cv_lock_;
  std::atomic_bool stop_;       //`std::atomic<bool>`的别名
  std::atomic_int thread_num_;  //`std::atomic<int>`的别名
  std::queue<Task> tasks_;
  std::vector<std::thread> pool_;
};

#endif  // !__THREAD_POOL_H__
