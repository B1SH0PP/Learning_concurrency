#include <iostream>
#include <string>
#include <thread>

// https://www.bilibili.com/video/BV1v8411R7hD/?spm_id_from=333.788&vd_source=a8a38358873e306ffdd6017aaab418e3
//  0:24:13 ~ 0:44:51

/* joining thread */
class joining_thread {
  std::thread _t;

 public:
  joining_thread() noexcept = default;
};

void use_jointhread() {}

int main() {
  /* 2.joining_thread */
  use_jointhread();
}