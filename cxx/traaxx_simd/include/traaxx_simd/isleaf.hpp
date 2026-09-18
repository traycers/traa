#pragma once
#include <cstdint>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<bool> isleaf(const std::vector<IndexT> &parent);
}
