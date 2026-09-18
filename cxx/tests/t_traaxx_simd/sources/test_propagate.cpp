#include <traaxx_simd/propagate.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <vector>

namespace
{
    std::vector<std::uint32_t> make_indices(std::size_t n)
    {
        std::vector<std::uint32_t> indices(n);
        std::iota(indices.begin(), indices.end(), std::uint32_t{ 0 });
        return indices;
    }

    auto depth_like_step(
        std::vector<std::uint32_t> const &parent, std::vector<std::uint32_t> const &indices)
    {
        return [&](std::vector<std::uint32_t> const &current, std::vector<std::uint32_t> &next)
        {
            std::transform(
                std::execution::par, indices.begin(), indices.end(), next.begin(),
                [&](std::uint32_t i)
                {
                    return parent[i] == i ? std::uint32_t{ 0 }
                                          : static_cast<std::uint32_t>(current[parent[i]] + std::uint32_t{ 1 });
                });
        };
    }
}

TEST(Propagate, ConvergesOnExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const n = parent.size();
    auto const indices = make_indices(n);
    auto const result = traaxx_simd::propagate(
        std::vector<std::uint32_t>(n, std::uint32_t{ 0 }),
        depth_like_step(parent, indices),
        static_cast<std::uint32_t>(n));
    ASSERT_TRUE(result.has_value());
    auto const expected_state = std::vector<std::uint32_t>{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 4, 5, 5, 5 };
    EXPECT_EQ(result->state, expected_state);
    EXPECT_EQ(result->iterations, 6u);
}

TEST(Propagate, RootNotAtZeroConverges)
{
    auto const parent = std::vector<std::uint32_t>{ 1, 1, 1 };
    auto const n = parent.size();
    auto const indices = make_indices(n);
    auto const result = traaxx_simd::propagate(
        std::vector<std::uint32_t>(n, std::uint32_t{ 0 }),
        depth_like_step(parent, indices),
        static_cast<std::uint32_t>(n));
    ASSERT_TRUE(result.has_value());
    auto const expected_state = std::vector<std::uint32_t>{ 1, 0, 1 };
    EXPECT_EQ(result->state, expected_state);
    EXPECT_EQ(result->iterations, 2u);
}

TEST(Propagate, DoesNotConvergeReturnsConvergenceError)
{
    // parent[i] never equals i, no fixed point reachable within max_iterations
    auto const parent = std::vector<std::uint32_t>{ 1, 0 };
    auto const n = parent.size();
    auto const indices = make_indices(n);
    auto const max_iterations = static_cast<std::uint32_t>(n);
    auto const result = traaxx_simd::propagate(
        std::vector<std::uint32_t>(n, std::uint32_t{ 0 }),
        depth_like_step(parent, indices),
        max_iterations);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().iterations, max_iterations);
}

TEST(Propagate, AlreadyFixedPointConvergesInOneIteration)
{
    auto const state = std::vector<std::uint32_t>{ 5, 5, 5 };
    auto const step = [](std::vector<std::uint32_t> const &current, std::vector<std::uint32_t> &next)
    {
        next = current;
    };
    auto const result = traaxx_simd::propagate(state, step, 10u);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->state, state);
    EXPECT_EQ(result->iterations, 1u);
}

TEST(Propagate, GenericOverStateType)
{
    // StateT need not be std::vector<IndexT> — any type supporting ==, swap and copy works.
    auto const state = std::vector<double>{ 1.5, 2.5 };
    auto const step = [](std::vector<double> const &current, std::vector<double> &next)
    {
        next = current;
    };
    auto const result = traaxx_simd::propagate(state, step, std::uint32_t{ 3 });
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->state, state);
    EXPECT_EQ(result->iterations, 1u);
}
