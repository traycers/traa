#include <traaxx/depth.hpp>
#include <algorithm>
#include <execution>
#include <numeric>
#include <stdexcept>

namespace traaxx
{
    template<typename IndexT>
    DepthResult<IndexT> depth(const std::vector<IndexT> &parent, IndexT max_iterations)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return { std::vector<IndexT>{}, IndexT{ 0 } };
        }
        std::vector<IndexT> indices(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::vector<IndexT> current(n, IndexT{ 0 });
        std::vector<IndexT> next(n, IndexT{ 0 });
        for (IndexT iteration = 0; iteration < max_iterations; ++iteration)
        {
            std::transform(
                std::execution::par, indices.begin(), indices.end(), next.begin(),
                [&](IndexT i)
                {
                    return parent[i] == i ? IndexT{ 0 }
                                          : static_cast<IndexT>(current[parent[i]] + IndexT{ 1 });
                });
            auto const changed
                = !std::equal(std::execution::par, current.begin(), current.end(), next.begin());
            current.swap(next);
            if (!changed)
            {
                return { current, static_cast<IndexT>(iteration + IndexT{ 1 }) };
            }
        }
        throw std::runtime_error("depth propagation did not converge within max_iterations");
    }

    template DepthResult<std::uint32_t> depth<std::uint32_t>(const std::vector<std::uint32_t> &, std::uint32_t);
}
