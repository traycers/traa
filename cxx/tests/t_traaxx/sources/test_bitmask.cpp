#include <traaxx/bitmask.hpp>
#include <gtest/gtest.h>
#include <cstdint>
#include <thread>
#include <vector>

TEST(BitMask, ConstructedZeroInitialized)
{
    auto const mask = traaxx::BitMask<std::uint32_t>(70);
    for (std::uint32_t i = 0; i < mask.size(); ++i)
    {
        EXPECT_FALSE(mask.get(i));
    }
}

TEST(BitMask, SizeReportsBitCount)
{
    auto const mask = traaxx::BitMask<std::uint32_t>(130);
    EXPECT_EQ(mask.size(), 130u);
}

TEST(BitMask, SetRoundTrip)
{
    auto mask = traaxx::BitMask<std::uint32_t>(70);
    mask.set(0);
    mask.set(63);
    mask.set(64);
    mask.set(69);
    EXPECT_TRUE(mask.get(0));
    EXPECT_TRUE(mask.get(63));
    EXPECT_TRUE(mask.get(64));
    EXPECT_TRUE(mask.get(69));
    EXPECT_FALSE(mask.get(1));
    EXPECT_FALSE(mask.get(62));
    EXPECT_FALSE(mask.get(65));
    EXPECT_FALSE(mask.get(68));
}

TEST(BitMask, AtomicOrRoundTrip)
{
    auto mask = traaxx::BitMask<std::uint32_t>(10);
    mask.atomic_or(2);
    mask.atomic_or(5);
    EXPECT_TRUE(mask.get(2));
    EXPECT_TRUE(mask.get(5));
    EXPECT_FALSE(mask.get(0));
    EXPECT_FALSE(mask.get(9));
}

TEST(BitMask, AtomicOrDoesNotClobberNeighborBitsInSameWord)
{
    auto mask = traaxx::BitMask<std::uint32_t>(64);
    mask.atomic_or(3);
    mask.atomic_or(40);
    for (std::uint32_t i = 0; i < mask.size(); ++i)
    {
        auto const expected = i == 3 || i == 40;
        EXPECT_EQ(mask.get(i), expected) << "bit " << i;
    }
}

TEST(BitMask, EqualityComparesContent)
{
    auto a = traaxx::BitMask<std::uint32_t>(20);
    auto b = traaxx::BitMask<std::uint32_t>(20);
    EXPECT_EQ(a, b);
    a.set(5);
    EXPECT_NE(a, b);
    b.set(5);
    EXPECT_EQ(a, b);
}

TEST(BitMask, SwapExchangesContent)
{
    auto a = traaxx::BitMask<std::uint32_t>(20);
    auto b = traaxx::BitMask<std::uint32_t>(20);
    a.set(3);
    b.set(17);
    a.swap(b);
    EXPECT_TRUE(a.get(17));
    EXPECT_FALSE(a.get(3));
    EXPECT_TRUE(b.get(3));
    EXPECT_FALSE(b.get(17));
}

TEST(BitMask, InvertFlipsAllValidBits)
{
    auto mask = traaxx::BitMask<std::uint32_t>(70);
    mask.set(0);
    mask.set(64);
    mask.invert();
    for (std::uint32_t i = 0; i < mask.size(); ++i)
    {
        auto const expected = i != 0 && i != 64;
        EXPECT_EQ(mask.get(i), expected) << "bit " << i;
    }
}

TEST(BitMask, InvertTwiceIsIdentity)
{
    auto mask = traaxx::BitMask<std::uint32_t>(70);
    mask.set(3);
    mask.set(69);
    auto const original = mask;
    mask.invert();
    mask.invert();
    EXPECT_EQ(mask, original);
}

TEST(BitMask, InvertClearsPaddingBitsInLastWord)
{
    // size is not a multiple of 64 - inverting must not leave stray 1-bits
    // past bit_count in the last word, or two masks built the same way would
    // stop comparing equal after each is inverted.
    auto a = traaxx::BitMask<std::uint32_t>(70);
    auto b = traaxx::BitMask<std::uint32_t>(70);
    a.invert();
    b.invert();
    EXPECT_EQ(a, b);
}

TEST(BitMask, ConcurrentAtomicOrOnSameWordIsRaceFree)
{
    auto mask = traaxx::BitMask<std::uint32_t>(64);
    auto threads = std::vector<std::thread>{};
    for (std::uint32_t i = 0; i < 64; ++i)
    {
        threads.emplace_back([&mask, i]() { mask.atomic_or(i); });
    }
    for (auto &t : threads)
    {
        t.join();
    }
    for (std::uint32_t i = 0; i < mask.size(); ++i)
    {
        EXPECT_TRUE(mask.get(i)) << "bit " << i;
    }
}
