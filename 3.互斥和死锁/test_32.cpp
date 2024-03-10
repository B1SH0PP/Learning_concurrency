#include <iostream>
#include <mutex>
#include <stack>
#include <stdexcept>

/* 数据安全 */
template <typename T>  //<int>
class threadsafe_stack_unsafe {
 private:
  std::stack<T> data;
  mutable std::mutex mtx;

 public:
  threadsafe_stack_unsafe() {}
  // 拷贝构造
  threadsafe_stack_unsafe(const threadsafe_stack_unsafe &other_obj) {
    std::lock_guard<std::mutex> lk_gk(other_obj.mtx);
    data = other_obj.data;
  }

  // 禁止对象之间的拷贝赋值
  threadsafe_stack_unsafe &operator=(const threadsafe_stack_unsafe &) =
      delete;

  void push(T new_value) {
    std::lock_guard<std::mutex> lk_gd(mtx);
    data.push(
        std::move(new_value));  // 使用`move`使用移动构造,减少一次拷贝构造
  }

  // 问题代码(危险)
  T pop() {
    std::lock_guard<std::mutex> lk_gd(mtx);
    auto element = data.top();
    data.pop();
    return element;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lk_gd(mtx);
    return data.empty();
  }
};

void test_threadsafe_stack_unsafe() {
  threadsafe_stack_unsafe<int> safe_stack;
  safe_stack.push(1);

  /*
  这里的问题是多个线程同时判断`empty`后是线程不安全的.
  虽然`push`,`pop`和`empty`函数内都确保了线程上锁,但是当两个线程同时调用`empty`并且`stack`非空
  顺利进入到`if`内部,此时另一线程若已执行`pop`清空了`stack`,这样该线程再执行`pop`就会发生错误.
  !!! 这也是由于`pop`函数内部没有执行`stack`非空检查导致的.
  */
  std::thread t1([&safe_stack]() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (!safe_stack.empty()) safe_stack.pop();
  });

  std::thread t2([&safe_stack]() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (!safe_stack.empty()) safe_stack.pop();
  });

  t1.join();
  t2.join();
}

/* 解决方案 */
// 自定义异常结构体
struct stack_empty : std::exception {
  /*
  @brief:`what`是`std::exception`的虚函数
  @param:`const throw()`代表`what`函数既不会修改对象的成员变量,也不会抛出异常(老写法);
         新写法: 关键字`noexcept`
  @return:返回C风格字符串`const char*`
  */
  // const char* what() const throw();
  const char *what() const noexcept override {
    return "Stack is empty";
  }
};

template <typename T>
class threadsafe_stack_safe {
 private:
  std::stack<T> data;
  mutable std::mutex mtx;

 public:
  threadsafe_stack_safe() {}
  threadsafe_stack_safe(const threadsafe_stack_safe &other_obj) {
    std::lock_guard<std::mutex> lk_gk(other_obj.mtx);
    data = other_obj.data;
  }

  // 禁止对象之间的拷贝赋值
  threadsafe_stack_safe &operator=(const threadsafe_stack_safe &) = delete;

  void push(T new_value) {
    std::lock_guard<std::mutex> lk_gd(mtx);
    data.push(
        std::move(new_value));  // 使用`move`使用移动构造,减少一次拷贝构造
  }

  /*
  @que1:多个线程同时判断`empty`后是线程不安全的.
  @slt:加一个判断,并弹出异常.
  */
  T pop() {
    std::lock_guard<std::mutex> lk_gd(mtx);
    if (data.empty()) {
      throw stack_empty();
    }
    auto element = data.top();
    data.pop();
    return element;
  }

  /*
  @que2:32位,当`T`是`vec`等大数据量的容器,用`return`返回后临时变量`element`与外部接收的变量会发生
        一次拷贝,若内存已满,则会发生`element`数据丢失.
        64位可以交换内存,基本不会发生.
  @slt:用智能指针封装.
  */
  std::shared_ptr<T> pop_sp() {
    std::lock_guard<std::mutex> lk_gd(mtx);
    if (data.empty()) {
      throw stack_empty();
    }

    std::shared_ptr<T> const element_sp(std::make_shared<T>(data.top()));
    // auto element = data.top();
    data.pop();
    return element_sp;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lk_gd(mtx);
    return data.empty();
  }
};

void test_threadsafe_stack_safe() {
  threadsafe_stack_safe<int> safe_stack;  // 修改后
  safe_stack.push(1);

  std::thread t1([&safe_stack]() {
    std::this_thread::sleep_for(std::chrono::seconds(20));
    if (!safe_stack.empty())
      // safe_stack.pop();
      safe_stack.pop_sp();
  });

  std::thread t2([&safe_stack]() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (!safe_stack.empty())
      // safe_stack.pop();
      safe_stack.pop_sp();
  });

  t1.join();
  t2.join();
}

int main() {
  /* 2.数据安全 */
  // test_threadsafe_stack_unsafe();
  test_threadsafe_stack_safe();
}
