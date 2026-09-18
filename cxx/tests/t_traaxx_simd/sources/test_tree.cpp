#include <traaxx_simd/traaxx_simd.hpp>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

TEST(Tree, ConstructsFromVectors)
{
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1 };
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 1 };
    auto const data = std::vector<std::uint32_t>{ 10, 20, 30 };
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ parent, sibling, data };
    EXPECT_EQ(tree.parent(), parent);
    EXPECT_EQ(tree.sibling(), sibling);
    EXPECT_EQ(tree.data(), data);
}

TEST(Tree, NodesCountMatchesParentSize)
{
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1 };
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 1, 3 };
    auto const data = std::vector<std::uint32_t>{ 1, 2, 3, 4 };
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ parent, sibling, data };
    EXPECT_EQ(tree.nodes_count(), 4u);
}

TEST(Tree, DefaultConstructedIsEmpty)
{
    auto const tree = traaxx_simd::Tree<std::uint32_t>{};
    EXPECT_EQ(tree.nodes_count(), 0u);
    EXPECT_TRUE(tree.parent().empty());
    EXPECT_TRUE(tree.sibling().empty());
    EXPECT_TRUE(tree.data().empty());
}

TEST(Tree, SupportsNonUint32Payload)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const sibling = std::vector<std::uint32_t>{ 0 };
    auto const data = std::vector<double>{ 3.5 };
    auto const tree = traaxx_simd::Tree<double>{ parent, sibling, data };
    EXPECT_EQ(tree.data(), data);
}
