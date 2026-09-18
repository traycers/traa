#include <traaxx_simd/nsibling.hpp>
#include <gtest/gtest.h>

TEST(NSibling, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md, cross-checked against
    // docs/learning/array-trees/10-mutations.md §11.1
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
    auto const result = traaxx_simd::nsibling(sibling);
    auto const expected = std::vector<std::uint32_t>{ 0, 1, 3, 3, 4, 5, 9, 7, 10, 9, 10, 11, 12 };
    EXPECT_EQ(result, expected);
}

TEST(NSibling, SingleNodeHasNoRightSibling)
{
    auto const sibling = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx_simd::nsibling(sibling);
    auto const expected = std::vector<std::uint32_t>{ 0 };
    EXPECT_EQ(result, expected);
}

TEST(NSibling, EmptyArray)
{
    auto const sibling = std::vector<std::uint32_t>{};
    auto const result = traaxx_simd::nsibling(sibling);
    EXPECT_TRUE(result.empty());
}
