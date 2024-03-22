#include <iostream>
#include <mutex>

/* 返回局部变量(对象)的RVO机制 */
class TestCopy {
 public:
  TestCopy() {}
  TestCopy(const TestCopy& tp) {
    std::cout << "--- use copy function" << std::endl;
  }
  TestCopy(TestCopy&& cp) {  // 带有右值引用就会被判定为移动构造
    std::cout << "--- use move function" << std::endl;
  }
};

TestCopy TestCp() {
  TestCopy tp;
  return tp;  // 返回局部对象,会发现调用的是移动语义操作
}

int main() {
  TestCp();
  return 0;
}