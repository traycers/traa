#include <traaxx_simd/bfs_reorder.hpp>
#include <algorithm>
#include <traaxx_simd/depth.hpp>
#include <traaxx_simd/rank.hpp>

namespace traaxx_simd
{
    // Per-level std::stable_sort — control-flow-heavy, level sizes are
    // data-dependent, not a batch::gather/select shape; matches cxx/traaxx.
    template<typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> bfs_reorder_tables(
        const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return ReorderTables<IndexT>{};
        }
        auto depth_result = depth(parent, static_cast<IndexT>(n));
        if (!depth_result.has_value())
        {
            return std::unexpected(depth_result.error());
        }
        auto rank_result = rank(sibling, static_cast<IndexT>(n));
        if (!rank_result.has_value())
        {
            return std::unexpected(rank_result.error());
        }
        auto const &node_depth = *depth_result;
        auto const &node_rank = *rank_result;
        auto const d_max = *std::max_element(node_depth.cbegin(), node_depth.cend());
        auto new2old = std::vector<IndexT>(n);
        auto old2new = std::vector<IndexT>(n, IndexT{ 0 });
        auto position = IndexT{ 0 };
        for (IndexT level = 0; level <= d_max; ++level)
        {
            auto level_nodes = std::vector<IndexT>{};
            for (IndexT i = 0; i < static_cast<IndexT>(n); ++i)
            {
                if (node_depth[i] == level)
                {
                    level_nodes.push_back(i);
                }
            }
            std::stable_sort(level_nodes.begin(), level_nodes.end(),
                [&](IndexT a, IndexT b)
                {
                    auto const key_a = level == IndexT{ 0 } ? IndexT{ 0 } : old2new[parent[a]];
                    auto const key_b = level == IndexT{ 0 } ? IndexT{ 0 } : old2new[parent[b]];
                    if (key_a != key_b)
                    {
                        return key_a < key_b;
                    }
                    return node_rank[a] < node_rank[b];
                });
            for (auto const old_id : level_nodes)
            {
                new2old[position] = old_id;
                old2new[old_id] = position;
                ++position;
            }
        }
        return ReorderTables<IndexT>{ std::move(new2old), std::move(old2new) };
    }

    template<typename T, typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> Tree<T, IndexT>::bfs_reorder()
    {
        auto tables = bfs_reorder_tables(parent_, sibling_);
        if (!tables.has_value())
        {
            return std::unexpected(tables.error());
        }
        auto new_parent = remap_index_vector(parent_, tables->new_to_old, tables->old_to_new);
        auto new_sibling = remap_index_vector(sibling_, tables->new_to_old, tables->old_to_new);
        auto new_data = gather(data_, tables->new_to_old);
        parent_.swap(new_parent);
        sibling_.swap(new_sibling);
        data_.swap(new_data);
        return std::move(tables.value());
    }

    template std::expected<ReorderTables<std::uint32_t>, ConvergenceError<std::uint32_t>> bfs_reorder_tables<
        std::uint32_t>(const std::vector<std::uint32_t> &, const std::vector<std::uint32_t> &);

    template std::expected<ReorderTables<std::uint32_t>, ConvergenceError<std::uint32_t>>
    Tree<std::uint32_t, std::uint32_t>::bfs_reorder();
}
