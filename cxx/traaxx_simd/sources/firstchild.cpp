#include <traaxx_simd/firstchild.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<IndexT> firstchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling)
    {
        auto const n = parent.size();
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
                if (sibling[j] == j && parent[j] != j)
                {
                    result[parent[j]] = j;
                }
            });
        return result;
    }

    template std::vector<std::uint32_t> firstchild<std::uint32_t>(
        const std::vector<std::uint32_t> &, const std::vector<std::uint32_t> &);
}
