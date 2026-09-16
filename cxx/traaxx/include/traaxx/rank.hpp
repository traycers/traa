#pragma once
#include <cstdint>
#include <expected>
#include <traaxx/propagate.hpp>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::expected<std::vector<IndexT>, ConvergenceError<IndexT>> rank(
        const std::vector<IndexT> &sibling, IndexT max_iterations);
}
