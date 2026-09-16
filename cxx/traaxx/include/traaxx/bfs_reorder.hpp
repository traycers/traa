#pragma once
#include <cstdint>
#include <expected>
#include <traaxx/permutation.hpp>
#include <traaxx/propagate.hpp>
#include <traaxx/reorder_tables.hpp>
#include <traaxx/traaxx.hpp>
#include <utility>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> bfs_reorder_tables(
        const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling);

    template<typename T, typename IndexT>
    struct BfsReorderResult
    {
        Tree<T, IndexT> tree;
        std::vector<IndexT> new_to_old;
        std::vector<IndexT> old_to_new;
    };

    template<typename T, typename IndexT>
    std::expected<BfsReorderResult<T, IndexT>, ConvergenceError<IndexT>> bfs_reorder(const Tree<T, IndexT> &tree)
    {
        auto tables = bfs_reorder_tables(tree.parent(), tree.sibling());
        if (!tables.has_value())
        {
            return std::unexpected(tables.error());
        }
        auto reordered = Tree<T, IndexT>{ remap_index_vector(tree.parent(), tables->new_to_old, tables->old_to_new),
            remap_index_vector(tree.sibling(), tables->new_to_old, tables->old_to_new),
            gather(tree.data(), tables->new_to_old) };
        return BfsReorderResult<T, IndexT>{
            std::move(reordered), std::move(tables->new_to_old), std::move(tables->old_to_new) };
    }
}
