#pragma once
#include <cstdint>
#include <expected>
#include <traaxx_simd/bitmask.hpp>
#include <traaxx_simd/propagate.hpp>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::expected<BitMask, ConvergenceError<IndexT>> propagate_down_iterative(
        const std::vector<IndexT> &parent, BitMask seed, IndexT max_iterations);
}
