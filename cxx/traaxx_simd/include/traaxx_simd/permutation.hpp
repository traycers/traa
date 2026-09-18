#pragma once
#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <vector>

namespace traaxx_simd
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

    // Declared here (not defined header-only, unlike inverse/gather above): the
    // body is templated only on IndexT (not T), so — like dfs_reorder_tables —
    // it can live in permutation.cpp, explicit-instantiated, and vectorized:
    // `old2new[vec[new2old[k]]]` is a chained double-gather with no branch, a
    // genuinely clean batch::gather target that gather<T,...>'s generic T (and
    // inverse's scatter shape) are not.
    template<typename IndexT>
    std::vector<IndexT> remap_index_vector(std::vector<IndexT> const &vec,
        std::vector<IndexT> const &new2old, std::vector<IndexT> const &old2new);
}
