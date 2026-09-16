#pragma once
#include <cstdint>
#include <vector>

namespace traaxx
{
    template<typename T, typename IndexT = std::uint32_t>
    struct Tree
    {
        std::vector<IndexT> parent;
        std::vector<IndexT> sibling;
        std::vector<T> data;
    };
}
