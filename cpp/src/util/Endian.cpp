#include "Endian.h"
#include <cstdint>
#include <bit>

namespace Crawler {

bool IsBigEndian(){    
    if constexpr (std::endian::native == std::endian::big)
        return true;
    return false;
}

}