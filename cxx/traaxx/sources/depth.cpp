#include <traaxx/depth.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
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
            std::transform(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
                next.begin(),          //
                [&](IndexT i)
                {
                    return parent[i] == i ? IndexT{ 0 }
                                          : static_cast<IndexT>(current[parent[i]] + IndexT{ 1 });
                });
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
