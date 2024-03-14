#include <iostream>
#include <mutex>

int num = 1;
std::mutex mtx_num;
std::condition_variable cvA;  // 条件变量
std::condition_variable cvB;

/* 条件变量使用demo */
/* 不良实现 */
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

/* 改进版实现 */
void reasonableImplemention() {
  std::thread thread_A([]() {
    for (;;) {
      std::unique_lock<std::mutex> ul_mt(mtx_num);
      // 写法一:用while循环判断
      // while (num != 1) {
      //   cvA.wait(ul_mt);
      // }
      // 写法二:用lambda表达式提供一个谓词
      cvA.wait(ul_mt, []() {  //(锁,谓词),如果谓词返回true则加上锁`ul_mt`不然就一直挂起等待.
        return num == 1;
      });

      num++;
      std::cout << "--- thread A print 1" << std::endl;
      cvB.notify_one();
    }
  });

  std::thread thread_B([]() {
    for (;;) {
      std::unique_lock<std::mutex> ul_mt(mtx_num);
      cvB.wait(ul_mt, []() {  //(锁,谓词)
        return num == 2;
      });

      num--;
      std::cout << "--- thread B print 2" << std::endl;
      /*
      唤醒`cvA`之后其会再次执行谓词`num == 1`,发现满足条件返回`true`,即可加锁并往下执行.
      */
      cvA.notify_one();
      // 唤醒所有等待cvA条件变量的线程(此处我们只有一个线程`thread_A`中使用了`cvA`,所以没必要)
      // cvA.notify_all();
    }
  });

  thread_A.join();
  thread_B.join();
}

int main() {
  /* 1.条件变量使用demo */
  // 不良实现(明显能感受到每个线程睡眠等待的卡顿感)
  // undueImplemention();
  // 改进版
  reasonableImplemention();
}
