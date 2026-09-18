#pragma once
#include <cstdint>
#include <traaxx_simd/ancestor_table.hpp>
#include <traaxx_simd/traaxx_simd.hpp>
#include <vector>

namespace traaxx_simd
{
    // Depends only on IndexT (parent/node_depth/AncestorTable/query vectors,
    // no T) — like dfs_reorder_tables/bfs_reorder_tables, defined in
    // lca_batch.cpp and explicit-instantiated, so it can vectorize with xsimd
    // (kept out of this header) without dragging xsimd types into a template
    // that would otherwise need to be header-only over T.
    template<typename IndexT>
    std::vector<IndexT> lca_batch_core(const std::vector<IndexT> &parent, const std::vector<IndexT> &node_depth,
        const AncestorTable<IndexT> &table, std::vector<IndexT> const &a, std::vector<IndexT> const &b);

    template<typename T, typename IndexT>
    std::vector<IndexT> lca_batch(const Tree<T, IndexT> &tree, const AncestorTable<IndexT> &table,
        const std::vector<IndexT> &node_depth, std::vector<IndexT> const &a, std::vector<IndexT> const &b)
    {
        return lca_batch_core(tree.parent(), node_depth, table, a, b);
    }
}
