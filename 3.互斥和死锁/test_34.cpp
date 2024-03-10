#include <iostream>
#include <mutex>
#include <stack>

/* 同时加锁:如果想在一个函数内对2个互斥量同时加锁,并且避免死锁 */
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

  // 重载=
  bigObject& operator=(const bigObject& obj) {
    // 先判断是否是自己
    if (this == &obj) {
      return *this;
    }

    _data = obj._data;
    return *this;
  }

  // 重载<<
  friend std::ostream& operator<<(std::ostream& os, bigObject& obj) {
    os << obj._data;
    return os;
  }

 private:
  int _data;
};

int main() {
  /* 4.同时加锁 */
  bigObject obj1(100);
  bigObject obj2(200);
  obj2 = std::move(obj1);
}
