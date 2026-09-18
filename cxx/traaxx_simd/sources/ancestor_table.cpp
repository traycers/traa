#include <traaxx_simd/ancestor_table.hpp>
#include <traaxx_simd/level_count_for.hpp>
#include <algorithm>
#include <execution>
#include <numeric>
#include <type_traits>
#include <xsimd/xsimd.hpp>

namespace traaxx_simd
{
    namespace
    {
        using batch_t = xsimd::batch<std::uint32_t>;
    }

    template<typename IndexT>
    AncestorTable<IndexT>::AncestorTable(const std::vector<IndexT> &parent, IndexT d_max)
    {
        auto const n = parent.size();
        auto const level_count = level_count_for(d_max);
        levels_.reserve(level_count);
        levels_.push_back(parent);
        for (IndexT k = 1; k < level_count; ++k)
        {
            auto const &previous = levels_.back();
            auto next = std::vector<IndexT>(n);
            // pointer-doubling squaring step: next[i] = previous[previous[i]] —
            // `previous[i]` is a contiguous load (i is sequential), then a genuine
            // data-dependent gather of `previous` at those values.
            auto i = std::size_t{ 0 };
            if constexpr (std::is_same_v<IndexT, std::uint32_t>)
            {
                auto const width = batch_t::size;
                for (; i + width <= n; i += width)
                {
                    auto const self = batch_t::load_unaligned(&previous[i]);
                    auto const doubled = batch_t::gather(previous.data(), self);
                    doubled.store_unaligned(&next[i]);
                }
            }
            for (; i < n; ++i)
            {
                next[i] = previous[previous[i]];
            }
            levels_.push_back(std::move(next));
        }
    }

    template<typename IndexT>
    const std::vector<IndexT> &AncestorTable<IndexT>::level(IndexT k) const
    {
        return levels_[k];
    }

    template<typename IndexT>
    IndexT AncestorTable<IndexT>::level_count() const
    {
        return static_cast<IndexT>(levels_.size());
    }

    template class AncestorTable<std::uint32_t>;
}
