#include <traaxx/propagate_up_pointer_doubling.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    BitMask<IndexT> propagate_up_pointer_doubling(
        const std::vector<IndexT> &parent, const AncestorTable<IndexT> &table, BitMask<IndexT> seed)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return seed;
        }
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        auto current = std::move(seed);
        for (IndexT k = 0; k < table.level_count(); ++k)
        {
            auto const &level_k = table.level(k);
            auto next = current;
            std::for_each(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
                [&](IndexT i)
                {
                    if (current.get(i))
                    {
                        next.atomic_or(level_k[i]);
                    }
                });
            current = std::move(next);
        }
        return current;
    }

    template BitMask<std::uint32_t> propagate_up_pointer_doubling<std::uint32_t>(
        const std::vector<std::uint32_t> &, const AncestorTable<std::uint32_t> &, BitMask<std::uint32_t>);
}
