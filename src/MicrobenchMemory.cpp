#include "MicrobenchMemory/MicrobenchMemory.hpp"

#include <cstdio>
#include <cstdlib>
#include <map>
#include <new>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace std::literals;

namespace MicrobenchMemory
{
  namespace detail
  {
    inline constexpr auto global_scope_name = "__global__"sv;

    struct internal_ptr_infos
    {
      std::size_t size  = 0;
      bool        freed = false;
    };

    template <typename T>
    struct vanilla_allocator : std::allocator<T>
    {
      using value_type                             = typename std::allocator<T>::value_type;
      using size_type                              = typename std::allocator<T>::size_type;
      using difference_type                        = typename std::allocator<T>::difference_type;
      using propagate_on_container_move_assignment = typename std::allocator<T>::propagate_on_container_move_assignment;


      vanilla_allocator() {}

      template <typename U>
      vanilla_allocator(const vanilla_allocator<U>& u)
        : std::allocator<T>(u)
      {
      }

      [[nodiscard]] constexpr T* allocate(size_type size)
      {
        void* p = std::malloc(size * sizeof(T));
        if (p == 0)
        {
          throw std::bad_alloc();
        }
        return static_cast<T*>(p);
      }

      constexpr void deallocate(T* p, size_type) { std::free(p); }
    };

    class internal_memory_information
    {
    public:
      internal_memory_information()
        : allocation_count_(0)
        , deallocation_count_(0)
        , total_memory_allocated_(0)
        , total_memory_deallocated_(0)
        , memory_info_{}
      {
      }

      void log_alloc(void* ptr, std::size_t sz)
      {
        // log the information
        ++allocation_count_;
        total_memory_allocated_ += sz;
        memory_info_.emplace(ptr, internal_ptr_infos{sz, false});
      }

      void log_dealloc(void* ptr)
      {
        // free non-allocated address detection
        if (!memory_info_.contains(ptr))
        {
          char buff[256];
          std::snprintf(buff, 256, "Attempting to free address <%p> which was not allocated.", ptr);
          throw std::runtime_error(buff);
        }

        auto& ptr_info = memory_info_[ptr];

        // double-free detection
        if (ptr_info.freed)
        {
          char buff[256];
          std::snprintf(buff, 256, "Double free on address <%p>..", ptr);
          throw std::runtime_error(buff);
        }

        // log the information
        ++deallocation_count_;
        total_memory_deallocated_ += ptr_info.size;
      }

      auto list_ptr_leaked() const
      {
        std::vector<void*, vanilla_allocator<void*>> ptrs;
        for (auto [ptr, infos] : memory_info_)
          if (!infos.freed)
            ptrs.push_back(ptr);
        return ptrs;
      }

      bool        has_memory_leak() const { return allocation_count_ != deallocation_count_ || total_memory_allocated_ != total_memory_deallocated_; }
      std::size_t count_memory_leaked() const { return total_memory_allocated_ - total_memory_deallocated_; }
      std::size_t count_ptr_leaked() const { return list_ptr_leaked().size(); }

      memory_informations to_user_info(std::string_view scope) const
      {
        return {allocation_count_, deallocation_count_, total_memory_allocated_, total_memory_deallocated_, count_ptr_leaked(), scope};
      }

    private:
      std::size_t                                                                                                          allocation_count_;
      std::size_t                                                                                                          deallocation_count_;
      std::size_t                                                                                                          total_memory_allocated_;
      std::size_t                                                                                                          total_memory_deallocated_;
      std::map<void*, internal_ptr_infos, std::less<void*>, vanilla_allocator<std::pair<void* const, internal_ptr_infos>>> memory_info_;
    };

    using scoped_memory_information_storage_t =
        std::map<std::string_view, internal_memory_information, std::less<std::string_view>, vanilla_allocator<std::pair<const std::string_view, internal_memory_information>>>;

    static scoped_memory_information_storage_t  __memory_informations = {{global_scope_name, internal_memory_information{}}};
    static scoped_memory_information_storage_t& get_global_memory_information() { return __memory_informations; }

    static void log_memory_allocation(std::string_view scope, void* ptr, std::size_t sz)
    {
      auto& minfo_map = get_global_memory_information();
      minfo_map.try_emplace(scope, internal_memory_information{});
      minfo_map[scope].log_alloc(ptr, sz);
    }

    static void log_memory_deallocation(std::string_view scope, void* ptr)
    {
      auto& minfo_map = get_global_memory_information();
      minfo_map.try_emplace(scope, internal_memory_information{});
      minfo_map[scope].log_dealloc(ptr);
    }

    [[nodiscard]] void* managed_new(std::size_t sz)
    {
      auto* ptr = std::malloc(sz);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(global_scope_name, ptr, sz);
      return ptr;
    }

    [[nodiscard]] void* managed_new(std::size_t count, std::align_val_t al)
    {
      auto* ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(global_scope_name, ptr, count);
      return ptr;
    }

    [[nodiscard]] void* managed_new_array(std::size_t count)
    {
      auto* ptr = std::malloc(count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(global_scope_name, ptr, count);
      return ptr;
    }

    [[nodiscard]] void* managed_new_array(std::size_t count, std::align_val_t al)
    {
      auto* ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(global_scope_name, ptr, count);
      return ptr;
    }


    void managed_delete(void* ptr) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete(void* ptr, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete(void* ptr, [[maybe_unused]] std::size_t sz) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete(void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(void* ptr) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(void* ptr, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(void* ptr, [[maybe_unused]] std::size_t sz) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(global_scope_name, ptr);
      std::free(ptr);
    }


    [[nodiscard]] void* managed_new(std::string_view class_name, std::size_t sz)
    {
      auto* ptr = std::malloc(sz);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(class_name, ptr, sz);
      return ptr;
    }

    [[nodiscard]] void* managed_new(std::string_view class_name, std::size_t count, std::align_val_t al)
    {
      auto* ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(class_name, ptr, count);
      return ptr;
    }

    [[nodiscard]] void* managed_new_array(std::string_view class_name, std::size_t count)
    {
      auto* ptr = std::malloc(count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(class_name, ptr, count);
      return ptr;
    }

    [[nodiscard]] void* managed_new_array(std::string_view class_name, std::size_t count, std::align_val_t al)
    {
      auto* ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
      if (!ptr)
        throw std::bad_alloc{};

      log_memory_allocation(class_name, ptr, count);
      return ptr;
    }


    void managed_delete(std::string_view class_name, void* ptr) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(std::string_view class_name, void* ptr) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

    void managed_delete_array(std::string_view class_name, void* ptr, [[maybe_unused]] std::size_t sz, [[maybe_unused]] std::align_val_t al) noexcept
    {
      log_memory_deallocation(class_name, ptr);
      std::free(ptr);
    }

  } // namespace detail


  memory_informations get_global_memory_information_snapshot()
  {
    return detail::get_global_memory_information()[detail::global_scope_name].to_user_info(detail::global_scope_name);
  }

  memory_informations get_memory_information_snapshot(std::string_view scope) { return detail::get_global_memory_information()[scope].to_user_info(scope); }

} // namespace MicrobenchMemory
