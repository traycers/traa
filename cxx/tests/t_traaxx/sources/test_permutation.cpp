#include <traaxx/permutation.hpp>
#include <gtest/gtest.h>
#include <cstdint>
#include <numeric>
#include <vector>

TEST(Permutation, InverseMatchesShuffledExample)
{
    // docs/learning/array-trees/06-permutations.md §7.4
    auto const new2old
        = std::vector<std::uint32_t>{ 8, 3, 0, 11, 5, 1, 9, 12, 2, 6, 4, 10, 7 };
    auto const old2new = traaxx::inverse(new2old);
    auto const expected_old2new
        = std::vector<std::uint32_t>{ 2, 5, 8, 1, 10, 4, 9, 12, 0, 6, 11, 3, 7 };
    EXPECT_EQ(old2new, expected_old2new);
}

TEST(Permutation, GatherPlainlyPermutesData)
{
    auto const new2old = std::vector<std::uint32_t>{ 2, 0, 1 };
    auto const data = std::vector<std::uint32_t>{ 100, 101, 102 };
    auto const gathered = traaxx::gather(data, new2old);
    auto const expected = std::vector<std::uint32_t>{ 102, 100, 101 };
    EXPECT_EQ(gathered, expected);
}

TEST(Permutation, RemapIndexVectorTranslatesShuffledParentSibling)
{
    // docs/learning/array-trees/01-example-tree.md (original) and
    // 06-permutations.md §7.4 (shuffled new2old and expected parent'/sibling')
    auto const parent
        = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
    auto const sibling
        = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
    auto const new2old
        = std::vector<std::uint32_t>{ 8, 3, 0, 11, 5, 1, 9, 12, 2, 6, 4, 10, 7 };
    auto const old2new = traaxx::inverse(new2old);
    auto const parent_prime = traaxx::remap_index_vector(parent, new2old, old2new);
    auto const sibling_prime = traaxx::remap_index_vector(sibling, new2old, old2new);
    auto const expected_parent_prime
        = std::vector<std::uint32_t>{ 9, 5, 2, 6, 1, 2, 10, 12, 5, 10, 8, 9, 4 };
    auto const expected_sibling_prime
        = std::vector<std::uint32_t>{ 0, 8, 2, 3, 4, 5, 9, 7, 8, 9, 10, 0, 12 };
    EXPECT_EQ(parent_prime, expected_parent_prime);
    EXPECT_EQ(sibling_prime, expected_sibling_prime);
}

TEST(Permutation, IdentityPermutationIsNoOp)
{
    auto const n = std::size_t{ 5 };
    auto new2old = std::vector<std::uint32_t>(n);
    std::iota(new2old.begin(), new2old.end(), std::uint32_t{ 0 });
    auto const old2new = traaxx::inverse(new2old);
    EXPECT_EQ(old2new, new2old);
    auto const parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2 };
    auto const remapped = traaxx::remap_index_vector(parent, new2old, old2new);
    EXPECT_EQ(remapped, parent);
}
