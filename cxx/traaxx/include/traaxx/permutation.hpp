#pragma once
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    std::vector<IndexT> inverse(std::vector<IndexT> const &new2old)
    {
        auto old2new = std::vector<IndexT>(new2old.size());
        for (IndexT k = 0; k < static_cast<IndexT>(new2old.size()); ++k)
        {
            old2new[new2old[k]] = k;
        }
        return old2new;
    }

    template<typename T, typename IndexT>
    std::vector<T> gather(std::vector<T> const &data, std::vector<IndexT> const &new2old)
    {
        auto out = std::vector<T>(new2old.size());
        for (std::size_t k = 0; k < new2old.size(); ++k)
        {
            out[k] = data[new2old[k]];
        }
        return out;
    }

    template<typename IndexT>
    std::vector<IndexT> remap_index_vector(std::vector<IndexT> const &vec,
        std::vector<IndexT> const &new2old, std::vector<IndexT> const &old2new)
    {
        auto out = std::vector<IndexT>(new2old.size());
        for (std::size_t k = 0; k < new2old.size(); ++k)
        {
            out[k] = old2new[vec[new2old[k]]];
        }
        return out;
    }
}
