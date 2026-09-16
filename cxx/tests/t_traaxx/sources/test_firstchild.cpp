#include <traaxx/firstchild.hpp>
#include <gtest/gtest.h>

TEST(FirstChild, ExampleTree)
{
    // docs/learning/array-trees/01-example-tree.md
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const sibling = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
    auto const result = traaxx::firstchild(parent, sibling);
    // leaves (8, 10, 11, 12) have no children — sentinel is self-reference.
    auto const expected
        = std::vector<std::uint32_t>{ 1, 2, 4, 5, 6, 7, 8, 12, 8, 11, 10, 11, 12 };
    EXPECT_EQ(result, expected);
}

TEST(FirstChild, SingleNodeHasNoChildren)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const sibling = std::vector<std::uint32_t>{ 0 };
    auto const result = traaxx::firstchild(parent, sibling);
    auto const expected = std::vector<std::uint32_t>{ 0 };
    EXPECT_EQ(result, expected);
}
