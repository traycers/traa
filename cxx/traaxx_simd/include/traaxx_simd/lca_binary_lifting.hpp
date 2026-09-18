#pragma once
#include <cstdint>
#include <traaxx_simd/ancestor_table.hpp>
#include <traaxx_simd/traaxx_simd.hpp>
#include <vector>

namespace traaxx_simd
{
    namespace detail
    {
        template<typename IndexT>
        IndexT lca_climb(const std::vector<IndexT> &parent, const std::vector<IndexT> &node_depth,
            const AncestorTable<IndexT> &table, IndexT a, IndexT b)
        {
            if (node_depth[a] < node_depth[b])
            {
                auto const tmp = a;
                a = b;
                b = tmp;
            }
            auto diff = static_cast<IndexT>(node_depth[a] - node_depth[b]);
            for (IndexT k = 0; k < table.level_count(); ++k)
            {
                if ((diff >> k) & IndexT{ 1 })
                {
                    a = table.level(k)[a];
                }
            }
            if (a == b)
            {
                return a;
            }
            for (IndexT k = table.level_count(); k-- > 0;)
            {
                if (table.level(k)[a] != table.level(k)[b])
                {
                    a = table.level(k)[a];
                    b = table.level(k)[b];
                }
            }
            return parent[a];
        }
    }

    template<typename T, typename IndexT>
    IndexT lca_binary_lifting(const Tree<T, IndexT> &tree, const AncestorTable<IndexT> &table,
        const std::vector<IndexT> &node_depth, IndexT a, IndexT b)
    {
        return detail::lca_climb(tree.parent(), node_depth, table, a, b);
    }
}
