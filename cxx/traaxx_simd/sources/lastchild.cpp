#include <traaxx_simd/lastchild.hpp>
#include <algorithm>
#include <execution>
#include <numeric>
#include <traaxx_simd/bitmask.hpp>

namespace traaxx_simd
{
    template<typename IndexT>
    std::vector<IndexT> lastchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling)
    {
        auto const n = parent.size();
        auto has_right_sibling = BitMask(n);
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::for_each(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            [&](IndexT k)
            {
                auto const s = sibling[k];
                if (s != k)
                {
                    has_right_sibling.atomic_or(s);
                }
            });
        auto result = std::vector<IndexT>(n);
        std::iota(result.begin(), result.end(), IndexT{ 0 });
        std::for_each(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            [&](IndexT j)
            {
                if (parent[j] != j && !has_right_sibling.get(j))
                {
                    result[parent[j]] = j;
                }
            });
        return result;
    }

    template std::vector<std::uint32_t> lastchild<std::uint32_t>(
        const std::vector<std::uint32_t> &, const std::vector<std::uint32_t> &);
}
