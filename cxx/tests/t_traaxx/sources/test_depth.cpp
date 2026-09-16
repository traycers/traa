#include <traaxx/depth.hpp>
#include <gtest/gtest.h>
#include <stdexcept>

TEST(Depth, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const result = traaxx::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector
        = std::vector<std::uint32_t>{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 4, 5, 5, 5 };
    auto const expected_iterations = 6u;
    EXPECT_EQ(result.depth_vector, expected_depth_vector);
    EXPECT_EQ(result.iterations, expected_iterations);
}

TEST(Depth, Root)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 0 };
    auto const expected_iterations = 1u;
    EXPECT_EQ(result.depth_vector, expected_depth_vector);
    EXPECT_EQ(result.iterations, expected_iterations);
}

TEST(Depth, RootNotAtZero)
{
    auto const parent = std::vector<std::uint32_t>{ 1, 1, 1 };
    auto const result = traaxx::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 1, 0, 1 };
    auto const expected_iterations = 2u;
    EXPECT_EQ(result.depth_vector, expected_depth_vector);
    EXPECT_EQ(result.iterations, expected_iterations);
}

TEST(Depth, MultipleRoots)
{
    auto const parent = std::vector<std::uint32_t>{ 0, 2, 2, 0 };
    auto const result = traaxx::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected_depth_vector = std::vector<std::uint32_t>{ 0, 1, 0, 1 };
    auto const expected_iterations = 2u;
    EXPECT_EQ(result.depth_vector, expected_depth_vector);
    EXPECT_EQ(result.iterations, expected_iterations);
}

TEST(Depth, EmptyArray)
{
    auto const parent = std::vector<std::uint32_t>{};
    auto const result = traaxx::depth(parent, static_cast<std::uint32_t>(parent.size()));
    auto const expected = traaxx::DepthResult<std::uint32_t>();
    EXPECT_EQ(result.depth_vector, expected.depth_vector);
    EXPECT_EQ(result.iterations, expected.iterations);
}

TEST(Depth, NoRoot)
{
    // parent[i] never equals i, so propagation never converges within max_iterations
    auto const parent = std::vector<std::uint32_t>{ 1, 0 };
    EXPECT_THROW(traaxx::depth(parent, static_cast<std::uint32_t>(parent.size())), std::runtime_error);
}
