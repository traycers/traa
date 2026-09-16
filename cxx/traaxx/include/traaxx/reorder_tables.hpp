#pragma once
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    struct ReorderTables
    {
        std::vector<IndexT> new_to_old;
        std::vector<IndexT> old_to_new;
    };
}
