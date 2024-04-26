#pragma once
#include <iostream>

class MyClass {
 public:
  MyClass() : _data(0) {
  }

  MyClass(int data) : _data(data) {
  }

  MyClass(const MyClass& mc) {
    std::cout << "called MyClass const MyClass& version " << std::endl;
    _data = mc._data;
  }

  MyClass(MyClass&& mc) noexcept {
    std::cout << "called MyClass  MyClass&& version " << std::endl;
    _data = std::move(mc._data);
  }

  friend std::ostream& operator<<(std::ostream& os, const MyClass& my) {
    os << "MyClass Data is " << my._data;
    return os;
  }

  MyClass& operator=(const MyClass& other) {
    _data = other._data;
    return *this;
  }

 private:
  int _data;
};