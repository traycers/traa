#include <traaxx/nsibling.hpp>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> nsibling(const std::vector<IndexT> &sibling)
    {
        auto const n = sibling.size();
        auto result = std::vector<IndexT>(n);
        std::iota(result.begin(), result.end(), IndexT{ 0 });
        for (IndexT j = 0; j < static_cast<IndexT>(n); ++j)
        {
            auto const s = sibling[j];
            if (s != j)
            {
                result[s] = j;
            }
        }
        return result;
    }

    template std::vector<std::uint32_t> nsibling<std::uint32_t>(const std::vector<std::uint32_t> &);
}
