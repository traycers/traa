#include <traaxx_simd/permutation.hpp>
#include <type_traits>
#include <xsimd/xsimd.hpp>

namespace traaxx_simd
{
    namespace
    {
        using batch_t = xsimd::batch<std::uint32_t>;
    }

    template<typename IndexT>
    std::vector<IndexT> remap_index_vector(std::vector<IndexT> const &vec,
        std::vector<IndexT> const &new2old, std::vector<IndexT> const &old2new)
    {
        auto const n = new2old.size();
        auto out = std::vector<IndexT>(n);
        auto i = std::size_t{ 0 };
        if constexpr (std::is_same_v<IndexT, std::uint32_t>)
        {
            auto const width = batch_t::size;
            for (; i + width <= n; i += width)
            {
                auto const src = batch_t::load_unaligned(&new2old[i]);
                auto const via_vec = batch_t::gather(vec.data(), src);
                auto const remapped = batch_t::gather(old2new.data(), via_vec);
                remapped.store_unaligned(&out[i]);
            }
        }
        for (; i < n; ++i)
        {
            out[i] = old2new[vec[new2old[i]]];
        }
        return out;
    }

    template std::vector<std::uint32_t> remap_index_vector<std::uint32_t>(
        std::vector<std::uint32_t> const &, std::vector<std::uint32_t> const &, std::vector<std::uint32_t> const &);
}
