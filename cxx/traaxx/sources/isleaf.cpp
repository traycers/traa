#include <traaxx/isleaf.hpp>
#include <algorithm>
#include <execution>
#include <numeric>
#include <traaxx/bitmask.hpp>

namespace traaxx
{
    template<typename IndexT>
    std::vector<bool> isleaf(const std::vector<IndexT> &parent)
    {
        auto const n = parent.size();
        if (n == 0)
        {
            return std::vector<bool>{};
        }
        auto is_parent = BitMask(n);
        auto indices = std::vector<IndexT>(n);
        std::iota(
            indices.begin(),   //
            indices.end(),     //
            IndexT{ 0 });
        std::for_each(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            [&](IndexT i)
            {
                auto const p = parent[i];
                if (p != i)
                {
                    is_parent.atomic_or(p);
                }
            });
        is_parent.invert();
        auto result = std::vector<bool>(n);
        std::transform(
            indices.cbegin(),   //
            indices.cend(),     //
            result.begin(),     //
            [&](IndexT i) { return is_parent.get(i); });
        return result;
    }

    template std::vector<bool> isleaf<std::uint32_t>(const std::vector<std::uint32_t> &);
}
