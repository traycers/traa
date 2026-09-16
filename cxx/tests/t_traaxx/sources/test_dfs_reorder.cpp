#include <traaxx/dfs_reorder.hpp>
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
}

TEST(DfsReorder, FreeFunctionMatchesWorkedExample)
{
    // docs/learning/array-trees/07-dfs-order.md §8.3
    auto const parent = example_parent();
    auto const sibling = example_sibling();
    auto data = std::vector<std::uint32_t>(parent.size());
    std::iota(data.begin(), data.end(), std::uint32_t{ 0 });
    auto const tree = traaxx::Tree<std::uint32_t>{ parent, sibling, data };
    auto const result = traaxx::dfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    auto const expected_new2old
        = std::vector<std::uint32_t>{ 0, 1, 2, 4, 6, 8, 10, 9, 11, 3, 5, 7, 12 };
    auto const expected_old2new
        = std::vector<std::uint32_t>{ 0, 1, 2, 9, 3, 10, 4, 11, 5, 7, 6, 8, 12 };
    auto const expected_parent_prime
        = std::vector<std::uint32_t>{ 0, 0, 1, 2, 3, 4, 4, 3, 7, 1, 9, 10, 11 };
    auto const expected_sibling_prime
        = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4, 5, 5, 4, 8, 2, 10, 11, 12 };
    EXPECT_EQ(result->new_to_old, expected_new2old);
    EXPECT_EQ(result->old_to_new, expected_old2new);
    EXPECT_EQ(result->tree.parent(), expected_parent_prime);
    EXPECT_EQ(result->tree.sibling(), expected_sibling_prime);
    EXPECT_EQ(result->tree.data(), traaxx::gather(data, expected_new2old));
}

TEST(DfsReorder, MethodMutatesInPlaceAndReturnsOnlyTables)
{
    // Same worked example, via the mutating Tree::dfs_reorder() method.
    auto tree = traaxx::Tree<std::uint32_t>{ example_parent(), example_sibling(),
        std::vector<std::uint32_t>(example_parent().size()) };
    auto const result = tree.dfs_reorder();
    ASSERT_TRUE(result.has_value());
    auto const expected_new2old
        = std::vector<std::uint32_t>{ 0, 1, 2, 4, 6, 8, 10, 9, 11, 3, 5, 7, 12 };
    auto const expected_parent_prime
        = std::vector<std::uint32_t>{ 0, 0, 1, 2, 3, 4, 4, 3, 7, 1, 9, 10, 11 };
    EXPECT_EQ(result->new_to_old, expected_new2old);
    EXPECT_EQ(tree.parent(), expected_parent_prime);
}

TEST(DfsReorder, SingleRootIsIdentity)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const sibling = std::vector<std::uint32_t>{ 0 };
    auto const tree = traaxx::Tree<std::uint32_t>{ parent, sibling, std::vector<std::uint32_t>{ 42 } };
    auto const result = traaxx::dfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->new_to_old, (std::vector<std::uint32_t>{ 0 }));
    EXPECT_EQ(result->old_to_new, (std::vector<std::uint32_t>{ 0 }));
}

TEST(DfsReorder, EmptyTreeProducesEmptyTables)
{
    auto const tree = traaxx::Tree<std::uint32_t>{};
    auto const result = traaxx::dfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->new_to_old.empty());
    EXPECT_TRUE(result->old_to_new.empty());
}

TEST(DfsReorder, DoesNotConvergeReturnsConvergenceError)
{
    // parent[i] never equals i, depth() never converges within n iterations.
    auto const parent = std::vector<std::uint32_t>{ 1, 0 };
    auto const sibling = std::vector<std::uint32_t>{ 0, 1 };
    auto const tree = traaxx::Tree<std::uint32_t>{ parent, sibling, std::vector<std::uint32_t>{ 0, 1 } };
    auto const result = traaxx::dfs_reorder(tree);
    ASSERT_FALSE(result.has_value());
}
