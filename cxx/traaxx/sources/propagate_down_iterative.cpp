#include <traaxx/propagate_down_iterative.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    std::expected<BitMask, ConvergenceError<IndexT>> propagate_down_iterative(
        const std::vector<IndexT> &parent, BitMask seed, IndexT max_iterations)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return seed;
        }
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        auto const step = [&](BitMask const &current, BitMask &next)
        {
            next = BitMask(n);
            std::for_each(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
                [&](IndexT i)
                {
                    if (current.get(i) || current.get(parent[i]))
                    {
                        next.atomic_or(i);
                    }
                });
        };
        auto result = propagate(std::move(seed), step, max_iterations);
        if (!result.has_value())
        {
            return std::unexpected(result.error());
        }
        return std::move(result->state);
    }

    template std::expected<BitMask, ConvergenceError<std::uint32_t>> propagate_down_iterative<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask, std::uint32_t);
}
