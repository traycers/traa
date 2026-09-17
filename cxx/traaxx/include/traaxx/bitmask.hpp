#pragma once
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <execution>
#include <vector>

namespace traaxx
{
    class BitMask
    {
    public:
        explicit BitMask(std::size_t n) : bit_count_(n), words_(word_count(n), std::uint64_t{ 0 })
        {
        }

        bool get(std::size_t i) const
        {
            auto const word = words_[word_index(i)];
            return (word & bit_mask(i)) != 0;
        }

        void set(std::size_t i)
        {
            words_[word_index(i)] |= bit_mask(i);
        }

        void atomic_or(std::size_t i)
        {
            auto ref = std::atomic_ref<std::uint64_t>(words_[word_index(i)]);
            ref.fetch_or(bit_mask(i));
        }

        std::size_t size() const
        {
            return bit_count_;
        }

        void swap(BitMask &other)
        {
            words_.swap(other.words_);
            std::swap(bit_count_, other.bit_count_);
        }

        void invert()
        {
            std::for_each(
                std::execution::par,   //
                words_.begin(),        //
                words_.end(),          //
                [](std::uint64_t &word) { word = ~word; });
            auto const remainder = bit_count_ % 64;
            if (remainder != 0)
            {
                auto const valid_mask = (std::uint64_t{ 1 } << remainder) - 1;
                words_.back() &= valid_mask;
            }
        }

        auto cbegin() const
        {
            return words_.cbegin();
        }

        auto cend() const
        {
            return words_.cend();
        }

        friend bool operator==(BitMask const &lhs, BitMask const &rhs)
        {
            return lhs.bit_count_ == rhs.bit_count_ && lhs.words_ == rhs.words_;
        }

        friend bool operator!=(BitMask const &lhs, BitMask const &rhs)
        {
            return !(lhs == rhs);
        }

    private:
        static std::size_t word_count(std::size_t n)
        {
            return (n + 63) / 64;
        }

        static std::size_t word_index(std::size_t i)
        {
            return i / 64;
        }

        static std::uint64_t bit_mask(std::size_t i)
        {
            return std::uint64_t{ 1 } << (i % 64);
        }

        std::size_t bit_count_;
        std::vector<std::uint64_t> words_;
    };
}
