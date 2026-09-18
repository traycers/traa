#pragma once
#include <cstdint>
#include <traaxx_simd/bitmask.hpp>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    BitMask propagate_down_single_pass(const std::vector<IndexT> &parent, BitMask seed);
}
