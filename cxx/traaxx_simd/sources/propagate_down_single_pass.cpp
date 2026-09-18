#include <traaxx_simd/propagate_down_single_pass.hpp>

namespace traaxx_simd
{
    // Strictly sequential in i (each step reads a bit `seed` just wrote for an
    // earlier index) — not data-parallel, matches cxx/traaxx exactly.
    template<typename IndexT>
    BitMask propagate_down_single_pass(const std::vector<IndexT> &parent, BitMask seed)
    {
        auto const n = static_cast<IndexT>(parent.size());
        for (IndexT i = 0; i < n; ++i)
        {
            if (seed.get(parent[i]))
            {
                seed.set(i);
            }
        }
        return seed;
    }

    template BitMask propagate_down_single_pass<std::uint32_t>(const std::vector<std::uint32_t> &, BitMask);
}
