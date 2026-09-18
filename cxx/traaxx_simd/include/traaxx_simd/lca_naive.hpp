#pragma once
#include <cstdint>
#include <expected>
#include <traaxx_simd/depth.hpp>
#include <traaxx_simd/propagate.hpp>
#include <traaxx_simd/traaxx_simd.hpp>

namespace traaxx_simd
{
    template<typename T, typename IndexT>
    std::expected<IndexT, ConvergenceError<IndexT>> lca_naive(const Tree<T, IndexT> &tree, IndexT a, IndexT b)
    {
        auto depth_result = depth(tree.parent(), tree.nodes_count());
        if (!depth_result.has_value())
        {
            return std::unexpected(depth_result.error());
        }
        auto const &node_depth = *depth_result;
        auto const &parent = tree.parent();
        while (node_depth[a] > node_depth[b])
        {
            a = parent[a];
        }
        while (node_depth[b] > node_depth[a])
        {
            b = parent[b];
        }
        while (a != b)
        {
            a = parent[a];
            b = parent[b];
        }
        return a;
    }
}
