#include <traaxx/lastchild.hpp>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> lastchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling)
    {
        auto const n = parent.size();
        auto has_right_sibling = std::vector<bool>(n, false);
        for (IndexT k = 0; k < static_cast<IndexT>(n); ++k)
        {
            auto const s = sibling[k];
            if (s != k)
            {
                has_right_sibling[s] = true;
            }
        }
        auto result = std::vector<IndexT>(n);
        std::iota(result.begin(), result.end(), IndexT{ 0 });
        for (IndexT j = 0; j < static_cast<IndexT>(n); ++j)
        {
            if (parent[j] != j && !has_right_sibling[j])
            {
                result[parent[j]] = j;
            }
        }
        return result;
    }

    template std::vector<std::uint32_t> lastchild<std::uint32_t>(
        const std::vector<std::uint32_t> &, const std::vector<std::uint32_t> &);
}
