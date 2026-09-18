#pragma once
#include <cstdint>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<IndexT> nsibling(const std::vector<IndexT> &sibling);
}
