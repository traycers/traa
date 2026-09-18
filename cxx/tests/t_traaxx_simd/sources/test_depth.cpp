#include <traaxx_simd/depth.hpp>
#include <gtest/gtest.h>

TEST(Depth, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const result = traaxx_simd::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector
        = std::vector<std::uint32_t>{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 4, 5, 5, 5 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_depth_vector);
}

TEST(Depth, Root)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx_simd::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 0 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_depth_vector);
}

TEST(Depth, RootNotAtZero)
{
    auto const parent = std::vector<std::uint32_t>{ 1, 1, 1 };
    auto const result = traaxx_simd::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 1, 0, 1 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_depth_vector);
}

TEST(Depth, MultipleRoots)
{
    auto const parent = std::vector<std::uint32_t>{ 0, 2, 2, 0 };
    auto const result = traaxx_simd::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 0, 1, 0, 1 };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, expected_depth_vector);
}

TEST(Depth, EmptyArray)
{
    auto const parent = std::vector<std::uint32_t>{};
    auto const result = traaxx_simd::depth(parent, static_cast<std::uint32_t>(parent.size()));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::vector<std::uint32_t>{});
}

TEST(Depth, NoRoot)
{
    // parent[i] never equals i, so propagation never converges within max_iterations
    auto const parent = std::vector<std::uint32_t>{ 1, 0 };
    auto const max_iterations = static_cast<std::uint32_t>(parent.size());
    auto const result = traaxx_simd::depth(parent, max_iterations);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().iterations, max_iterations);
}
