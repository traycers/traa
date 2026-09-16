#pragma once
#include <cstdint>
#include <traaxx/bitmask.hpp>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    BitMask<IndexT> propagate_down_single_pass(const std::vector<IndexT> &parent, BitMask<IndexT> seed);
}
