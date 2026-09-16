#include <traaxx/dfs_reorder.hpp>
#include <traaxx/depth.hpp>
#include <traaxx/size.hpp>

namespace traaxx
{
    template<typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> dfs_reorder_tables(
        const std::vector<IndexT> &parent)
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
        auto const node_size = size(parent);
        auto pos = std::vector<IndexT>(n, IndexT{ 0 });
        auto running = std::vector<IndexT>(n, IndexT{ 0 });
        for (IndexT i = 1; i < static_cast<IndexT>(n); ++i)
        {
            auto const p = parent[i];
            pos[i] = static_cast<IndexT>(pos[p] + IndexT{ 1 } + running[p]);
            running[p] = static_cast<IndexT>(running[p] + node_size[i]);
        }
        auto old2new = pos;
        auto new2old = inverse(old2new);
        return ReorderTables<IndexT>{ std::move(new2old), std::move(old2new) };
    }

    template<typename T, typename IndexT>
    std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> Tree<T, IndexT>::dfs_reorder()
    {
        auto tables = dfs_reorder_tables(parent_);
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

    template std::expected<ReorderTables<std::uint32_t>, ConvergenceError<std::uint32_t>> dfs_reorder_tables<
        std::uint32_t>(const std::vector<std::uint32_t> &);

    template std::expected<ReorderTables<std::uint32_t>, ConvergenceError<std::uint32_t>>
    Tree<std::uint32_t, std::uint32_t>::dfs_reorder();
}
