#ifndef GENEBITS_ENGINE_UTIL_EVENTHANDLER_H
#define GENEBITS_ENGINE_UTIL_EVENTHANDLER_H

#include <type_traits>
#include <utility>

namespace genebits::engine
{
///
/// Concept used to determine if an Invokable is eligible to be binded to an EventHandler.
///
/// This concept is more restrictive than invokable binding for other delegates like
/// std::function. The justification is because we do not want to create additional overhead
/// for something we will rarely use.
///
/// @note
///     The size of the invokable matters. There are size restrictions.
///
/// @tparam Invokable Type of the invokable (usually a lambda).
/// @tparam Event Type of the event to invoke.
///
template<typename Invokable, typename Event>
concept EventHandlerInvokable = std::is_class_v<Invokable> && sizeof(Invokable) <= sizeof(void*) && requires(Invokable invokable, const Event& event)
{
  invokable(event);
};

///
/// Highly optimized delegate for handling events. Supports binding free functions,
/// member functions & sometimes invokables.
///
/// Zero overhead for member functions (the most commonly used). 8 byte memory overhead
/// for free functions. Maximum of 8 byte memory overhead for invokable. No call overhead.
///
/// @tparam Event Type of event for delegate to handle.
///
template<typename Event>
class EventHandler
{
public:
  ///
  /// Default Constructor
  ///
  constexpr EventHandler() noexcept
    : function_(nullptr), storage_(nullptr)
  {
  }

  ///
  /// Binds a free function.
  ///
  /// 8 bytes of memory overhead and no call overhead.
  ///
  /// @tparam FreeFunction Compile-time free function pointer.
  ///
  template<void (*FreeFunction)(const Event&)>
  constexpr void Bind() noexcept
  {
    storage_ = nullptr;

    function_ = [](void*, const Event& event)
    {
      (*FreeFunction)(event);
    };
  }

  ///
  /// Binds a member function.
  ///
  /// No overhead.
  ///
  /// @tparam MemberFunction Compile-time member function pointer.
  /// @tparam Type The type of class the member function is for.
  ///
  /// @param instance The instance to call the member function for.
  ///
  template<auto MemberFunction, typename Type>
  requires std::is_member_function_pointer_v<decltype(MemberFunction)>
  constexpr void Bind(Type* instance) noexcept
  {
    storage_ = instance;

    function_ = [](void* storage, const Event& event)
    {
      (static_cast<Type*>(storage)->*MemberFunction)(event);
    };
  }

  ///
  /// Bind an invokable.
  ///
  /// Invokable are restricted but still supported. Restrictions include
  /// a size of less or equal to a pointer size.
  ///
  /// No overhead of the invokable uses all the available size. Maximum of
  /// 8 bytes of overhead.
  ///
  /// @tparam Invokable The type of the invokable.
  ///
  /// @param invokable The instance of the invokable.
  ///
  template<EventHandlerInvokable<Event> Invokable>
  constexpr void Bind(Invokable&& invokable) noexcept
  {
    new (&storage_) Invokable(std::move(invokable));

    function_ = [](void* storage, const Event& event)
    {
      reinterpret_cast<Invokable*>(&storage)->operator()(event);
    };
  }

  ///
  /// Invokes the stored function with the specified event.
  ///
  /// @param event The event to invoke with.
  ///
  constexpr void Invoke(const Event& event)
  {
    function_(storage_, event);
  }

  ///
  /// Equality operator.
  ///
  /// @param other Other handler to compare.
  ///
  /// @return True if the handlers are equal, false otherwise.
  ///
  [[nodiscard]] constexpr bool operator==(const EventHandler<Event> other) const noexcept
  {
    return function_ == other.function_ && storage_ == other.storage_;
  }

  ///
  /// Inequality operator.
  ///
  /// @param other Other handler to compare.
  ///
  /// @return True if the handlers are unequal, false otherwise.
  ///
  [[nodiscard]] constexpr bool operator!=(const EventHandler<Event> other) const noexcept
  {
    return function_ != other.function_ || storage_ != other.storage_;
  }

  ///
  /// Bool operator.
  ///
  /// return True if the handler had been binded with a function, false otherwise.
  ///
  [[nodiscard]] constexpr operator bool() const noexcept
  {
    return function_ != nullptr;
  }

private:
  void (*function_)(void*, const Event&);
  void* storage_;
};

} // namespace genebits::engine

#endif