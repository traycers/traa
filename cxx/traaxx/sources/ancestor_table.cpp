#include <traaxx/ancestor_table.hpp>
#include <traaxx/level_count_for.hpp>
#include <algorithm>
#include <execution>
#include <numeric>

namespace traaxx
{
    template<typename IndexT>
    AncestorTable<IndexT>::AncestorTable(const std::vector<IndexT> &parent, IndexT d_max)
    {
        auto const n = parent.size();
        auto const level_count = level_count_for(d_max);
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        levels_.reserve(level_count);
        levels_.push_back(parent);
        for (IndexT k = 1; k < level_count; ++k)
        {
            auto const &previous = levels_.back();
            auto next = std::vector<IndexT>(n);
            std::transform(
                std::execution::par,   //
                indices.cbegin(),      //
                indices.cend(),        //
                next.begin(),          //
                [&](IndexT i) { return previous[previous[i]]; });
            levels_.push_back(std::move(next));
        }
    }

    template<typename IndexT>
    const std::vector<IndexT> &AncestorTable<IndexT>::level(IndexT k) const
    {
        return levels_[k];
    }

    template<typename IndexT>
    IndexT AncestorTable<IndexT>::level_count() const
    {
        return static_cast<IndexT>(levels_.size());
    }

    template class AncestorTable<std::uint32_t>;
}
