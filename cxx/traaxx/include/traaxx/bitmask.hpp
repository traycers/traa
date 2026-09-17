#pragma once
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <execution>
#include <vector>

namespace traaxx
{
    template<typename IndexT>
    class BitMask
    {
    public:
        explicit BitMask(IndexT n) : bit_count_(n), words_(word_count(n), std::uint64_t{ 0 })
        {
        }

        bool get(IndexT i) const
        {
            auto const word = words_[word_index(i)];
            return (word & bit_mask(i)) != 0;
        }

        void set(IndexT i)
        {
            words_[word_index(i)] |= bit_mask(i);
        }

        void atomic_or(IndexT i)
        {
            auto ref = std::atomic_ref<std::uint64_t>(words_[word_index(i)]);
            ref.fetch_or(bit_mask(i));
        }

        IndexT size() const
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
            auto const remainder = static_cast<std::size_t>(bit_count_) % 64;
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
        static std::size_t word_count(IndexT n)
        {
            return (static_cast<std::size_t>(n) + 63) / 64;
        }

        static std::size_t word_index(IndexT i)
        {
            return static_cast<std::size_t>(i) / 64;
        }

        static std::uint64_t bit_mask(IndexT i)
        {
            return std::uint64_t{ 1 } << (static_cast<std::size_t>(i) % 64);
        }

        IndexT bit_count_;
        std::vector<std::uint64_t> words_;
    };
}
