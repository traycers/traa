#include <traaxx_simd/propagate_down_pointer_doubling.hpp>
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
    BitMask propagate_down_pointer_doubling(const std::vector<IndexT> &parent, BitMask seed, IndexT d_max)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return seed;
        }
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        auto const level_count = level_count_for(d_max);
        auto current = std::move(seed);
        auto anc = parent;
        for (IndexT k = 0; k < level_count; ++k)
        {
            // BitMask combine step stays scalar/parallel — same reasoning as
            // propagate_down_iterative (bit-packed read, not a dense gather).
            auto next = BitMask(n);
            std::for_each(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
                [&](IndexT i)
                {
                    if (current.get(i) || current.get(anc[i]))
                    {
                        next.atomic_or(i);
                    }
                });
            current = std::move(next);
            if (k + IndexT{ 1 } < level_count)
            {
                // Ancestor-array squaring `anc[anc[i]]`: the genuine
                // pointer-doubling gather this track was chosen for.
                auto next_anc = std::vector<IndexT>(n);
                auto i = std::size_t{ 0 };
                if constexpr (std::is_same_v<IndexT, std::uint32_t>)
                {
                    auto const width = batch_t::size;
                    for (; i + width <= n; i += width)
                    {
                        auto const self = batch_t::load_unaligned(&anc[i]);
                        auto const doubled = batch_t::gather(anc.data(), self);
                        doubled.store_unaligned(&next_anc[i]);
                    }
                }
                for (; i < n; ++i)
                {
                    next_anc[i] = anc[anc[i]];
                }
                anc = std::move(next_anc);
            }
        }
        return current;
    }

    template BitMask propagate_down_pointer_doubling<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask, std::uint32_t);
}
