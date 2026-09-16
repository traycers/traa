#pragma once
#include <cstdint>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> lastchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling);
}
