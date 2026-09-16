#include <traaxx/isleaf.hpp>

namespace traaxx
{
    template<typename IndexT>
    std::vector<bool> isleaf(const std::vector<IndexT> &parent)
    {
        auto const n = parent.size();
        auto has_child = std::vector<bool>(n, false);
        for (IndexT j = 0; j < static_cast<IndexT>(n); ++j)
        {
            auto const p = parent[j];
            if (p != j)
            {
                has_child[p] = true;
            }
        }
        auto result = std::vector<bool>(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            result[i] = !has_child[i];
        }
        return result;
    }

    template std::vector<bool> isleaf<std::uint32_t>(const std::vector<std::uint32_t> &);
}
