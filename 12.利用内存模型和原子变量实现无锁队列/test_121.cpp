#include "test_121.hpp"

#include "MyClass.h"

void test_CircularQueLock() {
  // 最大容量为5
  CircularQueLock<MyClass, 5> cq_lk;
  MyClass mc1(1);
  MyClass mc2(2);
  cq_lk.push(mc1);
  cq_lk.push(std::move(mc2));
  for (int i = 3; i <= 5; i++) {
    MyClass mc(i);
    auto res = cq_lk.push(mc);
    if (res == false) {
      break;
    }
  }

  cq_lk.push(mc2);

  for (int i = 0; i < 5; i++) {
    MyClass mc1;
    auto res = cq_lk.pop(mc1);
    if (!res) {
      break;
    }
    std::cout << "pop success, " << mc1 << std::endl;
  }

  auto res = cq_lk.pop(mc1);
}

int main() {
  test_CircularQueLock();
}