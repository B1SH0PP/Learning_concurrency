#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include "test_91.hpp"

int main() {
  Channel<int> ch(10);  // 10缓冲的channel

  std::thread producer([&]() {
    for (int i = 0; i < 15; ++i) {
      ch.send(i);
      std::cout << "Sent: " << i << std::endl;
    }
    ch.close();
  });

  std::thread consumer([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 延迟消费者500ms,等待一下生产者
    int val;
    while (ch.receive(val)) {
      std::cout << "Received: " << val << std::endl;
    }
  });
  
  producer.join();
  consumer.join();
  return 0;
}