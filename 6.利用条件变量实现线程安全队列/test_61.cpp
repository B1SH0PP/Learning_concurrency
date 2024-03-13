#include <iostream>
#include <mutex>

int num = 1;
std::mutex mtx_num;
std::condition_variable cvA;  // 条件变量
std::condition_variable cvB;

// 不良实现
void undueImplemention() {
  std::thread thread_A([]() {
    for (;;) {
      std::lock_guard<std::mutex> lg_mt(mtx_num);
      if (num == 1) {
        std::cout << "--- thread A print 1" << std::endl;
        num++;
        continue;
      }
      // 如果不是1,睡50ms,等线程B完成`--`
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  });

  std::thread thread_B([]() {
    for (;;) {
      std::lock_guard<std::mutex> lg_mt(mtx_num);
      if (num == 2) {
        std::cout << "--- thread B print 2" << std::endl;
        num--;
        continue;
      }
      // 如果不是2,睡50ms,等线程A完成`++`
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  });
  thread_A.join();
  thread_B.join();
}

// 改进版实现
void reasonableImplemention() {
  std::thread thread_A([]() {
    for (;;) {
      std::unique_lock<std::mutex> ul_mt(mtx_num);
      // 条件变量的使用
      cvA.wait(ul_mt, []() {  //(锁,谓词)
        return num == 1;
      });
    }
  });
}

int main() {
  // 不良实现(明显能感受到每个线程睡眠等待的卡顿感)
  undueImplemention();
}
