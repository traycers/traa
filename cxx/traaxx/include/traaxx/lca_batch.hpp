#pragma once
#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <traaxx/ancestor_table.hpp>
#include <traaxx/lca_binary_lifting.hpp>
#include <traaxx/traaxx.hpp>
#include <vector>

namespace traaxx
{
    template<typename T, typename IndexT>
    std::vector<IndexT> lca_batch(const Tree<T, IndexT> &tree, const AncestorTable<IndexT> &table,
        const std::vector<IndexT> &node_depth, std::vector<IndexT> const &a, std::vector<IndexT> const &b)
    {
        auto const &parent = tree.parent();
        auto result = std::vector<IndexT>(a.size());
        auto indices = std::vector<IndexT>(a.size());
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::transform(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            result.begin(),        //
            [&](IndexT i) { return detail::lca_climb(parent, node_depth, table, a[i], b[i]); });
        return result;
    }
}
