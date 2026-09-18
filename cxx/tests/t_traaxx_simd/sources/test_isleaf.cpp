#include <traaxx_simd/isleaf.hpp>
#include <gtest/gtest.h>

TEST(IsLeaf, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md, docs/learning/array-trees/03-derived-vectors.md §4
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const result = traaxx_simd::isleaf(parent);
    auto const expected = std::vector<bool>{
        false, false, false, false, false, false, false, false, true, false, true, true, true
    };
    ASSERT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(result[i], expected[i]) << "index " << i;
    }
}

TEST(IsLeaf, SingleNodeRootIsLeaf)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx_simd::isleaf(parent);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_TRUE(result[0]);
}

TEST(IsLeaf, EmptyArray)
{
    auto const parent = std::vector<std::uint32_t>{};
    auto const result = traaxx_simd::isleaf(parent);
    EXPECT_TRUE(result.empty());
}

TEST(IsLeaf, IsolatedRootAmongOtherNodesIsLeaf)
{
    // node 0 is its own (childless) root; nodes 1/2 form a separate rooted
    // subtree under node 2 - node 0's self-loop must not count as "has a child"
    auto const parent = std::vector<std::uint32_t>{ 0, 2, 2 };
    auto const result = traaxx_simd::isleaf(parent);
    auto const expected = std::vector<bool>{ true, true, false };
    ASSERT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(result[i], expected[i]) << "index " << i;
    }
}
