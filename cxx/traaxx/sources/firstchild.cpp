#include <traaxx/firstchild.hpp>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> firstchild(const std::vector<IndexT> &parent, const std::vector<IndexT> &sibling)
    {
        auto const n = parent.size();
        auto result = std::vector<IndexT>(n);
        std::iota(result.begin(), result.end(), IndexT{ 0 });
        for (IndexT j = 0; j < static_cast<IndexT>(n); ++j)
        {
            if (sibling[j] == j && parent[j] != j)
            {
                result[parent[j]] = j;
            }
        }
        return result;
    }

    template std::vector<std::uint32_t> firstchild<std::uint32_t>(
        const std::vector<std::uint32_t> &, const std::vector<std::uint32_t> &);
}
