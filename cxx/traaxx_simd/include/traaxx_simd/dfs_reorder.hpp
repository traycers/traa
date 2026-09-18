#pragma once
#include <cstdint>
#include <expected>
#include <traaxx_simd/permutation.hpp>
#include <traaxx_simd/propagate.hpp>
#include <traaxx_simd/reorder_tables.hpp>
#include <traaxx_simd/traaxx_simd.hpp>
#include <utility>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> dfs_reorder_tables(
        const std::vector<IndexT> &parent);

    template<typename T, typename IndexT>
    struct DfsReorderResult
    {
        Tree<T, IndexT> tree;
        std::vector<IndexT> new_to_old;
        std::vector<IndexT> old_to_new;
    };

    template<typename T, typename IndexT>
    std::expected<DfsReorderResult<T, IndexT>, ConvergenceError<IndexT>> dfs_reorder(const Tree<T, IndexT> &tree)
    {
        auto tables = dfs_reorder_tables(tree.parent());
        if (!tables.has_value())
        {
            return std::unexpected(tables.error());
        }
        auto reordered = Tree<T, IndexT>{ remap_index_vector(tree.parent(), tables->new_to_old, tables->old_to_new),
            remap_index_vector(tree.sibling(), tables->new_to_old, tables->old_to_new),
            gather(tree.data(), tables->new_to_old) };
        return DfsReorderResult<T, IndexT>{
            std::move(reordered), std::move(tables->new_to_old), std::move(tables->old_to_new) };
    }
}
