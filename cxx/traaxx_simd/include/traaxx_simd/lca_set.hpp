#pragma once
#include <cstdint>
#include <traaxx_simd/traaxx_simd.hpp>
#include <vector>

namespace traaxx_simd
{
    // Precondition: `s` is non-empty and every element is a valid node id.
    // Every ancestor of the true LCA (inclusive, up to the root) has cnt[i]==|s|;
    // that set is a single root-to-LCA chain, so the LCA is the unique qualifying
    // node with no qualifying child — this avoids a separate depth() call/failure
    // mode, matching the ticket's bare-IndexT (no std::expected) signature.
    template<typename T, typename IndexT>
    IndexT lca_set(const Tree<T, IndexT> &tree, std::vector<IndexT> const &s)
    {
        auto const &parent = tree.parent();
        auto const n = parent.size();
        auto cnt = std::vector<IndexT>(n, IndexT{ 0 });
        for (auto const x : s)
        {
            cnt[x] = static_cast<IndexT>(cnt[x] + IndexT{ 1 });
        }
        for (auto i = n; i-- > 1;)
        {
            cnt[parent[i]] = static_cast<IndexT>(cnt[parent[i]] + cnt[i]);
        }
        auto const target = static_cast<IndexT>(s.size());
        auto has_qualifying_child = std::vector<bool>(n, false);
        for (IndexT i = 0; i < static_cast<IndexT>(n); ++i)
        {
            if (cnt[i] == target && parent[i] != i)
            {
                has_qualifying_child[parent[i]] = true;
            }
        }
        for (IndexT i = 0; i < static_cast<IndexT>(n); ++i)
        {
            if (cnt[i] == target && !has_qualifying_child[i])
            {
                return i;
            }
        }
        return parent[0];
    }
}
