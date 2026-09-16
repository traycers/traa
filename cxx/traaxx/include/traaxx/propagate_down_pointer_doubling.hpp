#pragma once
#include <cstdint>
#include <traaxx/ancestor_table.hpp>
#include <traaxx/bitmask.hpp>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    BitMask<IndexT> propagate_down_pointer_doubling(
        const std::vector<IndexT> &parent, const AncestorTable<IndexT> &table, BitMask<IndexT> seed);
}
