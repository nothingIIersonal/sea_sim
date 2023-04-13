#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>

#include "blocking_queue.hpp"

namespace gears
{

namespace fdx /// full-duplex channel
{
/**
 * @brief Механизм обмена данными между thread-ами.
 * Канал удобно представить как две соединенные накрест.<br>
 *         WRITE(A) -> READ(B)  -- UPLINE
 *         WRITE(B) -> READ(A)  -- DOWNLINE
 * Данные образуют очередь. Коммуникация осуществляется через Endpoint-ы
 * Между Endpoint-ами находится буфер, в котором
 * содержатся данные, до которых еще не дошла очередь.
 * Endpoint-ы двусторонние, т.е поддерживается и чтение и запись данных.
 */
namespace detail
{

/**
 * @brief Хранит обмениваемые данные.
 * @tparam T - тип обмениваемых данных.
 */
template <typename T>
class ChannelStorage
{
public:
  /// @brief rule of five https://en.cppreference.com/w/cpp/language/rule_of_three
  ChannelStorage() = default;
  ChannelStorage(ChannelStorage&&) = default;
  ~ChannelStorage() = default;

  ChannelStorage(const ChannelStorage&) = delete;
  ChannelStorage& operator=(const ChannelStorage&) = delete;

  /**
   * @brief Переслать значение по UPLINE.
   * @param value
   */
  void PushToUpline(T&& value)
  {
    upline_.Put(std::move(value));
  }

  /**
   * @brief Переслать значение по UPLINE.
   * @param value
   */
  void PushToUpline(const T& value)
  {
    upline_.Put(value);
  }

  /**
   * @brief Переслать значение по DOWNLINE.
   * @param value
   */
  void PushToDownline(T&& value)
  {
    downline_.Put(std::move(value));
  }

  /**
   * @brief Переслать значение по DOWNLINE.
   * @param value
   */
  void PushToDownline(const T& value)
  {
    downline_.Put(value);
  }

  /**
   * @brief Считать значение из UPLINE.
   * Если сообщений нет, то вызывающий поток заблокируется
   * до тех пор, пока сообщение не появится. Выход из функции возможен только со значением.
   */
  T TakeUpline()
  {
    return upline_.Take();
  }

  /**
   * @brief Считать значение из DOWNLINE.
   * Если сообщений нет, то вызывающий поток заблокируется
   * до тех пор, пока сообщение не появится. Выход из функции возможен только со значением.
   */
  T TakeDownline()
  {
    return downline_.Take();
  }

  /**
   * @brief Попытаться считать значение из UPLINE.
   * Если сообщений нет, то вызывающий поток получит std::nullopt
   * Иначе - std::optional содержащее значение.
   */
  std::optional<T> TryTakeUpline()
  {
    return upline_.TryTake();
  }

  /**
   * @brief Попытаться считать значение из DOWNLINE.
   * Если сообщений нет, то вызывающий поток получит std::nullopt
   * Иначе - std::optional содержащее значение.
   */
  std::optional<T> TryTakeDownline()
  {
    return downline_.TryTake();
  }

private:
  ::gears::BlockingQueue<T> upline_;
  ::gears::BlockingQueue<T> downline_;
};

/// @brief Умный указатель на ChannelStorage
template <typename T>
using ChannelStoragePtr = std::shared_ptr<::gears::fdx::detail::ChannelStorage<T>>;

} // namespace detail

/**
 * @brief Конечная точка, через которую осуществляется коммуникация.
 * @tparam T - тип пересылаемых значений.
 */
template <typename T>
class ChannelEndpoint
{
private:
  /// @brief Тип линии канала, на которой будет осуществляться коммуникация
  enum class Line
  {
    UPLINE,
    DOWNLINE,
  };

  /// @brief Конструктор
  ChannelEndpoint(::gears::fdx::detail::ChannelStoragePtr<T> channel_ptr, Line role)
      : channel_(channel_ptr) /// данные канала
      , line_(role)           /// линия по которой идет общение
  {
  }

public:
  /// @brief rule of five https://en.cppreference.com/w/cpp/language/rule_of_three
  ChannelEndpoint(const ChannelEndpoint&) = default;
  ChannelEndpoint(ChannelEndpoint&&) = default;
  ChannelEndpoint& operator=(ChannelEndpoint&&) = default;
  ChannelEndpoint& operator=(const ChannelEndpoint&) = default;
  ~ChannelEndpoint() = default;

  /**
   * @brief Записать значение в канал
   * @param value - отправляемое значение
   */
  const ChannelEndpoint& SendData(T&& value) const
  {
    switch (line_)
    {
    case Line::UPLINE:
      channel_->PushToUpline(std::move(value));
      break;
    case Line::DOWNLINE:
      channel_->PushToDownline(std::move(value));
      break;
    }
    return *this;
  }

  /**
   * @brief Записать значение в канал
   * @param value - отправляемое значение
   */
  const ChannelEndpoint& SendData(const T& value) const
  {
    switch (line_)
    {
    case Line::UPLINE:
      channel_->PushToUpline(value);
      break;
    case Line::DOWNLINE:
      channel_->PushToDownline(value);
      break;
    }
    return *this;
  }

  /**
   * @brief Блокирующее чтение. Если сообщений нет, то поток блокируется.
   * @param value - отправляемое значение
   */
  T BlockingRead() const
  {
    switch (line_)
    {
    case Line::UPLINE:
      return channel_->TakeDownline();
    case Line::DOWNLINE:
      return channel_->TakeUpline();
    }
  }

