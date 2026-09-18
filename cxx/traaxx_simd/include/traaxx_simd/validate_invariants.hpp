#pragma once
#include <algorithm>
#include <cstdint>
#include <traaxx_simd/bitmask.hpp>
#include <traaxx_simd/propagate_down_iterative.hpp>
#include <traaxx_simd/traaxx_simd.hpp>
#include <vector>

namespace traaxx_simd
{
    enum class InvariantCheck
    {
        bounds,
        parent_order,
        sibling_order,
        single_root,
        sibling_same_parent,
        sibling_injective,
        reachable_from_root,
        propagation_converges
    };

    template<typename IndexT>
    struct Violation
    {
        InvariantCheck check;
        std::vector<IndexT> nodes;
    };

    template<typename IndexT>
    struct ValidationResult
    {
        bool valid;
        std::vector<Violation<IndexT>> violations;
    };

    template<typename T, typename IndexT>
    ValidationResult<IndexT> validate_invariants(const Tree<T, IndexT> &tree)
    {
        auto const &parent = tree.parent();
        auto const &sibling = tree.sibling();
        auto const n = tree.nodes_count();
        auto violations = std::vector<Violation<IndexT>>{};
        auto bounds_nodes = std::vector<IndexT>{};
        for (IndexT i = 0; i < n; ++i)
        {
            if (parent[i] >= n || sibling[i] >= n)
            {
                bounds_nodes.push_back(i);
            }
        }
        auto const bounds_clean = bounds_nodes.empty();
        if (!bounds_clean)
        {
            violations.push_back(Violation<IndexT>{ InvariantCheck::bounds, std::move(bounds_nodes) });
        }
        auto parent_order_nodes = std::vector<IndexT>{};
        auto sibling_order_nodes = std::vector<IndexT>{};
        auto root_nodes = std::vector<IndexT>{};
        for (IndexT i = 0; i < n; ++i)
        {
            if (parent[i] > i)
            {
                parent_order_nodes.push_back(i);
            }
            if (sibling[i] > i)
            {
                sibling_order_nodes.push_back(i);
            }
            if (parent[i] == i)
            {
                root_nodes.push_back(i);
            }
        }
        if (!parent_order_nodes.empty())
        {
            violations.push_back(Violation<IndexT>{ InvariantCheck::parent_order, std::move(parent_order_nodes) });
        }
        if (!sibling_order_nodes.empty())
        {
            violations.push_back(Violation<IndexT>{ InvariantCheck::sibling_order, std::move(sibling_order_nodes) });
        }
        if (root_nodes.size() != 1)
        {
            violations.push_back(Violation<IndexT>{ InvariantCheck::single_root, root_nodes });
        }
        auto sibling_values = std::vector<IndexT>{};
        for (IndexT i = 0; i < n; ++i)
        {
            if (sibling[i] != i)
            {
                sibling_values.push_back(sibling[i]);
            }
        }
        std::sort(sibling_values.begin(), sibling_values.end());
        auto injective_nodes = std::vector<IndexT>{};
        for (std::size_t k = 1; k < sibling_values.size(); ++k)
        {
            if (sibling_values[k] == sibling_values[k - 1]
                && (injective_nodes.empty() || injective_nodes.back() != sibling_values[k]))
            {
                injective_nodes.push_back(sibling_values[k]);
            }
        }
        if (!injective_nodes.empty())
        {
            violations.push_back(Violation<IndexT>{ InvariantCheck::sibling_injective, std::move(injective_nodes) });
        }
        if (bounds_clean)
        {
            auto same_parent_nodes = std::vector<IndexT>{};
            for (IndexT i = 0; i < n; ++i)
            {
                if (sibling[i] != i && parent[sibling[i]] != parent[i])
                {
                    same_parent_nodes.push_back(i);
                }
            }
            if (!same_parent_nodes.empty())
            {
                violations.push_back(
                    Violation<IndexT>{ InvariantCheck::sibling_same_parent, std::move(same_parent_nodes) });
            }
            auto seed = BitMask(n);
            for (auto const root : root_nodes)
            {
                seed.set(root);
            }
            auto const propagated = propagate_down_iterative(parent, seed, n);
            if (!propagated.has_value())
            {
                violations.push_back(Violation<IndexT>{ InvariantCheck::propagation_converges, {} });
            }
            else
            {
                auto unreached_nodes = std::vector<IndexT>{};
                for (IndexT i = 0; i < n; ++i)
                {
                    if (!propagated->get(i))
                    {
                        unreached_nodes.push_back(i);
                    }
                }
                if (!unreached_nodes.empty())
                {
                    violations.push_back(
                        Violation<IndexT>{ InvariantCheck::reachable_from_root, std::move(unreached_nodes) });
                }
            }
        }
        return ValidationResult<IndexT>{ violations.empty(), std::move(violations) };
    }
}
