#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

template <typename Iterator, typename T>
struct accumulate_block {
  void operator()(Iterator first, Iterator last, T& result) {
    result = std::accumulate(first, last, result);
  }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
  unsigned long const length = std::distance(first, last);
  if (!length) return init;
  unsigned long const min_per_thread = 25;
  unsigned long const max_threads =
      (length + min_per_thread - 1) / min_per_thread;
  unsigned long const hardware_threads = std::thread::hardware_concurrency();
  unsigned long const num_threads =
      std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
  unsigned long const block_size = length / num_threads;
  std::vector<T> results(num_threads);
  std::vector<std::thread> threads(num_threads - 1);  //`-1`是剔除主线程
  Iterator block_start = first;

  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);  //跳转到当前线程终止位置
    threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start,
                             block_end, std::ref(results[i]));
    block_start = block_end;
  }
  accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);

  for (auto& entry : threads) entry.join();
  return std::accumulate(results.begin(), results.end(), init);
}

void use_parallel_acc() {
  std::vector<int> vec(10000);
  for (int i = 0; i < 10000; i++) {
    vec.push_back(i);
  }

  int sum = 0;
  /*
  有点小问题 ???
  这里计算`vec`元素数量的方式还是有点问题,因为实际`vec`开辟的空间是要比实际插入的数要大的,用首尾迭代器
  的坐标间的`distance`作为元素数量不太严谨.
  */
  sum = parallel_accumulate<std::vector<int>::iterator, int>(vec.begin(), vec.end(), sum);
  std::cout << "--- sum is: " << sum << std::endl;
}

int main() {
  /* 4.加法并行计算 */
  use_parallel_acc();
}