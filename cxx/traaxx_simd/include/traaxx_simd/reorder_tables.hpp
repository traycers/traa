#pragma once
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    struct ReorderTables
    {
        std::vector<IndexT> new_to_old;
        std::vector<IndexT> old_to_new;
    };
}
