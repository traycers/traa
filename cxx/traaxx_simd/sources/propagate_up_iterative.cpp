#include <traaxx_simd/propagate_up_iterative.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx_simd
{
    // Not vectorized (deliberate, and the strongest of the "stays scalar"
    // calls): `next.atomic_or(parent[i])` is a scatter to a data-dependent
    // destination index, not a gather — there is no portable SIMD scatter
    // available here (AVX2 has no scatter instruction; that needs AVX-512),
    // and ADR-0001 already chose atomic_or over this exact write pattern for
    // race-free concurrent bit-sets. This mirrors cxx/traaxx exactly.
    template<typename IndexT>
    std::expected<BitMask, ConvergenceError<IndexT>> propagate_up_iterative(
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
            next = current;
            std::for_each(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
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

    template std::expected<BitMask, ConvergenceError<std::uint32_t>> propagate_up_iterative<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask, std::uint32_t);
}
