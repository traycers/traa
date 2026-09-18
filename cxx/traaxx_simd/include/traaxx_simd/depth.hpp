#pragma once
#include <cstdint>
#include <expected>
#include <traaxx_simd/propagate.hpp>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::expected<std::vector<IndexT>, ConvergenceError<IndexT>> depth(
        const std::vector<IndexT> &parent, IndexT max_iterations);
}
