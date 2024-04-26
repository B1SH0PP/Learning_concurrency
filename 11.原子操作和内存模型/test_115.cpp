#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/* 单例模式改良 */
// ??? 先略过,没耐心看,不知道咋用

// 利用智能指针解决释放问题
class SingleAuto {
 private:
  SingleAuto() {}
  // 删除拷贝构造/赋值
  SingleAuto(const SingleAuto&) = delete;
  SingleAuto& operator=(const SingleAuto&) = delete;

 public:
  ~SingleAuto() { std::cout << "single auto delete success !" << std::endl; }

  static std::shared_ptr<SingleAuto> GetInst() {
    if (single != nullptr) {  // 1
      return single;
    }

    s_mutex.lock();           // 2
    if (single != nullptr) {  // 3
      s_mutex.unlock();
      return single;
    }
    single = std::shared_ptr<SingleAuto>(new SingleAuto);  // 4
    s_mutex.unlock();

    return single;
  }

 private:
  static std::shared_ptr<SingleAuto> single;
  static std::mutex s_mutex;
};

std::shared_ptr<SingleAuto> SingleAuto::single = nullptr;
std::mutex SingleAuto::s_mutex;

void TestSingle() {
  std::thread t1([]() {
    std::cout << "thread t1 singleton address is 0X: " << SingleAuto::GetInst() << std::endl;
  });

  std::thread t2([]() {
    std::cout << "thread t2 singleton address is 0X: " << SingleAuto::GetInst() << std::endl;
  });

  t2.join();
  t1.join();
}

int main() {
}