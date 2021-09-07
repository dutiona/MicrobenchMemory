#pragma once

#include <string_view>

namespace MicrobenchMemory
{
  struct global_memory_informations
  {
    std::size_t allocation_count;
    std::size_t deallocation_count;
    std::size_t total_memory_allocated;

    bool has_memory_leak() const { return allocation_count != deallocation_count; }
  };

  struct scoped_memory_informations
  {
    std::size_t      allocation_count;
    std::size_t      deallocation_count;
    std::size_t      total_memory_allocated;
    std::size_t      total_memory_deallocated;
    std::size_t      nb_ptr_leaked;
    std::string_view scope;

    bool        has_memory_leak() const { return allocation_count != deallocation_count || total_memory_allocated != total_memory_deallocated; }
    std::size_t count_memory_leaked() const { return total_memory_allocated - total_memory_deallocated; }
    std::size_t count_ptr_leaked() const { return nb_ptr_leaked; }
  };

  global_memory_informations get_global_memory_information_snapshot();
  scoped_memory_informations get_memory_information_snapshot(std::string_view scope);

  namespace detail
  {
    [[nodiscard]] void* managed_new(std::size_t sz);
    [[nodiscard]] void* managed_new(std::size_t count, std::align_val_t al);
    [[nodiscard]] void* managed_new_array(std::size_t count);
    [[nodiscard]] void* managed_new_array(std::size_t count, std::align_val_t al);

    void managed_delete(void* ptr) noexcept;
    void managed_delete(void* ptr, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete(void* ptr, [[maybe_unused]] std::size_t sz) noexcept;
    void managed_delete(void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete_array(void* ptr) noexcept;
    void managed_delete_array(void* ptr, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete_array(void* ptr, [[maybe_unused]] std::size_t sz) noexcept;
    void managed_delete_array(void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept;

    [[nodiscard]] void* managed_new(std::string_view class_name, std::size_t sz);
    [[nodiscard]] void* managed_new(std::string_view class_name, std::size_t count, std::align_val_t al);
    [[nodiscard]] void* managed_new_array(std::string_view class_name, std::size_t count);
    [[nodiscard]] void* managed_new_array(std::string_view class_name, std::size_t count, std::align_val_t al);

    void managed_delete(std::string_view class_name, void* ptr) noexcept;
    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz) noexcept;
    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete_array(std::string_view class_name, void* ptr) noexcept;
    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::align_val_t al) noexcept;
    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz) noexcept;
    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept;
  } // namespace detail
} // namespace MicrobenchMemory

#define MICROBENCH_MEMORY_OVERLOAD_GLOBAL()                                                                                                                                        \
  [[nodiscard]] void* operator new(std::size_t sz) { return MicrobenchMemory::detail::managed_new(sz); }                                                                           \
  [[nodiscard]] void* operator new(std::size_t count, std::align_val_t al) { return MicrobenchMemory::detail::managed_new(count, al); }                                            \
                                                                                                                                                                                   \
  [[nodiscard]] void* operator new[](std::size_t count) { return MicrobenchMemory::detail::managed_new_array(count); }                                                             \
  [[nodiscard]] void* operator new[](std::size_t count, std::align_val_t al) { return MicrobenchMemory::detail::managed_new_array(count, al); }                                    \
                                                                                                                                                                                   \
  void operator delete(void* ptr) noexcept { return MicrobenchMemory::detail::managed_delete(ptr); }                                                                               \
  void operator delete(void* ptr, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete(ptr, al); }                                                      \
  void operator delete(void* ptr, std::size_t sz) noexcept { return MicrobenchMemory::detail::managed_delete(ptr, sz); }                                                           \
  void operator delete(void* ptr, std::size_t sz, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete(ptr, sz, al); }                                  \
                                                                                                                                                                                   \
  void operator delete[](void* ptr) noexcept { return MicrobenchMemory::detail::managed_delete_array(ptr); }                                                                       \
  void operator delete[](void* ptr, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete_array(ptr, al); }                                              \
  void operator delete[](void* ptr, std::size_t sz) noexcept { return MicrobenchMemory::detail::managed_delete_array(ptr, sz); }                                                   \
  void operator delete[](void* ptr, std::size_t sz, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete_array(ptr, sz, al); }

// Insert inside a class declaration
#define MICROBENCH_MEMORY_INSTRUMENT_CLASS(class_name)                                                                                                                             \
  [[nodiscard]] static void* operator new(std::size_t sz) { return MicrobenchMemory::detail::managed_new(class_name, sz); }                                                        \
  [[nodiscard]] static void* operator new(std::size_t count, std::align_val_t al) { return MicrobenchMemory::detail::managed_new(class_name, count, al); }                         \
                                                                                                                                                                                   \
  [[nodiscard]] static void* operator new[](std::size_t count) { return MicrobenchMemory::detail::managed_new_array(class_name, count); }                                          \
  [[nodiscard]] static void* operator new[](std::size_t count, std::align_val_t al) { return MicrobenchMemory::detail::managed_new_array(class_name, count, al); }                 \
                                                                                                                                                                                   \
  static void operator delete(void* ptr) noexcept { return MicrobenchMemory::detail::managed_delete(class_name, ptr); }                                                            \
  static void operator delete(void* ptr, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete(class_name, ptr, al); }                                   \
  static void operator delete(void* ptr, std::size_t sz) noexcept { return MicrobenchMemory::detail::managed_delete(class_name, ptr, sz); }                                        \
  static void operator delete(void* ptr, std::size_t sz, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete(class_name, ptr, sz, al); }               \
                                                                                                                                                                                   \
  static void operator delete[](void* ptr) noexcept { return MicrobenchMemory::detail::managed_delete_array(class_name, ptr); }                                                    \
  static void operator delete[](void* ptr, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete_array(class_name, ptr, al); }                           \
  static void operator delete[](void* ptr, std::size_t sz) noexcept { return MicrobenchMemory::detail::managed_delete_array(class_name, ptr, sz); }                                \
  static void operator delete[](void* ptr, std::size_t sz, std::align_val_t al) noexcept { return MicrobenchMemory::detail::managed_delete_array(class_name, ptr, sz, al); }
