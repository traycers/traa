#include <traaxx/size.hpp>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> size(const std::vector<IndexT> &parent)
    {
        auto const n = parent.size();
        auto result = std::vector<IndexT>(n, IndexT{ 1 });
        for (auto i = n; i-- > 1;)
        {
            result[parent[i]] = static_cast<IndexT>(result[parent[i]] + result[i]);
        }
        return result;
    }

    template std::vector<std::uint32_t> size<std::uint32_t>(const std::vector<std::uint32_t> &);
}
