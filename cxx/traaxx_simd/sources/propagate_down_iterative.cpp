#include <traaxx_simd/propagate_down_iterative.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx_simd
{
    // Not vectorized (deliberate): the read side is `current.get(i) |
    // current.get(parent[i])` over BitMask's packed-uint64-word bits, not a
    // dense IndexT array — a literal batch::gather here would need to gather
    // whole 64-bit words at `parent[i]/64` (uint32 index into uint64 data —
    // a width mismatch xsimd's hardware gather does not take directly) and
    // then extract one bit per lane, or materialize a dense scratch buffer
    // every iteration, which defeats the bit-packing ADR-0001 chose BitMask
    // for. The write side (`next.atomic_or(i)`, `i` sequential) would be
    // easy to batch on its own, but pairing it with a scalar-extracted read
    // side buys nothing. Kept parallel-scalar, matching cxx/traaxx exactly.
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
