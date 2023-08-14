/**********************************************************************
 * @brief A Threadpool based heavily (almost verbatim) on Anthony Williams'
 *implementation in "C++ Concurrency in Action".
 * @details
 * @author Based on code by Anthony Williams.
 * @copyright I don't know.
 * @date 2023
 ***********************************************************************/

#pragma once

#include "parallel/function_wrapper.hpp"
#include "parallel/threadsafe_queue.hpp"

#include <future>
#include <vector>

namespace utils {
namespace parallel {

/**
 * A simple class for joining threads.
 */
class thread_joiner {
  std::vector<std::thread> &threads;

public:
  explicit thread_joiner(std::vector<std::thread> &threads_)
      : threads{threads_} {}

  ~thread_joiner() {
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

/**
 * A very simple thread pool for independent tasks.
 */
class thread_pool {
  std::atomic_bool done;
  threadsafe_queue<function_wrapper> work_queue;
  std::vector<std::thread> threads;
  thread_joiner joiner;

  void worker_thread() {
    while (!done) {
      function_wrapper task;
      if (work_queue.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield();
      }
    }
  }

public:
  thread_pool(std::size_t thread_count = std::thread::hardware_concurrency() -
                                         1)
      : done{false}, joiner{threads} {

    try {
      for (std::size_t i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(&thread_pool::worker_thread, this));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }

  ~thread_pool() { done = true; }

  template <typename FunctionType>
  std::future<typename std::invoke_result_t<FunctionType>>
  submit(FunctionType f) {
    using result_type = typename std::invoke_result_t<FunctionType>;

    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    work_queue.push(std::move(task));
    return res;
  }
};

} // namespace parallel

} // namespace utils
