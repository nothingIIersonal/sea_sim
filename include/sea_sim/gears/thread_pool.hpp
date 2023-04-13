#pragma once

#include <future>
#include <thread>
#include <vector>

#include "blocking_queue.hpp"
#include "task.hpp"

namespace gears
{

class StaticThreadPool
{
public:
  explicit StaticThreadPool(std::size_t workers_num);

  void SubmitTask(TaskType&& task);

  void Join();

private:
  void StartWorkers(std::size_t workers_num);

  void WorkerRoutine();

private:
  std::vector<std::thread> workers_;
  ::gears::BlockingQueue<TaskType> tasks_;
};

/**
 * @brief Runs task in thread pool asynchroniously
 * @tparam Function - task_type, that will be run in thread pool
 * @param pool - thread-pool where function will be executed
 * @param task - task, that will be executed
 * @return std::future<T> where T - function return value
 */
template <typename Function>
auto Async(StaticThreadPool& pool, Function&& task)
{

  using ReturnType = decltype(task());

  std::promise<ReturnType> promise;
  auto future = promise.get_future();

  auto task_wrapper = [promise = std::move(promise), task]() mutable {
    if constexpr (std::is_void_v<ReturnType>)
    {
      task();
    }
    else
    {
      ReturnType result = task();
      promise.set_value(std::move(result));
    }
  };

  pool.SubmitTask(std::move(task_wrapper));
  return future;
}

} // namespace gears