  /**
   * @brief Не блокирующее чтение.
   * Если сообщений нет - вернет std::nullopt.
   * Если есть - std::optional<T>
   */
  std::optional<T> TryRead() const
  {
    switch (line_)
    {
    case Line::UPLINE:
      return channel_->TryTakeDownline();
    case Line::DOWNLINE:
      return channel_->TryTakeUpline();
    }
    return std::nullopt;
  }

  /// @brief Создание канала.
  template <typename U>
  friend auto MakeChannel();

private:
  mutable ::gears::fdx::detail::ChannelStoragePtr<T> channel_;
  const Line line_;
};

/**
 *
 * @tparam T - передаваемый тип данных
 * @return пару эндпоинтов.<br>
 *
 * @example auto [r,w] = gears::fdx::MakeChannel<int>();
 *
 * r - gears::fdx::ChannelEndpoint <br>
 * w - gears::fdx::ChannelEndpoint <br>
 */
template <typename T>
auto MakeChannel()
{
  auto storage_ptr = std::make_shared<::gears::fdx::detail::ChannelStorage<T>>();
  return std::make_pair<ChannelEndpoint<T>>(
      ::gears::fdx::ChannelEndpoint<T>(storage_ptr, ::gears::fdx::ChannelEndpoint<T>::Line::UPLINE),
      ::gears::fdx::ChannelEndpoint<T>(storage_ptr, ::gears::fdx::ChannelEndpoint<T>::Line::DOWNLINE));
}

} // namespace fdx

namespace hdx /// half-duplex channel
{

namespace detail
{

template <typename T>
class ChannelStorage
{
public:
  /// @brief rule of five https://en.cppreference.com/w/cpp/language/rule_of_three
  ChannelStorage() = default;
  ChannelStorage(ChannelStorage&&) = default;
  ~ChannelStorage() = default;

  ChannelStorage(const ChannelStorage&) = delete;
  ChannelStorage& operator=(const ChannelStorage&) = delete;

  /**
   * Кладет данные в буфер
   * @param value - Отправляемые данные.
   */
  void PushData(T&& value)
  {
    buffer_.Put(std::move(value));
  }

  /**
   * Кладет данные в буфер
   * @param value - Отправляемые данные.
   */
  void PushData(const T& value)
  {
    buffer_.Put(value);
  }

  T BlockingFetchData()
  {
    return buffer_.Take();
  }

  std::optional<T> TryFetchData()
  {
    return buffer_.TryTake();
  }

private:
  ::gears::BlockingQueue<T> buffer_;
};

template <typename T>
using ChannelStoragePtr = std::shared_ptr<::gears::hdx::detail::ChannelStorage<T>>;

} // namespace detail

template <typename T>
class ReadChannelEndpoint
{
private:
  explicit ReadChannelEndpoint(::gears::hdx::detail::ChannelStoragePtr<T> storage)
      : storage_(storage)
  {
  }

public:
  ReadChannelEndpoint(ReadChannelEndpoint&&) = default;
  ReadChannelEndpoint& operator=(ReadChannelEndpoint&&) = default;
  ~ReadChannelEndpoint() = default;

  /// Noncopyable
  ReadChannelEndpoint(const ReadChannelEndpoint&) = delete;
  ReadChannelEndpoint& operator=(const ReadChannelEndpoint&) = delete;

  T BlockingRead() const
  {
    return storage_->BlockingFetchData();
  }

  std::optional<T> TryRead() const
  {
    return storage_->TryFetchData();
  }

  template <typename U>
  friend auto MakeChannel();

private:
  mutable ::gears::hdx::detail::ChannelStoragePtr<T> storage_;
};

template <typename T>
class WriteChannelEndpoint
{
private:
  explicit WriteChannelEndpoint(::gears::hdx::detail::ChannelStoragePtr<T> storage)
      : storage_(storage)
  {
  }

public:
  ~WriteChannelEndpoint() = default;
  WriteChannelEndpoint(WriteChannelEndpoint&&) = default;
  WriteChannelEndpoint& operator=(WriteChannelEndpoint&&) = default;

  /// Copyable
  WriteChannelEndpoint(const WriteChannelEndpoint&) = default;
  WriteChannelEndpoint& operator=(const WriteChannelEndpoint&) = default;

  const WriteChannelEndpoint& SendData(const T& value) const
  {
    storage_->PushData(value);
    return *this;
  }

  const WriteChannelEndpoint& SendData(T&& value) const
  {
    storage_->PushData(std::move(value));
    return *this;
  }

  template <typename U>
  friend auto MakeChannel();

private:
  mutable ::gears::hdx::detail::ChannelStoragePtr<T> storage_;
};

/**
 *
 * @tparam T - передаваемый тип данных
 * @return пару эндпоинтов.<br>
 * Первый - ответственный за запись в канал<br>
 * Второй - за чтение из канала<br>
 *
 * @example auto [r,w] = gears::hdx::MakeChannel<int>();
 *
 * r - gears::hdx::ReadChannelEndpoint <br>
 * w - gears::hdx::WriteChannelEndpoint <br>
 */
template <typename T>
auto MakeChannel()
{
  auto storage_ptr = std::make_shared<::gears::hdx::detail::ChannelStorage<T>>();
  return std::make_pair(::gears::hdx::ReadChannelEndpoint<T>(storage_ptr),
                        ::gears::hdx::WriteChannelEndpoint<T>(storage_ptr));
}

} // namespace hdx

} // namespace gears