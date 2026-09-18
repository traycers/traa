#include <traaxx_simd/depth.hpp>
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
    std::expected<std::vector<IndexT>, ConvergenceError<IndexT>> depth(
        const std::vector<IndexT> &parent, IndexT max_iterations)
    {
        auto const n = parent.size();
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
                    auto const p = batch_t::load_unaligned(&parent[i]);
                    auto const is_root = p == idx;
                    auto const gathered = batch_t::gather(current.data(), p);
                    auto const value = xsimd::select(is_root, zero, gathered + one);
                    value.store_unaligned(&next[i]);
                }
            }
            for (; i < n; ++i)
            {
                next[i] = parent[i] == static_cast<IndexT>(i) ? IndexT{ 0 }
                                                               : static_cast<IndexT>(current[parent[i]] + IndexT{ 1 });
            }
        };
        auto result = propagate(std::vector<IndexT>(n, IndexT{ 0 }), step, max_iterations);
        if (!result.has_value())
        {
            return std::unexpected(result.error());
        }
        return std::move(result->state);
    }

    template std::expected<std::vector<std::uint32_t>, ConvergenceError<std::uint32_t>> depth<std::uint32_t>(
        const std::vector<std::uint32_t> &, std::uint32_t);
}
