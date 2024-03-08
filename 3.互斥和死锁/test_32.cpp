#include <iostream>
#include <mutex>
#include <stack>

/* 数据安全 */
template <typename T>
class threadsafe_stack_unsafe {
 private:
  std::stack<T> data;
  mutable std::mutex mtx;

 public:
 threadsafe_stack_unsafe(){}
 threadsafe_stack_unsafe(const threadsafe_stack_unsafe& other_obj){
    std::lock_guard<std::mutex> lk_gk(other_obj.mtx);
    data = other_obj.data;
 }

 //禁止对象之间的拷贝赋值(保留拷贝构造)
 threadsafe_stack_unsafe& operator=(const threadsafe_stack_unsafe&) = delete;
};

int main() { /* 2.数据安全 */
}
