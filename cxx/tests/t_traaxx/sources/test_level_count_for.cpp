#include <traaxx/level_count_for.hpp>
#include <gtest/gtest.h>
#include <cstdint>

TEST(LevelCountFor, SingleNodeDepthOne)
{
    EXPECT_EQ(traaxx::level_count_for(1u), 1u);
}

TEST(LevelCountFor, DepthZeroTreatedAsOne)
{
    EXPECT_EQ(traaxx::level_count_for(0u), 1u);
}

TEST(LevelCountFor, IsCeilLog2PlusOne)
{
    // ceil(log2(5)) = 3, plus one level of margin, per docs/learning/array-trees/04-mask-propagation.md §5.4
    EXPECT_EQ(traaxx::level_count_for(5u), 4u);
}

TEST(LevelCountFor, ExactPowerOfTwo)
{
    EXPECT_EQ(traaxx::level_count_for(4u), 3u);
    EXPECT_EQ(traaxx::level_count_for(8u), 4u);
}
