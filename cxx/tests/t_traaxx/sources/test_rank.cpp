#include <traaxx/rank.hpp>
#include <gtest/gtest.h>

TEST(Rank, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md, sibling vector;
    // expected rank computed by hand from docs/learning/array-trees/02-path-coordinate-matrix.md §3.6:
    // rank[i] = 0 if sibling[i] == i, else rank[sibling[i]] + 1
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
    auto const result = traaxx::rank(sibling, static_cast<std::uint32_t>(sibling.size()));
    auto const expected_rank = std::vector<std::uint32_t>{ 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_rank);
}

TEST(Rank, AllFirstChildren)
{
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 2 };
    auto const result = traaxx::rank(sibling, static_cast<std::uint32_t>(sibling.size()));
    auto const expected_rank = std::vector<std::uint32_t>{ 0, 0, 0 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_rank);
}

TEST(Rank, EmptyArray)
{
    auto const sibling = std::vector<std::uint32_t>{};
    auto const result = traaxx::rank(sibling, static_cast<std::uint32_t>(sibling.size()));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::vector<std::uint32_t>{});
}

TEST(Rank, DoesNotConvergeReturnsConvergenceError)
{
    // sibling[i] never equals i, so propagation never converges within max_iterations
    auto const sibling = std::vector<std::uint32_t>{ 1, 0 };
    auto const max_iterations = static_cast<std::uint32_t>(sibling.size());
    auto const result = traaxx::rank(sibling, max_iterations);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().iterations, max_iterations);
}
