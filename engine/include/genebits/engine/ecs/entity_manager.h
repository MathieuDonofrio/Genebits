#ifndef GENEBITS_ENGINE_ECS_ENTITY_MANAGER_H
#define GENEBITS_ENGINE_ECS_ENTITY_MANAGER_H

#include <concepts>

#include "genebits/engine/util/fast_vector.h"

namespace genebits::engine
{
/**
 * Responsible for providing and recycling of entity identifiers.
 *
 * @tparam Entity Entity integral type to generate.
 * @tparam AllocatorImpl Allocator to use.
 */
template<std::unsigned_integral Entity, Allocator AllocatorImpl = Mallocator>
class EntityManager
{
public:
  /**
   * Constructor.
   */
  constexpr EntityManager() noexcept : current_(0) {}

  EntityManager(const EntityManager&) = delete;
  EntityManager(EntityManager&&) = delete;
  EntityManager& operator=(const EntityManager&) = delete;
  EntityManager& operator=(EntityManager&&) = delete;

  /**
   * Obtain a unique entity identifier.
   *
   * If there are entity identifiers that have been recycled, those will be returned first,
   * otherwise a new identifier will be generated.
   *
   * @return Unique entity identifier.
   */
  [[nodiscard]] Entity Obtain()
  {
    if (recycled_.Empty()) return Generate();
    else
    {
      auto entity = recycled_.back();
      recycled_.PopBack();

      return entity;
    }
  }

  /**
   * Generates a new entity identifier.
   *
   * Fetches current sequence value and increments it after returning the value;
   *
   * @note
   *    Sequence starts at 0.
   *
   * @return New unique entity identifier.
   */
  [[nodiscard]] Entity Generate()
  {
    return current_++;
  }

  /**
   * Releases the entity identifier allowing it to be reused.
   *
   * @param[in] entity Entity identifier to release.
   */
  void Release(const Entity entity) noexcept
  {
    ASSERT(entity <= current_, "Entity not from this manager");

    recycled_.PushBack(entity);
  }

  /**
   * Releases all the entity identifiers and resets the generator sequence to 0.
   *
   * This method should be preferred to releasing every entity one by one.
   *
   * This operation is O(1).
   */
  void ReleaseAll() noexcept
  {
    recycled_.Clear();

    current_ = 0;
  }

  /**
   * Returns the amount of entity identifier currently circulating.
   *
   * @return Amount of entity identifiers currently circulating.
   */
  [[nodiscard]] size_t CirculatingCount() const noexcept
  {
    return current_ - recycled_.Size();
  }

  /**
   * Returns the amount of entity identifiers ready to be reused.
   *
   * While this is bigger than 0, no new identifier will be generating when
   * obtaining unless explicitly generated.
   *
   * @return Amount of entity identifiers currently recycled.
   */
  [[nodiscard]] size_t RecycledCount() const noexcept
  {
    return recycled_.Size();
  }

private:
  Entity current_;
  FastVector<Entity, AllocatorImpl> recycled_;
};

} // namespace genebits::engine

#endif
