#include <traaxx/propagate_up_iterative.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    std::expected<BitMask<IndexT>, ConvergenceError<IndexT>> propagate_up_iterative(
        const std::vector<IndexT> &parent, BitMask<IndexT> seed, IndexT max_iterations)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return std::move(seed);
        }
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        auto const step = [&](BitMask<IndexT> const &current, BitMask<IndexT> &next)
        {
            next = current;
            std::for_each(
                std::execution::par, indices.begin(), indices.end(),
                [&](IndexT i)
                {
                    if (current.get(i))
                    {
                        next.atomic_or(parent[i]);
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

    template std::expected<BitMask<std::uint32_t>, ConvergenceError<std::uint32_t>>
    propagate_up_iterative<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask<std::uint32_t>, std::uint32_t);
}
