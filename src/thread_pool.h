#pragma once
#include <functional>
#include <thread>
#include <vector>

namespace socks {

class ThreadPool {
 public:
  explicit ThreadPool(size_t thread_num);

  void AddTask(std::function<void()> task);

 private:
  bool stop_{false};
  std::vector<std::thread> threads_;
};

}  // namespace socks
