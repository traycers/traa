#include <traaxx_simd/propagate_up_single_pass.hpp>

namespace traaxx_simd
{
    // Strictly sequential in reverse i (each step reads a bit `seed` just set
    // for a later index) — not data-parallel, matches cxx/traaxx exactly.
    template<typename IndexT>
    BitMask propagate_up_single_pass(const std::vector<IndexT> &parent, BitMask seed)
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

    template BitMask propagate_up_single_pass<std::uint32_t>(const std::vector<std::uint32_t> &, BitMask);
}
