#include <traaxx/level_count_for.hpp>

namespace traaxx
{
    template<typename IndexT>
    IndexT level_count_for(IndexT d_max)
    {
        if (d_max <= IndexT{ 1 })
        {
            return IndexT{ 1 };
        }
        auto k = IndexT{ 0 };
        auto reach = IndexT{ 1 };
        while (reach < d_max)
        {
            reach = static_cast<IndexT>(reach * IndexT{ 2 });
            ++k;
        }
        return static_cast<IndexT>(k + IndexT{ 1 });
    }

    template std::uint32_t level_count_for<std::uint32_t>(std::uint32_t);
}
