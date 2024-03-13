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
    /*
    shared_mutex:另一种互斥量(区别于普通箱子`std::mutex`),其允许多个线程访问(读取)共享数据,但是只允许
                  一个线程独占数据.
                  理解为可以看到箱子里是啥,但是不允许打开.
    这里用`shared_lock上锁`shared_mutex`后,共享数据只可读不可写.
    */
    std::shared_lock<std::shared_mutex> sl(_shared_mtx);
    auto iter = _dns_info.find(dnsName);
    if (iter != _dns_info.end()) {
      return iter->second;
    }
    return "";
  }

  // 写操作使用独占锁
  void AddDNSInfo(std::string dnsName, std::string dnsEntry) {
    /*
    这里用`lock_guard`上锁`shared_mutex`后,共享数据依然可被其他线程读取,但是只有本线程函数可以写.
    */
    std::lock_guard<std::shared_mutex> lg(_shared_mtx);
    _dns_info.insert(std::make_pair(dnsName, dnsEntry));
  }

 private:
  std::map<std::string, std::string> _dns_info;
  mutable std::shared_mutex _shared_mtx;
};

int main() {
  /* 2.共享锁 */
  // 测试的不对,不重要.主要是理解该锁抽象的用法

  // DNService obj;
  // std::thread t1(&DNService::QueryDNS, &obj, "DNS_WSSB");
  // std::thread t2(&DNService::AddDNSInfo, &obj, "DNS_Name","DNS_Entry");
  // t1.join();
  // t2.join();
}
