#pragma once
#include <cstdint>
#include <expected>
#include <traaxx/bitmask.hpp>
#include <traaxx/propagate.hpp>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::expected<BitMask, ConvergenceError<IndexT>> propagate_down_iterative(
        const std::vector<IndexT> &parent, BitMask seed, IndexT max_iterations);
}
