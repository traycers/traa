#include <traaxx_simd/ancestor_table.hpp>
#include <traaxx_simd/depth.hpp>
#include <traaxx_simd/lca_batch.hpp>
#include <traaxx_simd/lca_binary_lifting.hpp>
#include <traaxx_simd/lca_naive.hpp>
#include <traaxx_simd/lca_set.hpp>
#include <traaxx_simd/traaxx_simd.hpp>
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>

namespace
{
    // docs/learning/array-trees/01-example-tree.md
    traaxx_simd::Tree<std::uint32_t> exampleTree()
    {
        auto parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
        auto sibling = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 6, 8, 9, 9, 11, 12 };
        auto data = std::vector<std::uint32_t>(parent.size(), 0u);
        return traaxx_simd::Tree<std::uint32_t>{ std::move(parent), std::move(sibling), std::move(data) };
    }
}

TEST(Lca, NaiveMatchesWorkedTraces)
{
    auto const tree = exampleTree();
    EXPECT_EQ(traaxx_simd::lca_naive(tree, 8u, 11u).value(), 4u);
    EXPECT_EQ(traaxx_simd::lca_naive(tree, 8u, 10u).value(), 6u);
    EXPECT_EQ(traaxx_simd::lca_naive(tree, 10u, 12u).value(), 1u);
    EXPECT_EQ(traaxx_simd::lca_naive(tree, 2u, 8u).value(), 2u);
}

TEST(Lca, BinaryLiftingAgreesWithNaive)
{
    auto const tree = exampleTree();
    auto const node_depth = traaxx_simd::depth(tree.parent(), tree.nodes_count()).value();
    auto const d_max = *std::max_element(node_depth.begin(), node_depth.end());
    auto const table = traaxx_simd::AncestorTable<std::uint32_t>(tree.parent(), d_max);
    EXPECT_EQ(traaxx_simd::lca_binary_lifting(tree, table, node_depth, 8u, 11u), 4u);
    EXPECT_EQ(traaxx_simd::lca_binary_lifting(tree, table, node_depth, 8u, 10u), 6u);
    EXPECT_EQ(traaxx_simd::lca_binary_lifting(tree, table, node_depth, 10u, 12u), 1u);
    EXPECT_EQ(traaxx_simd::lca_binary_lifting(tree, table, node_depth, 2u, 8u), 2u);
}

TEST(Lca, BatchAgreesWithBinaryLifting)
{
    auto const tree = exampleTree();
    auto const node_depth = traaxx_simd::depth(tree.parent(), tree.nodes_count()).value();
    auto const d_max = *std::max_element(node_depth.begin(), node_depth.end());
    auto const table = traaxx_simd::AncestorTable<std::uint32_t>(tree.parent(), d_max);
    auto const a = std::vector<std::uint32_t>{ 8, 8, 10, 2 };
    auto const b = std::vector<std::uint32_t>{ 11, 10, 12, 8 };
    auto const result = traaxx_simd::lca_batch(tree, table, node_depth, a, b);
    auto const expected = std::vector<std::uint32_t>{ 4, 6, 1, 2 };
    EXPECT_EQ(result, expected);
}

TEST(Lca, SetOfThreeAcrossTwoGroups)
{
    auto const tree = exampleTree();
    auto const s = std::vector<std::uint32_t>{ 8, 11, 12 };
    EXPECT_EQ(traaxx_simd::lca_set(tree, s), 1u);
}

TEST(Lca, SetOfTwoWithinOneGroup)
{
    auto const tree = exampleTree();
    auto const s = std::vector<std::uint32_t>{ 8, 11 };
    EXPECT_EQ(traaxx_simd::lca_set(tree, s), 4u);
}

TEST(Lca, SetSingletonReturnsItself)
{
    auto const tree = exampleTree();
    auto const s = std::vector<std::uint32_t>{ 8 };
    EXPECT_EQ(traaxx_simd::lca_set(tree, s), 8u);
}

TEST(Lca, AncestorPairReturnsAncestor)
{
    auto const tree = exampleTree();
    EXPECT_EQ(traaxx_simd::lca_naive(tree, 2u, 2u).value(), 2u);
}
