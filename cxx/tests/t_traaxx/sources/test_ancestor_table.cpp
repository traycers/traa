#include <traaxx/ancestor_table.hpp>
#include <gtest/gtest.h>

TEST(AncestorTable, ExampleTreeLevels)
{
    // docs/learning/array-trees/01-example-tree.md, depth = 5
    // docs/learning/array-trees/04-mask-propagation.md §5.4 (A1/A2/A4)
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const table = traaxx::AncestorTable<std::uint32_t>(parent, 5u);
    auto const expected_a1 = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const expected_a2 = std::vector<std::uint32_t>{ 0, 0, 0, 0, 1, 1, 2, 3, 4, 2, 4, 4, 5 };
    auto const expected_a4 = std::vector<std::uint32_t>{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1 };
    EXPECT_EQ(table.level(0), expected_a1);
    EXPECT_EQ(table.level(1), expected_a2);
    EXPECT_EQ(table.level(2), expected_a4);
}

TEST(AncestorTable, LevelCountIsCeilLog2PlusOne)
{
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const table = traaxx::AncestorTable<std::uint32_t>(parent, 5u);
    // ceil(log2(5)) = 3, plus one level of margin, per docs/learning/array-trees/04-mask-propagation.md §5.4
    EXPECT_EQ(table.level_count(), 4u);
}

TEST(AncestorTable, SingleNodeDepthOne)
{
    auto const parent = std::vector<std::uint32_t>{ 0 };
    auto const table = traaxx::AncestorTable<std::uint32_t>(parent, 1u);
    EXPECT_EQ(table.level_count(), 1u);
    EXPECT_EQ(table.level(0), parent);
}
