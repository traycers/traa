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
        auto has_child = BitMask<IndexT>(static_cast<IndexT>(n));
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::for_each(std::execution::par, indices.begin(), indices.end(),
            [&](IndexT j)
            {
                auto const p = parent[j];
                if (p != j)
                {
                    has_child.atomic_or(p);
                }
            });
        auto result = std::vector<bool>(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            result[i] = !has_child.get(static_cast<IndexT>(i));
        }
        return result;
    }

    template std::vector<bool> isleaf<std::uint32_t>(const std::vector<std::uint32_t> &);
}
