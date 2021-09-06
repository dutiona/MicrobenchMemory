#pragma once

#include <new>

namespace MicrobenchMemory
{
  [[nodiscard] void* managed_new(std::size_t sz);
  [[nodiscard] void* managed_new(std::size_t count, std::align_val_t al);
  [[nodiscard] void* managed_new_array(std::size_t count );
  [[nodiscard] void* managed_new_array(std::size_t count,std::align_val_t al);

  void managed_delete(void* ptr) noexcept;
  void managed_delete(void* ptr, std::align_val_t al) noexcept;
  void managed_delete(void* ptr, std::size_t sz) noexcept;
  void managed_delete(void* ptr, std::size_t sz, std::align_val_t al) noexcept;
  void managed_delete_array( void* ptr) noexcept;
  void managed_delete_array( void* ptr, std::align_val_t al) noexcept;
  void managed_delete_array( void* ptr, std::size_t sz) noexcept;
  void managed_delete_array( void* ptr, std::size_t sz, std::align_val_t al) noexcept;
}
