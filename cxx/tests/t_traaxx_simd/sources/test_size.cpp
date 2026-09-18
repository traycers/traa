#include <traaxx_simd/size.hpp>
#include <gtest/gtest.h>

TEST(Size, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md;
    // expected size given directly in docs/learning/array-trees/07-dfs-order.md §8.1
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const result = traaxx_simd::size(parent);
    auto const expected_size
        = std::vector<std::uint32_t>{ 13, 12, 7, 4, 6, 3, 3, 2, 1, 2, 1, 1, 1 };
    EXPECT_EQ(result, expected_size);
}

TEST(Size, SingleNode)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx_simd::size(parent);
    EXPECT_EQ(result, (std::vector<std::uint32_t>{ 1 }));
}

TEST(Size, EmptyArray)
{
    auto const parent = std::vector<std::uint32_t>{};
    auto const result = traaxx_simd::size(parent);
    EXPECT_EQ(result, std::vector<std::uint32_t>{});
}

TEST(Size, LinearChain)
{
    // parent[i] <= i for all i, as size() requires (unlike depth/rank, it does not
    // go through propagate<>, so it needs the topological-order invariant to hold).
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 2 };
    auto const result = traaxx_simd::size(parent);
    auto const expected_size = std::vector<std::uint32_t>{ 4, 3, 2, 1 };
    EXPECT_EQ(result, expected_size);
}
