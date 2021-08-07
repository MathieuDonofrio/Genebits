#ifndef GENEBITS_ENGINE_UTIL_FASTVECTOR_H
#define GENEBITS_ENGINE_UTIL_FASTVECTOR_H

#include <iterator>
#include <type_traits>
#include <utility>

#include "engine/util/allocator.h"

namespace genebits::engine
{
// Make vector faster than std::vector in certain cases by making these optimizations:
// - Less flexibility: harder to use wrong
// - Unordered: Some possibilities to optimize
// - Genebits Allocators: Less overhead than std::allocator & more strategies flexibility
// - POD Optimizations: Example using c realloc

template<typename Type>
concept FastVectorType = std::is_copy_constructible_v<Type> || std::is_move_constructible_v<Type>;

template<FastVectorType Type, Allocator AllocatorImpl = Mallocator>
class FastVector
{
public:
  static constexpr bool cSmallType = sizeof(Type) <= 2 * sizeof(void*);
  static constexpr bool cTypePassedByValue = cSmallType && std::is_trivially_copy_constructible_v<Type>;

  using OptimalPassType = std::conditional_t<cTypePassedByValue, Type, Type&>;

  FastVector()
    : array_(nullptr),
      size_(0), capacity_(0)
  {
  }

  ~FastVector()
  {
    Destroy();

    allocator_.Deallocate(Block { array_, capacity_ });
  }

  void PushBack(const OptimalPassType value) noexcept
  {
    PrepareInsertion();
    new (array_[size_++]) Type(value);
  }

  void PushBack(Type&& value) noexcept
  {
    PrepareInsertion();
    new (array_[size_++]) Type(std::move(value));
  }

  void PopBack() noexcept
  {
    array_[--size_].~Type();
  }

  [[nodiscard]] constexpr size_t Size() const noexcept
  {
    return size_;
  }

  [[nodiscard]] constexpr size_t Capacity() const noexcept
  {
    return capacity_;
  }

  [[nodiscard]] constexpr bool Empty() const noexcept
  {
    return size_ == 0;
  }

protected:
  void Grow(uint32_t min_capacity) noexcept
  {
    if constexpr (!std::is_trivially_copy_constructible_v<Type> && !std::is_trivially_move_constructible_v<Type>)
    {
      Block block = allocator_.Allocate(min_capacity);

      for (uint32_t i = 0; i != size_; i++)
      {
        Type* old_element = array_ + i;
        Type* new_element = reinterpret_cast<Type*>(block.ptr) + i;

        if constexpr (cTypePassedByValue)
        {
          new (new_element) Type(*old_element);
        }
        else if constexpr (std::is_move_constructible_v<Type>)
        {
          new (new_element) Type(std::move(*old_element));
        }
        else
        {
          new (new_element) Type(*old_element);
        }

        if constexpr (!std::is_trivially_destructible_v<Type>)
        {
          old_element->~Type();
        }
      }

      allocator_.Deallocate(Block { array_, capacity_ });

      array_ = reinterpret_cast<Type*>(block.ptr);
      capacity_ = static_cast<uint32_t>(block.size);
    }
    else
    {
      Block block { array_, capacity_ };

      allocator_.Reallocate(block, min_capacity);

      array_ = reinterpret_cast<Type*>(block.ptr);
      capacity_ = static_cast<uint32_t>(block.size);
    }
  }

  void GoldenGrow() noexcept
  {
    // We want the growth rate to be close to the golden ratio (~1.618). The golden ration
    // maximizes reuse and is the optimal reallocation size. For simplicity and calculation
    // speed we use 1.5 and small constant amount.
    const uint32_t new_capacity = capacity_ + (capacity_ >> 1) + 8; // capacity * 1.5 + 8

    Grow(new_capacity);
  }

  void PrepareInsertion() noexcept
  {
    if (size_ == capacity_) [[unlikely]]
      GoldenGrow();
  }

  void Destroy() noexcept
  {
    if constexpr (!std::is_trivially_destructible_v<Type>)
    {
      for (uint32_t i = 0; i < size_; i++)
      {
        array_[i].~Type();
      }
    }
  }

public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = Type;
  using iterator = Type*;
  using const_iterator = const Type*;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  using reference = Type&;
  using const_reference = const Type&;
  using pointer = Type*;
  using const_pointer = const Type*;

  // Forward iterator creation methods.
  iterator begin() { return array_; }
  const_iterator begin() const { return array_; }
  iterator end() { return array_ + size_; }
  const_iterator end() const { return array_ + size_; }

  // Explicit const forward iterator creation methods.
  const_iterator cbegin() const { return array_; }
  const_iterator cend() const { return array_ + size_; }

  // Reverse iterator creation methods.
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

  pointer data() { return pointer(begin()); }
  const_pointer data() const { return const_pointer(begin()); }

  reference front() { return begin()[0]; }
  const_reference front() const { return begin()[0]; }
  reference back() { return end()[-1]; }
  const_reference back() const { return end()[-1]; }

  [[nodiscard]] const_reference operator[](const size_type index) const noexcept
  {
    return array_[index];
  }

  [[nodiscard]] reference operator[](const size_type index) noexcept
  {
    return array_[index];
  }

private:
  Type* array_;

  // We use 32 bit size & capacity to reduce the size of the vector.
  uint32_t size_;
  uint32_t capacity_;

  [[no_unique_address]] AllocatorImpl allocator_;
};

} // namespace genebits::engine

#endif
