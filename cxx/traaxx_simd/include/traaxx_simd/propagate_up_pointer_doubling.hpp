#pragma once
#include <cstdint>
#include <traaxx_simd/bitmask.hpp>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    BitMask propagate_up_pointer_doubling(const std::vector<IndexT> &parent, BitMask seed, IndexT d_max);
}
