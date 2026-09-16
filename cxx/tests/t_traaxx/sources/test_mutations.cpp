#include <traaxx/traaxx.hpp>
#include <gtest/gtest.h>
#include <numeric>

namespace
{
    // docs/learning/array-trees/01-example-tree.md
    std::vector<std::uint32_t> example_parent()
    {
        return { 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    }

    std::vector<std::uint32_t> example_sibling()
    {
        return { 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
    }

    traaxx::Tree<std::uint32_t> example_tree()
    {
        auto const parent = example_parent();
        auto data = std::vector<std::uint32_t>(parent.size());
        std::iota(data.begin(), data.end(), std::uint32_t{ 0 });
        return traaxx::Tree<std::uint32_t>{ parent, example_sibling(), data };
    }
}

TEST(Mutations, AppendMatchesWorkedExample)
{
    // docs/learning/array-trees/10-mutations.md §11.2
    auto tree = example_tree();
    auto const x = tree.append(9u, 99u);
    EXPECT_EQ(x, 13u);
    EXPECT_EQ(tree.nodes_count(), 14u);
    EXPECT_EQ(tree.parent()[13], 9u);
    EXPECT_EQ(tree.sibling()[13], 11u);
    EXPECT_EQ(tree.data()[13], 99u);
}

TEST(Mutations, AppendToChildlessParentSelfLoops)
{
    auto tree = example_tree();
    auto const x = tree.append(8u);
    EXPECT_EQ(tree.sibling()[x], x);
}

TEST(Mutations, DeleteLeafMatchesWorkedExample)
{
    // docs/learning/array-trees/10-mutations.md §11.4
    auto tree = example_tree();
    tree.deleteLeaf(8u);
    EXPECT_EQ(tree.sibling()[10], 10u);
    EXPECT_EQ(tree.nodes_count(), 13u);
}

TEST(Mutations, DeleteLeafOfLastChildIsNoOp)
{
    auto tree = example_tree();
    auto const before = tree.sibling();
    tree.deleteLeaf(11u);
    EXPECT_EQ(tree.sibling(), before);
}

TEST(Mutations, SpliceMatchesWorkedExample)
{
    // docs/learning/array-trees/10-mutations.md §11.6
    auto tree = example_tree();
    tree.splice(6u);
    EXPECT_EQ(tree.parent()[8], 4u);
    EXPECT_EQ(tree.parent()[10], 4u);
    EXPECT_EQ(tree.sibling()[8], 8u);
    EXPECT_EQ(tree.sibling()[9], 10u);
}

TEST(Mutations, DeleteSubtreeMatchesWorkedExample)
{
    // docs/learning/array-trees/10-mutations.md §11.5
    auto tree = example_tree();
    auto const result = tree.deleteSubtree(3u);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(tree.nodes_count(), 9u);
    auto const expected_parent = std::vector<std::uint32_t>{ 0, 0, 1, 2, 3, 4, 3, 4, 6 };
    auto const expected_sibling = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4, 5, 4, 5, 8 };
    EXPECT_EQ(tree.parent(), expected_parent);
    EXPECT_EQ(tree.sibling(), expected_sibling);
}

TEST(Mutations, MoveMatchesWorkedExample)
{
    // docs/learning/array-trees/10-mutations.md §11.7
    auto tree = example_tree();
    auto const result = tree.move(4u, 7u);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(tree.parent()[4], 7u);
    EXPECT_EQ(tree.sibling()[4], 12u);
}

TEST(Mutations, MoveUnderOwnDescendantIsRejectedAndTreeUnchanged)
{
    auto tree = example_tree();
    auto const parent_before = tree.parent();
    auto const sibling_before = tree.sibling();
    auto const result = tree.move(1u, 4u);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().kind, traaxx::MoveError<std::uint32_t>::Kind::would_create_cycle);
    EXPECT_EQ(tree.parent(), parent_before);
    EXPECT_EQ(tree.sibling(), sibling_before);
}

TEST(Mutations, MoveOntoSelfIsRejected)
{
    auto tree = example_tree();
    auto const result = tree.move(4u, 4u);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().kind, traaxx::MoveError<std::uint32_t>::Kind::would_create_cycle);
}
