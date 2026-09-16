#include <traaxx/isleaf.hpp>
#include <gtest/gtest.h>

TEST(IsLeaf, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md, docs/learning/array-trees/03-derived-vectors.md §4
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const result = traaxx::isleaf(parent);
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
    auto const result = traaxx::isleaf(parent);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_TRUE(result[0]);
}

TEST(IsLeaf, EmptyArray)
{
    auto const parent = std::vector<std::uint32_t>{};
    auto const result = traaxx::isleaf(parent);
    EXPECT_TRUE(result.empty());
}
