#pragma once

#include "MicrobenchMemory/MicrobenchMemory.hpp"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const MicrobenchMemory::memory_informations& minfo)
{
  os << "Memory information snapshot for scope: <" << minfo.scope << ">:\n"
     << "Total allocations: " << minfo.allocation_count << '\n'
     << "Total deallocations: " << minfo.deallocation_count << '\n'
     << "Total memory allocated: " << minfo.total_memory_allocated << '\n'
     << "Total memory deallocated: " << minfo.total_memory_deallocated << '\n'
     << "---\n";
  return os;
}
