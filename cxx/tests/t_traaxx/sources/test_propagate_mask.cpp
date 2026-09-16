#include <traaxx/ancestor_table.hpp>
#include <traaxx/bitmask.hpp>
#include <traaxx/propagate_down_iterative.hpp>
#include <traaxx/propagate_down_pointer_doubling.hpp>
#include <traaxx/propagate_down_single_pass.hpp>
#include <traaxx/propagate_up_iterative.hpp>
#include <traaxx/propagate_up_pointer_doubling.hpp>
#include <traaxx/propagate_up_single_pass.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace
{
    // docs/learning/array-trees/01-example-tree.md
    std::vector<std::uint32_t> const example_parent
        = { 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    std::uint32_t const example_n = static_cast<std::uint32_t>(example_parent.size());
    std::uint32_t const example_d_max = 5;

    traaxx::BitMask<std::uint32_t> seed_at(std::uint32_t n, std::uint32_t bit)
    {
        auto mask = traaxx::BitMask<std::uint32_t>(n);
        mask.set(bit);
        return mask;
    }

    void expect_bits(traaxx::BitMask<std::uint32_t> const &mask, std::vector<std::uint32_t> const &expected)
    {
        for (std::uint32_t i = 0; i < example_n; ++i)
        {
            auto const should_be_set
                = std::find(expected.begin(), expected.end(), i) != expected.end();
            EXPECT_EQ(mask.get(i), should_be_set) << "bit " << i;
        }
    }
}

TEST(PropagateMask, DownIterativeMatchesWorkedExample)
{
    auto const result = traaxx::propagate_down_iterative(example_parent, seed_at(example_n, 2), example_n);
    ASSERT_TRUE(result.has_value());
    expect_bits(*result, { 2, 4, 6, 8, 9, 10, 11 });
}

TEST(PropagateMask, UpIterativeMatchesWorkedExample)
{
    auto const result = traaxx::propagate_up_iterative(example_parent, seed_at(example_n, 10), example_n);
    ASSERT_TRUE(result.has_value());
    expect_bits(*result, { 0, 1, 2, 4, 6, 10 });
}

TEST(PropagateMask, DownSinglePassMatchesWorkedExample)
{
    auto const result
        = traaxx::propagate_down_single_pass(example_parent, seed_at(example_n, 2));
    expect_bits(result, { 2, 4, 6, 8, 9, 10, 11 });
}

TEST(PropagateMask, UpSinglePassMatchesWorkedExample)
{
    auto const result = traaxx::propagate_up_single_pass(example_parent, seed_at(example_n, 10));
    expect_bits(result, { 0, 1, 2, 4, 6, 10 });
}

TEST(PropagateMask, DownPointerDoublingMatchesWorkedExample)
{
    auto const table = traaxx::AncestorTable<std::uint32_t>(example_parent, example_d_max);
    auto const result
        = traaxx::propagate_down_pointer_doubling(example_parent, table, seed_at(example_n, 2));
    expect_bits(result, { 2, 4, 6, 8, 9, 10, 11 });
}

TEST(PropagateMask, UpPointerDoublingMatchesWorkedExample)
{
    auto const table = traaxx::AncestorTable<std::uint32_t>(example_parent, example_d_max);
    auto const result
        = traaxx::propagate_up_pointer_doubling(example_parent, table, seed_at(example_n, 10));
    expect_bits(result, { 0, 1, 2, 4, 6, 10 });
}

TEST(PropagateMask, AllThreeStrategiesAgreeOnDown)
{
    auto const table = traaxx::AncestorTable<std::uint32_t>(example_parent, example_d_max);
    auto const iterative = traaxx::propagate_down_iterative(example_parent, seed_at(example_n, 3), example_n);
    auto const single_pass = traaxx::propagate_down_single_pass(example_parent, seed_at(example_n, 3));
    auto const doubling
        = traaxx::propagate_down_pointer_doubling(example_parent, table, seed_at(example_n, 3));
    ASSERT_TRUE(iterative.has_value());
    EXPECT_EQ(*iterative, single_pass);
    EXPECT_EQ(*iterative, doubling);
}

TEST(PropagateMask, AllThreeStrategiesAgreeOnUp)
{
    auto const table = traaxx::AncestorTable<std::uint32_t>(example_parent, example_d_max);
    auto const iterative = traaxx::propagate_up_iterative(example_parent, seed_at(example_n, 11), example_n);
    auto const single_pass = traaxx::propagate_up_single_pass(example_parent, seed_at(example_n, 11));
    auto const doubling
        = traaxx::propagate_up_pointer_doubling(example_parent, table, seed_at(example_n, 11));
    ASSERT_TRUE(iterative.has_value());
    EXPECT_EQ(*iterative, single_pass);
    EXPECT_EQ(*iterative, doubling);
}
