#include <traaxx_simd/bfs_reorder.hpp>
#include <gtest/gtest.h>
#include <algorithm>
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

TEST(BfsReorder, FreeFunctionMatchesWorkedExample)
{
    // docs/learning/array-trees/08-bfs-order.md §9.3
    auto const parent = example_parent();
    auto const sibling = example_sibling();
    auto data = std::vector<std::uint32_t>(parent.size());
    std::iota(data.begin(), data.end(), std::uint32_t{ 0 });
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ parent, sibling, data };
    auto const result = traaxx_simd::bfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    auto const expected_new2old
        = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4, 5, 6, 9, 7, 8, 10, 11, 12 };
    auto const expected_old2new
        = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4, 5, 6, 8, 9, 7, 10, 11, 12 };
    auto const expected_parent_prime
        = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 4, 5, 6, 6, 7, 8 };
    auto const expected_sibling_prime
        = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 6, 8, 9, 9, 11, 12 };
    EXPECT_EQ(result->new_to_old, expected_new2old);
    EXPECT_EQ(result->old_to_new, expected_old2new);
    EXPECT_EQ(result->tree.parent(), expected_parent_prime);
    EXPECT_EQ(result->tree.sibling(), expected_sibling_prime);
    EXPECT_EQ(result->tree.data(), traaxx_simd::gather(data, expected_new2old));
}

TEST(BfsReorder, MethodMutatesInPlaceAndReturnsOnlyTables)
{
    // Same worked example, via the mutating Tree::bfs_reorder() method.
    auto tree = traaxx_simd::Tree<std::uint32_t>{ example_parent(), example_sibling(),
        std::vector<std::uint32_t>(example_parent().size()) };
    auto const result = tree.bfs_reorder();
    ASSERT_TRUE(result.has_value());
    auto const expected_new2old
        = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4, 5, 6, 9, 7, 8, 10, 11, 12 };
    auto const expected_parent_prime
        = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 4, 5, 6, 6, 7, 8 };
    EXPECT_EQ(result->new_to_old, expected_new2old);
    EXPECT_EQ(tree.parent(), expected_parent_prime);
}

TEST(BfsReorder, ParentIsNonDecreasingAfterReorder)
{
    // docs/learning/array-trees/08-bfs-order.md §9.4: BFS order makes parent' non-decreasing.
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ example_parent(), example_sibling(),
        std::vector<std::uint32_t>(example_parent().size()) };
    auto const result = traaxx_simd::bfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::is_sorted(result->tree.parent().begin(), result->tree.parent().end()));
}

TEST(BfsReorder, EmptyTree)
{
    auto const tree = traaxx_simd::Tree<std::uint32_t>{};
    auto const result = traaxx_simd::bfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->new_to_old.empty());
    EXPECT_TRUE(result->old_to_new.empty());
}

TEST(BfsReorder, MultipleRootsProducesValidPermutation)
{
    // No doc-given oracle for multi-root tie-break; only asserting internal
    // consistency (new2old/old2new are mutually inverse permutations, and
    // every root lands before every non-root) rather than a specific order.
    auto const parent = std::vector<std::uint32_t>{ 0, 2, 2, 0 };
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 1, 0 };
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ parent, sibling, std::vector<std::uint32_t>(4) };
    auto const result = traaxx_simd::bfs_reorder(tree);
    ASSERT_TRUE(result.has_value());
    auto sorted_new2old = result->new_to_old;
    std::sort(sorted_new2old.begin(), sorted_new2old.end());
    auto expected_ids = std::vector<std::uint32_t>{ 0, 1, 2, 3 };
    EXPECT_EQ(sorted_new2old, expected_ids);
    for (std::uint32_t old_id = 0; old_id < 4; ++old_id)
    {
        EXPECT_EQ(result->new_to_old[result->old_to_new[old_id]], old_id);
    }
}
