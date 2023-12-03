/**********************************************************************
 * @brief A thread-safe queue, based off of the implementation in "C++
 *Concurrency in Action" by Anthony Williams. I eventually want to actually
 *write my own parallel libraries but am currently still learning the basics.
 * @details
 * @author Original author is Anthony Williams.
 * @copyright I don't know.
 * @date 2023
 ***********************************************************************/

#pragma once

#include <condition_variable>
#include <memory>
#include <thread>

namespace utils {
namespace parallel {

template <typename T>
class threadsafe_queue {
private:
  struct node {
    std::shared_ptr<T> data;
    std::unique_ptr<node> next;
  };

private:
  node *get_tail();
  std::unique_ptr<node> pop_head();
  std::unique_ptr<node> try_pop_head();
  std::unique_ptr<node> try_pop_head(T &value);
  std::unique_lock<std::mutex> wait_for_data();
  std::unique_ptr<node> wait_pop_head();
  std::unique_ptr<node> wait_pop_head(T &value);

public:
  threadsafe_queue() : head(new node), tail(head.get()) {}
  threadsafe_queue(const threadsafe_queue &other) = delete;
  threadsafe_queue &operator=(const threadsafe_queue &other) = delete;

  /**
   * @brief Tries to pop the value from the front of the queue.
   *
   * @details If the queue is not empty, removes the front item from the queue
   * and returns it. If the queue is empty, returns nullptr immediately.
   * This operation does not block.
   *
   * @return std::shared_ptr<T> Shared pointer to the popped value,
   * or nullptr if the queue was empty.
   */
  std::shared_ptr<T> try_pop();

  /**
   * @brief Tries to pop the value from the front of the queue.
   *
   * @details If the queue is not empty, removes the front item from the queue
   * and stores it in 'value'. If the queue is empty, returns false immediately.
   * This operation does not block.
   *
   * @param value Reference to a T where the popped value will be stored.
   * @return bool true if a value was successfully popped, false otherwise.
   */
  bool try_pop(T &value);

  /**
   * @brief Pops the value from the front of the queue.
   *
   * @details If the queue is empty, this operation will block until
   * an item becomes available due to a call to push(). Once an item
   * is available or if the queue was not empty to begin with,
   * it removes the front item from the queue and returns it.
   *
   * @return std::shared_ptr<T> Shared pointer to the popped value.
   */
  std::shared_ptr<T> wait_and_pop();

  /**
   * @brief Pops the value from the front of the queue.
   *
   * @details If the queue is empty, this operation will block until
   * an item becomes available due to a call to push(). Once an item
   * is available or if the queue was not empty to begin with,
   * it removes the front item from the queue.
   *
   * @param value Reference to a T where the popped value will be stored.
   * @return void
   */
  void wait_and_pop(T &value);

  /**
   * @brief Pushes a new value to the end of the queue.
   *
   * @details The new value is copied into the queue. If there are threads
   * blocked on wait_and_pop(), one of them will be woken up.
   *
   * @param new_value The value to push onto the queue.
   */
  void push(T new_value);

  /**
   * @brief Checks if the queue is empty.
   *
   * @details Note that due to concurrent access, a check that the queue
   * is empty may be invalid by the time the function returns. Therefore,
   * empty() should not be used to decide whether to call try_pop() or
   * wait_and_pop().
   *
   * @return bool true if the queue is empty, false otherwise.
   */
  bool empty();

private:
  std::mutex head_mutex;
  std::unique_ptr<node> head;
  std::mutex tail_mutex;
  node *tail;
  std::condition_variable data_cond;
};

// ===== IMPLEMENTATION =====

template <typename T>
typename threadsafe_queue<T>::node *threadsafe_queue<T>::get_tail() {
  std::lock_guard<std::mutex> tail_lock(tail_mutex);
  return tail;
}

template <typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::pop_head() {
  std::unique_ptr<node> old_head = std::move(head);
  head = std::move(old_head->next);
  return old_head;
}

template <typename T>
typename std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::try_pop_head() {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  if (head.get() == get_tail()) {
    return std::unique_ptr<node>();
  }
  return pop_head();
}

template <typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::try_pop_head(T &value) {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  if (head.get() == get_tail()) {
    return std::unique_ptr<node>();
  }
  value = std::move(*head->data);
  return pop_head();
}

template <typename T>
std::unique_lock<std::mutex> threadsafe_queue<T>::wait_for_data() {
  std::unique_lock<std::mutex> head_lock(head_mutex);
  data_cond.wait(head_lock, [&] { return head.get() != get_tail(); });
  return head_lock;
}

template <typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::wait_pop_head() {
  std::unique_lock<std::mutex> head_lock(wait_for_data());
  return pop_head();
}

template <typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::wait_pop_head(T &value) {
  std::unique_lock<std::mutex> head_lock(wait_for_data());
  value = std::move(*head->data);
  return pop_head();
}

template <typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop() {
  std::unique_ptr<node> old_head = try_pop_head();
  return old_head ? old_head->data : std::shared_ptr<T>();
}

template <typename T>
bool threadsafe_queue<T>::try_pop(T &value) {
  std::unique_ptr<node> const old_head = try_pop_head(value);
  return old_head != nullptr;
}

template <typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop() {
  std::unique_ptr<node> const old_head = wait_pop_head();
  return old_head->data;
}

template <typename T>
void threadsafe_queue<T>::wait_and_pop(T &value) {
  std::unique_ptr<node> const old_head = wait_pop_head(value);
}

template <typename T>
void threadsafe_queue<T>::push(T new_value) {
  std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
  std::unique_ptr<node> p(new node);

  // New brace-enclosed scope so that the lock_guard is released as soon as it
  // is no longer necessary.
  {
    std::lock_guard<std::mutex> tail_lock(tail_mutex);
    tail->data = new_data;
    node *const new_tail = p.get();
    tail->next = std::move(p);
    tail = new_tail;
  }
  data_cond.notify_one();
}
template <typename T>
bool threadsafe_queue<T>::empty() {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  return (head.get() == get_tail());
}

} // namespace parallel

} // namespace utils
