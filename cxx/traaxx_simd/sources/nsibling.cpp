#include <traaxx_simd/nsibling.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<IndexT> nsibling(const std::vector<IndexT> &sibling)
    {
        auto const n = sibling.size();
        auto result = std::vector<IndexT>(n);
        std::iota(result.begin(), result.end(), IndexT{ 0 });
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::for_each(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            [&](IndexT j)
            {
                auto const s = sibling[j];
                if (s != j)
                {
                    result[s] = j;
                }
            });
        return result;
    }

    template std::vector<std::uint32_t> nsibling<std::uint32_t>(const std::vector<std::uint32_t> &);
}
