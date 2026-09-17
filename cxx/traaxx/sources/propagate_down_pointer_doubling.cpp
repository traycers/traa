#include <traaxx/propagate_down_pointer_doubling.hpp>
#include <traaxx/level_count_for.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
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
                auto next_anc = std::vector<IndexT>(n);
                std::transform(
                    std::execution::par,   //
                    indices.cbegin(),      //
                    indices.cend(),        //
                    next_anc.begin(),      //
                    [&](IndexT i) { return anc[anc[i]]; });
                anc = std::move(next_anc);
            }
        }
        return current;
    }

    template BitMask propagate_down_pointer_doubling<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask, std::uint32_t);
}
