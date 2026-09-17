#pragma once
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> inverse(std::vector<IndexT> const &new2old)
    {
        auto const n = new2old.size();
        auto old2new = std::vector<IndexT>(n);
        auto indices = std::vector<IndexT>(n);
        std::iota(indices.begin(), indices.end(), IndexT{ 0 });
        std::for_each(
            std::execution::par,   //
            indices.cbegin(),      //
            indices.cend(),        //
            [&](IndexT k) { old2new[new2old[k]] = k; });
        return old2new;
    }

    template<typename T, typename IndexT>
    std::vector<T> gather(std::vector<T> const &data, std::vector<IndexT> const &new2old)
    {
        auto out = std::vector<T>(new2old.size());
        std::transform(
            std::execution::par,   //
            new2old.cbegin(),      //
            new2old.cend(),        //
            out.begin(),           //
            [&](IndexT src) { return data[src]; });
        return out;
    }

    template<typename IndexT>
    std::vector<IndexT> remap_index_vector(std::vector<IndexT> const &vec,
        std::vector<IndexT> const &new2old, std::vector<IndexT> const &old2new)
    {
        auto out = std::vector<IndexT>(new2old.size());
        std::transform(
            std::execution::par,   //
            new2old.cbegin(),      //
            new2old.cend(),        //
            out.begin(),           //
            [&](IndexT src) { return old2new[vec[src]]; });
        return out;
    }
}
