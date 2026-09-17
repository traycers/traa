#pragma once
#include <cstdint>
#include <traaxx/bitmask.hpp>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    BitMask propagate_down_pointer_doubling(const std::vector<IndexT> &parent, BitMask seed, IndexT d_max);
}
