#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <stack>

/* 递归锁(不推荐使用) */
class recursive_lock_Demo {
 public:
  recursive_lock_Demo() {}
  bool QueryStudent(std::string name) {
    std::lock_guard<std::recursive_mutex> lg_rm(_recursive_mtx);
    auto iter_find = _students_info.find(name);
    if (iter_find == _students_info.end()) {
      return false;
    }
    return true;
  }

  void AddScore(std::string name, int score) {
    std::lock_guard<std::recursive_mutex> recursive_lock(_recursive_mtx);
    if (!QueryStudent(name)) {
      _students_info.insert(std::make_pair(name, score));
      return;
    }
    _students_info[name] = _students_info[name] + score;
  }

 private:
  std::map<std::string, int> _students_info;
  std::recursive_mutex _recursive_mtx;
};

int main() {
  /* 3.递归锁 */
}
