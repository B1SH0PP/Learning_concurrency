#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <stack>

/* 共享锁 */
/*
C++17引入`shared_mutex`
C++14引入`shared_time_mutex`
C++11无,只能用boost库版
*/
class DNService {
 public:
  DNService() {}
  // 读操作使用共享锁
  std::string QueryDNS(std::string dnsName) {
    // shared_mutex:第二种箱子类型,区别于普通箱子`std::mutex`
    std::shared_lock<std::shared_mutex> sl(_shared_mtx);
    auto iter = _dns_info.find(dnsName);
    if (iter != _dns_info.end()) {
      return iter->second;
    }
    return "";
  }

  // 写操作使用独占锁
  void addDNSInfo(std::string dnsName, std::string dnsEntry) {
    
  }

 private:
  std::map<std::string, std::string> _dns_info;
  mutable std::shared_mutex _shared_mtx;
};

int main() {
  /* 6.共享锁 */
}
