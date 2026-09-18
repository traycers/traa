#include <traaxx_simd/rank.hpp>
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
    std::expected<std::vector<IndexT>, ConvergenceError<IndexT>> rank(
        const std::vector<IndexT> &sibling, IndexT max_iterations)
    {
        auto const n = sibling.size();
        if (n == 0)
        {
            return std::vector<IndexT>{};
        }
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        auto const step = [&](std::vector<IndexT> const &current, std::vector<IndexT> &next)
        {
            auto i = std::size_t{ 0 };
            if constexpr (std::is_same_v<IndexT, std::uint32_t>)
            {
                auto const width = batch_t::size;
                auto const zero = batch_t(std::uint32_t{ 0 });
                auto const one = batch_t(std::uint32_t{ 1 });
                for (; i + width <= n; i += width)
                {
                    auto const idx = batch_t::load_unaligned(&indices[i]);
                    auto const s = batch_t::load_unaligned(&sibling[i]);
                    auto const is_last = s == idx;
                    auto const gathered = batch_t::gather(current.data(), s);
                    auto const value = xsimd::select(is_last, zero, gathered + one);
                    value.store_unaligned(&next[i]);
                }
            }
            for (; i < n; ++i)
            {
                next[i] = sibling[i] == static_cast<IndexT>(i) ? IndexT{ 0 }
                                                                : static_cast<IndexT>(current[sibling[i]] + IndexT{ 1 });
            }
        };
        auto result = propagate(std::vector<IndexT>(n, IndexT{ 0 }), step, max_iterations);
        if (!result.has_value())
        {
            return std::unexpected(result.error());
        }
        return std::move(result->state);
    }

    template std::expected<std::vector<std::uint32_t>, ConvergenceError<std::uint32_t>> rank<std::uint32_t>(
        const std::vector<std::uint32_t> &, std::uint32_t);
}
