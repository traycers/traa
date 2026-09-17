#include <traaxx/traaxx.hpp>
#include <traaxx/firstchild.hpp>
#include <traaxx/isleaf.hpp>
#include <traaxx/lastchild.hpp>
#include <traaxx/nsibling.hpp>
#include <traaxx/permutation.hpp>
#include <traaxx/propagate_down_iterative.hpp>
#include <algorithm>
#include <cassert>
#include <execution>
#include <utility>

namespace traaxx
{
    template<typename T, typename IndexT>
    IndexT Tree<T, IndexT>::append(IndexT p, T value)
    {
        assert(p < static_cast<IndexT>(parent_.size()));
        auto const x = static_cast<IndexT>(parent_.size());
        auto const p_last_child = lastchild(parent_, sibling_)[p];
        parent_.push_back(p);
        sibling_.push_back(p_last_child == p ? x : p_last_child);
        data_.push_back(std::move(value));
        return x;
    }

    template<typename T, typename IndexT>
    void Tree<T, IndexT>::deleteLeaf(IndexT x)
    {
        assert(isleaf(parent_)[x]);
        auto const r = nsibling(sibling_)[x];
        if (r != x)
        {
            auto const was_first = sibling_[x] == x;
            sibling_[r] = sibling_[x];
            if (was_first)
            {
                sibling_[r] = r;
            }
        }
    }

    template<typename T, typename IndexT>
    void Tree<T, IndexT>::splice(IndexT x)
    {
        assert(parent_[x] != x);
        auto const l = sibling_[x];
        auto const r = nsibling(sibling_)[x];
        auto const f = firstchild(parent_, sibling_)[x];
        auto const lc = lastchild(parent_, sibling_)[x];
        auto const p = parent_[x];
        std::transform(
            std::execution::par,   //
            parent_.cbegin(),      //
            parent_.cend(),        //
            parent_.begin(),       //
            [x, p](IndexT v) { return v == x ? p : v; });
        if (f != x)
        {
            sibling_[f] = l != x ? l : f;
            if (r != x)
            {
                sibling_[r] = lc;
            }
        }
        else if (r != x)
        {
            sibling_[r] = l;
            if (l == x)
            {
                sibling_[r] = r;
            }
        }
    }

    template<typename T, typename IndexT>
    std::expected<void, ConvergenceError<IndexT>> Tree<T, IndexT>::deleteSubtree(IndexT x)
    {
        auto const n = static_cast<IndexT>(parent_.size());
        auto seed = BitMask<IndexT>(n);
        seed.set(x);
        auto mask_result = propagate_down_iterative(parent_, std::move(seed), n);
        if (!mask_result.has_value())
        {
            return std::unexpected(mask_result.error());
        }
        auto const &mask = *mask_result;
        auto const r = nsibling(sibling_)[x];
        if (r != x)
        {
            auto const was_first = sibling_[x] == x;
            sibling_[r] = sibling_[x];
            if (was_first)
            {
                sibling_[r] = r;
            }
        }
        auto new2old = std::vector<IndexT>{};
        auto old2new = std::vector<IndexT>(n, IndexT{ 0 });
        for (IndexT i = 0; i < n; ++i)
        {
            if (!mask.get(i))
            {
                old2new[i] = static_cast<IndexT>(new2old.size());
                new2old.push_back(i);
            }
        }
        auto new_parent = remap_index_vector(parent_, new2old, old2new);
        auto new_sibling = remap_index_vector(sibling_, new2old, old2new);
        auto new_data = gather(data_, new2old);
        parent_.swap(new_parent);
        sibling_.swap(new_sibling);
        data_.swap(new_data);
        return {};
    }

    template<typename T, typename IndexT>
    std::expected<void, MoveError<IndexT>> Tree<T, IndexT>::move(IndexT x, IndexT q)
    {
        auto const n = static_cast<IndexT>(parent_.size());
        auto seed = BitMask<IndexT>(n);
        seed.set(x);
        auto mask_result = propagate_down_iterative(parent_, std::move(seed), n);
        if (!mask_result.has_value())
        {
            return std::unexpected(
                MoveError<IndexT>{ MoveError<IndexT>::Kind::tree_corrupted, mask_result.error().iterations });
        }
        if (mask_result->get(q))
        {
            return std::unexpected(MoveError<IndexT>{ MoveError<IndexT>::Kind::would_create_cycle, IndexT{ 0 } });
        }
        auto const q_last_child = lastchild(parent_, sibling_)[q];
        auto const r = nsibling(sibling_)[x];
        if (r != x)
        {
            auto const was_first = sibling_[x] == x;
            sibling_[r] = sibling_[x];
            if (was_first)
            {
                sibling_[r] = r;
            }
        }
        parent_[x] = q;
        sibling_[x] = q_last_child == q ? x : q_last_child;
        return {};
    }

    template std::uint32_t Tree<std::uint32_t, std::uint32_t>::append(std::uint32_t, std::uint32_t);
    template void Tree<std::uint32_t, std::uint32_t>::deleteLeaf(std::uint32_t);
    template void Tree<std::uint32_t, std::uint32_t>::splice(std::uint32_t);
    template std::expected<void, ConvergenceError<std::uint32_t>> Tree<std::uint32_t, std::uint32_t>::deleteSubtree(
        std::uint32_t);
    template std::expected<void, MoveError<std::uint32_t>> Tree<std::uint32_t, std::uint32_t>::move(
        std::uint32_t, std::uint32_t);
}
