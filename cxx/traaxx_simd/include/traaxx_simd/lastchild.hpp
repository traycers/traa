#pragma once
#include <cstdint>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<IndexT> lastchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling);
}
