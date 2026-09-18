#include <traaxx_simd/lca_batch.hpp>
#include <traaxx_simd/lca_binary_lifting.hpp>
#include <type_traits>
#include <xsimd/xsimd.hpp>

namespace traaxx_simd
{
    namespace
    {
        using batch_t = xsimd::batch<std::uint32_t>;
    }

    // Masked/branchless batch-LCA (§10.5): instead of branching per query pair
    // like detail::lca_climb, every step below runs for all lanes in the batch
    // and xsimd::select picks the surviving value per lane — the "where"
    // comparison this track was chosen (over Highway) to cover with
    // batch::gather + xsimd::select.
    template<typename IndexT>
    std::vector<IndexT> lca_batch_core(const std::vector<IndexT> &parent, const std::vector<IndexT> &node_depth,
        const AncestorTable<IndexT> &table, std::vector<IndexT> const &a_in, std::vector<IndexT> const &b_in)
    {
        auto const m = a_in.size();
        auto result = std::vector<IndexT>(m);
        auto i = std::size_t{ 0 };
        if constexpr (std::is_same_v<IndexT, std::uint32_t>)
        {
            auto const width = batch_t::size;
            auto const zero = batch_t(std::uint32_t{ 0 });
            auto const one = batch_t(std::uint32_t{ 1 });
            for (; i + width <= m; i += width)
            {
                auto const a0 = batch_t::load_unaligned(&a_in[i]);
                auto const b0 = batch_t::load_unaligned(&b_in[i]);
                auto const depth_a0 = batch_t::gather(node_depth.data(), a0);
                auto const depth_b0 = batch_t::gather(node_depth.data(), b0);
                auto const swap_mask = depth_a0 < depth_b0;
                auto a = xsimd::select(swap_mask, b0, a0);
                auto b = xsimd::select(swap_mask, a0, b0);
                auto const depth_a = xsimd::select(swap_mask, depth_b0, depth_a0);
                auto const depth_b = xsimd::select(swap_mask, depth_a0, depth_b0);
                auto const diff = depth_a - depth_b;
                for (IndexT k = 0; k < table.level_count(); ++k)
                {
                    auto const bit_set = ((diff >> static_cast<int32_t>(k)) & one) != zero;
                    auto const lifted = batch_t::gather(table.level(k).data(), a);
                    a = xsimd::select(bit_set, lifted, a);
                }
                auto const equal_mask = a == b;
                for (IndexT k = table.level_count(); k-- > 0;)
                {
                    auto const la = batch_t::gather(table.level(k).data(), a);
                    auto const lb = batch_t::gather(table.level(k).data(), b);
                    auto const differ = la != lb;
                    auto const active = differ & ~equal_mask;
                    a = xsimd::select(active, la, a);
                    b = xsimd::select(active, lb, b);
                }
                auto const parent_a = batch_t::gather(parent.data(), a);
                auto const climbed = xsimd::select(equal_mask, a, parent_a);
                climbed.store_unaligned(&result[i]);
            }
        }
        for (; i < m; ++i)
        {
            result[i] = detail::lca_climb(parent, node_depth, table, a_in[i], b_in[i]);
        }
        return result;
    }

    template std::vector<std::uint32_t> lca_batch_core<std::uint32_t>(const std::vector<std::uint32_t> &,
        const std::vector<std::uint32_t> &, const AncestorTable<std::uint32_t> &, std::vector<std::uint32_t> const &,
        std::vector<std::uint32_t> const &);
}
