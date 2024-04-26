#pragma once
#include <iostream>
#include <memory>
#include <mutex>

/* 环形队列 */

/*
`size_t Cap`是非类型模板
*/
template <typename T, size_t Cap>
class CircularQueLock : private std::allocator<T> {
 public:
  CircularQueLock() : _max_size(Cap + 1),
                      _data(std::allocator<T>::allocate(_max_size)),
                      _head(0),
                      _tail(0) {}
  CircularQueLock(const CircularQueLock&) = delete;
  CircularQueLock& operator=(const CircularQueLock&) volatile = delete;  // 禁止对`volatile`修饰的对象拷贝赋值
  CircularQueLock& operator=(const CircularQueLock&) = delete;

  ~CircularQueLock() {
    // 循环销毁
    std::lock_guard<std::mutex> lock(_mtx);
    // 遍历元素析构
    while (_head != _tail) {
      // 通过`destroy`调用对应数据类型的析构函数
      std::allocator<T>::destroy(_data + _head);
      _head++;
    }
    // 用`allocator`创建的内存对象,需使用`deallocate`回收内存
    std::allocator<T>::deallocate(_data, _max_size);
  }

  // 先实现一个可变参数列表版本的插入函数作为基准函数
  template <typename... Args>
  bool emplace(Args&&... args) {
    std::lock_guard<std::mutex> lock(_mtx);
    /*
    判断队列是否满了,`尾下标+1`若等于队列大小了,说明首下标和尾下标已重合,说明此时队列已满
    */
    if ((_tail + 1) % _max_size == _head) {
      std::cout << "circular queue is full !" << std::endl;
      return false;
    }

    /*
    在尾部位置构造一个T类型的对象,构造参数为args...
    首地址向后偏移到"尾下标"处,
    */
    std::allocator<T>::construct(_data + _tail, std::forward<Args>(args)...);
    // 更新尾部元素位置
    _tail = (_tail + 1) % _max_size;
    return true;
  }

  // `push`实现两个版本,一个接受左值引用,一个接受右值引用
  // 左值引用版本
  bool push(const T& val) {
    std::cout << "called push const T& version" << std::endl;
    return emplace(val);
  }

  // 右值引用版本 当然也可以接受左值引用,`T&&`为万能引用,但是因为我们实现了const T&
  bool push(T&& val) {
    std::cout << "called push T&& version" << std::endl;
    return emplace(std::move(val));
  }

  // 出队函数
  bool pop(T& val) {
    std::lock_guard<std::mutex> lock(_mtx);
    // 判断头部和尾部指针是否重合,如果重合则队列为空
    if (_head == _tail) {
      std::cout << "circular queue is empty !" << std::endl;
      return false;
    }

    // 取出头部指针指向的数据
    val = std::move(_data[_head]);
    // 更新头部指针
    _head = (_head + 1) % _max_size;
    return true;
  }

 private:
  size_t _max_size;  // 队列大小(容量)
  T* _data;          // 队列首地址
  std::mutex _mtx;
  size_t _head = 0;  // 队列首下标(指向第一个元素)
  size_t _tail = 0;  // 队列尾下标(指向最后一个元素的下一位)
};