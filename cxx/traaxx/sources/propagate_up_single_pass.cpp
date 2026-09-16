#include <traaxx/propagate_up_single_pass.hpp>

namespace traaxx
{
    template<typename IndexT>
    BitMask<IndexT> propagate_up_single_pass(const std::vector<IndexT> &parent, BitMask<IndexT> seed)
    {
        auto const n = static_cast<IndexT>(parent.size());
        for (IndexT i = n; i-- > 0;)
        {
            if (seed.get(i))
            {
                seed.set(parent[i]);
            }
        }
        return seed;
    }

    template BitMask<std::uint32_t> propagate_up_single_pass<std::uint32_t>(
        const std::vector<std::uint32_t> &, BitMask<std::uint32_t>);
}
