#pragma once
#include <cstdint>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    class AncestorTable
    {
    public:
        AncestorTable(const std::vector<IndexT> &parent, IndexT d_max);
        const std::vector<IndexT> &level(IndexT k) const;
        IndexT level_count() const;

    private:
        std::vector<std::vector<IndexT>> levels_;
    };
}
