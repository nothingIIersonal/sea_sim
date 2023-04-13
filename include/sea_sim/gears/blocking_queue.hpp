#pragma once

#include <deque>
#include <optional>

namespace gears
{

/// Unbounded Blocking Multi-Producer/Multi-Consumer Queue
template <typename T>
class BlockingQueue
{
public:
  /**
   * @brief Получить значение из очереди.
   * В случае, если очередь пуста, то вызвавший поток блокируется, до тех пор, пока
   * в очереди не появится значение. Значение непосредственно из очереди будет удалено.
   * @return головное значение очереди.
   */
  T Take()
  {
    /**
     * Зашедший сюда поток, заблокируется, если в очереди нет задач.
     */
    std::unique_lock lock(mutex_);
    while (buffer_.empty())
    {
      /**
       * 1) Отпускаем мьютекс<br>
       * 2) Ждем пока в очереди появится хоть одна задача<br>
       * 3) Выходя из wait перезахватываем мьютекс <br>
       */
      queue_not_empty_.wait(lock);
    }
    return TakeLocked();
  }

  /**
   * @brief Попытаться взять значение из очереди. Если очередь пуста, то вернуть std::nullopt,
   * сигнализирующий о том, что в данный момент значений в очереди нет.
   * @return optional to front value if it exitsts or std::nullopt
   */
  std::optional<T> TryTake()
  {
    std::unique_lock lock(mutex_);
    if (buffer_.empty())
    {
      return std::nullopt;
    }
    return std::make_optional(std::move(TakeLocked()));
  }

  /**
   * @brief Добавить значение в конец очереди.
   * @param value - Вносимое в очередь значение.
   */
  void Put(T&& value)
  {
    {
      std::lock_guard guard(mutex_);
      buffer_.push_back(std::move(value));
    }
    queue_not_empty_.notify_one();
  }

  /**
   * @brief Добавить значение в конец очереди.
   * @param value - Вносимое в очередь значение.
   */
  void Put(const T& value)
  {
    {
      std::lock_guard guard(mutex_);
      buffer_.push_back(value);
    }
    queue_not_empty_.notify_one();
  }

private:
  /**
   * @brief Извлечение элемента из очереди.
   * НЕ ПОТОКОБЕЗОПАСЕН
   * @return TaskType
   */
  T TakeLocked()
  {
    auto result = std::move(buffer_.front());
    buffer_.pop_front();
    return result;
  }

private:
  std::deque<T> buffer_;                    /// protected by mutex_
  std::condition_variable queue_not_empty_; /// Очередь не пуста.
  mutable std::mutex mutex_;
};




} // namespace gears
