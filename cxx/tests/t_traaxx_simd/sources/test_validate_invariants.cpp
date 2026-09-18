#include <traaxx_simd/validate_invariants.hpp>
#include <gtest/gtest.h>
#include <algorithm>

namespace
{
    bool has_violation(traaxx_simd::ValidationResult<std::uint32_t> const &result, traaxx_simd::InvariantCheck check)
    {
        return std::any_of(result.violations.begin(), result.violations.end(),
            [check](auto const &v) { return v.check == check; });
    }

    traaxx_simd::Tree<std::uint32_t> exampleTree()
    {
        // docs/learning/array-trees/01-example-tree.md
        auto parent = std::vector<std::uint32_t>{ 0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7 };
        auto sibling = std::vector<std::uint32_t>{ 0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12 };
        auto data = std::vector<std::uint32_t>(parent.size());
        return traaxx_simd::Tree<std::uint32_t>{ std::move(parent), std::move(sibling), std::move(data) };
    }
}

TEST(ValidateInvariants, ValidExampleTreeHasNoViolations)
{
    auto const result = traaxx_simd::validate_invariants(exampleTree());
    EXPECT_TRUE(result.valid);
    EXPECT_TRUE(result.violations.empty());
}

TEST(ValidateInvariants, DetectsOutOfBounds)
{
    auto tree = exampleTree();
    auto parent = tree.parent();
    parent[5] = 999;
    tree = traaxx_simd::Tree<std::uint32_t>{ parent, tree.sibling(), tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::bounds));
}

TEST(ValidateInvariants, DetectsParentOrderViolation)
{
    auto tree = exampleTree();
    auto parent = tree.parent();
    parent[1] = 5;
    tree = traaxx_simd::Tree<std::uint32_t>{ parent, tree.sibling(), tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::parent_order));
}

TEST(ValidateInvariants, DetectsSiblingOrderViolation)
{
    auto tree = exampleTree();
    auto sibling = tree.sibling();
    sibling[1] = 5;
    tree = traaxx_simd::Tree<std::uint32_t>{ tree.parent(), sibling, tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::sibling_order));
}

TEST(ValidateInvariants, DetectsZeroRoots)
{
    auto tree = exampleTree();
    auto parent = tree.parent();
    parent[0] = 1;
    tree = traaxx_simd::Tree<std::uint32_t>{ parent, tree.sibling(), tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::single_root));
}

TEST(ValidateInvariants, DetectsMultipleRoots)
{
    auto tree = exampleTree();
    auto parent = tree.parent();
    parent[4] = 4;
    tree = traaxx_simd::Tree<std::uint32_t>{ parent, tree.sibling(), tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::single_root));
}

TEST(ValidateInvariants, DetectsSiblingSameParentViolation)
{
    auto tree = exampleTree();
    auto sibling = tree.sibling();
    // node 9's real predecessor is 4 (self-loop, first child of 4); point it at
    // node 3 instead (parent[3]=1 != parent[9]=4), without breaking sibling order (3<=9).
    sibling[9] = 3;
    tree = traaxx_simd::Tree<std::uint32_t>{ tree.parent(), sibling, tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::sibling_same_parent));
}

TEST(ValidateInvariants, DetectsSiblingInjectiveViolation)
{
    auto tree = exampleTree();
    auto sibling = tree.sibling();
    // node 10 originally claims 8 as its predecessor; make it also claim 6,
    // which node 9 already claims — two children now claim the same predecessor.
    sibling[10] = 6;
    tree = traaxx_simd::Tree<std::uint32_t>{ tree.parent(), sibling, tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::sibling_injective));
}

TEST(ValidateInvariants, DetectsUnreachableComponent)
{
    // Node 0 is the sole root; nodes 3 and 4 form a 2-cycle disconnected from
    // it (parent[3]=4, parent[4]=3) — parent_order fails for node 3, but
    // bounds/single_root still hold, so stage 2 still runs.
    auto parent = std::vector<std::uint32_t>{ 0, 0, 1, 4, 3 };
    auto sibling = std::vector<std::uint32_t>{ 0, 1, 2, 3, 4 };
    auto data = std::vector<std::uint32_t>(parent.size());
    auto const tree = traaxx_simd::Tree<std::uint32_t>{ std::move(parent), std::move(sibling), std::move(data) };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::parent_order));
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::reachable_from_root));
    EXPECT_FALSE(has_violation(result, traaxx_simd::InvariantCheck::propagation_converges));
    for (auto const &violation : result.violations)
    {
        if (violation.check == traaxx_simd::InvariantCheck::reachable_from_root)
        {
            EXPECT_EQ(violation.nodes, (std::vector<std::uint32_t>{ 3, 4 }));
        }
    }
}

TEST(ValidateInvariants, BoundsFailureSuppressesStageTwo)
{
    auto tree = exampleTree();
    auto parent = tree.parent();
    auto sibling = tree.sibling();
    parent[5] = 999; // bounds violation
    sibling[9] = 3; // would also trip sibling_same_parent, if stage 2 ran
    tree = traaxx_simd::Tree<std::uint32_t>{ parent, sibling, tree.data() };
    auto const result = traaxx_simd::validate_invariants(tree);
    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(has_violation(result, traaxx_simd::InvariantCheck::bounds));
    EXPECT_FALSE(has_violation(result, traaxx_simd::InvariantCheck::sibling_same_parent));
    EXPECT_FALSE(has_violation(result, traaxx_simd::InvariantCheck::reachable_from_root));
    EXPECT_FALSE(has_violation(result, traaxx_simd::InvariantCheck::propagation_converges));
}
