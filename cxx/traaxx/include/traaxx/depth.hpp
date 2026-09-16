#pragma once
#include <cstdint>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    struct DepthResult
    {
        std::vector<IndexT> depth_vector;
        IndexT iterations = 0;
    };

    template<typename IndexT>
    DepthResult<IndexT> depth(const std::vector<IndexT> &parent, IndexT max_iterations);
}
