#include <traaxx_simd/bitmask.hpp>
#include <traaxx_simd/dispatch_bool.hpp>
#include <algorithm>
#include <atomic>
#include <numeric>
#include <xsimd/xsimd.hpp>

namespace traaxx_simd
{
    namespace
    {
        std::size_t word_count(std::size_t n)
        {
            return (n + 63) / 64;
        }

        std::size_t word_index(std::size_t i)
        {
            return i / 64;
        }

        std::uint64_t bit_mask(std::size_t i)
        {
            return std::uint64_t{ 1 } << (i % 64);
        }

        using batch_t = xsimd::batch<std::uint64_t>;
    }

    BitMask::BitMask(std::size_t n): bit_count_(n), words_(word_count(n), std::uint64_t{ 0 })
    {
    }

    bool BitMask::get(std::size_t i) const
    {
        auto const word = words_[word_index(i)];
        return (word & bit_mask(i)) != 0;
    }

    void BitMask::set(std::size_t i)
    {
        words_[word_index(i)] |= bit_mask(i);
    }

    void BitMask::atomic_or(std::size_t i)
    {
        auto ref = std::atomic_ref<std::uint64_t>(words_[word_index(i)]);
        ref.fetch_or(bit_mask(i));
    }

    std::size_t BitMask::size() const
    {
        return bit_count_;
    }

    bool BitMask::empty() const
    {
        return bit_count_ == 0U;
    }

    std::vector<std::size_t> BitMask::indices() const
    {
        auto result = std::vector<std::size_t>{};
        auto const doit = DispatchBool(
            [this](std::size_t v) { return get(v); },
            [&result](std::size_t v) { result.push_back(v); });
        auto positions = std::vector<std::size_t>(bit_count_);
        std::iota(positions.begin(), positions.end(), std::size_t{ 0 });
        std::for_each(positions.cbegin(), positions.cend(), doit);
        return result;
    }

    void BitMask::swap(BitMask &other)
    {
        words_.swap(other.words_);
        std::swap(bit_count_, other.bit_count_);
    }

    void BitMask::invert()
    {
        auto const n = words_.size();
        auto const width = batch_t::size;
        std::size_t i = 0;
        for (; i + width <= n; i += width)
        {
            auto const w = batch_t::load_unaligned(&words_[i]);
            auto const inverted = ~w;
            inverted.store_unaligned(&words_[i]);
        }
        for (; i < n; ++i)
        {
            words_[i] = ~words_[i];
        }
        auto const remainder = bit_count_ % 64;
        if (remainder != 0)
        {
            auto const valid_mask = (std::uint64_t{ 1 } << remainder) - 1;
            words_.back() &= valid_mask;
        }
    }

    BitMask::const_iterator BitMask::cbegin() const
    {
        return words_.cbegin();
    }

    BitMask::const_iterator BitMask::cend() const
    {
        return words_.cend();
    }

    bool BitMask::operator==(BitMask const &v) const
    {
        return bit_count_ == v.bit_count_ && words_ == v.words_;
    }

    bool BitMask::operator!=(BitMask const &v) const
    {
        return !(*this == v);
    }

    BitMask BitMask::operator&(BitMask const &v) const
    {
        if (bit_count_ != v.bit_count_)
        {
            return BitMask(0U);
        }
        auto result = *this;
        result &= v;
        return result;
    }

    BitMask BitMask::operator|(BitMask const &v) const
    {
        if (bit_count_ != v.bit_count_)
        {
            return BitMask(0U);
        }
        auto result = *this;
        result |= v;
        return result;
    }

    BitMask BitMask::operator~() const
    {
        auto result = *this;
        result.invert();
        return result;
    }

    BitMask &BitMask::operator&=(BitMask const &v)
    {
        if (bit_count_ == v.bit_count_)
        {
            auto const n = words_.size();
            auto const width = batch_t::size;
            std::size_t i = 0;
            for (; i + width <= n; i += width)
            {
                auto const a = batch_t::load_unaligned(&words_[i]);
                auto const b = batch_t::load_unaligned(&v.words_[i]);
                auto const combined = a & b;
                combined.store_unaligned(&words_[i]);
            }
            for (; i < n; ++i)
            {
                words_[i] &= v.words_[i];
            }
        }
        return *this;
    }

    BitMask &BitMask::operator|=(BitMask const &v)
    {
        if (bit_count_ == v.bit_count_)
        {
            auto const n = words_.size();
            auto const width = batch_t::size;
            std::size_t i = 0;
            for (; i + width <= n; i += width)
            {
                auto const a = batch_t::load_unaligned(&words_[i]);
                auto const b = batch_t::load_unaligned(&v.words_[i]);
                auto const combined = a | b;
                combined.store_unaligned(&words_[i]);
            }
            for (; i < n; ++i)
            {
                words_[i] |= v.words_[i];
            }
        }
        return *this;
    }
}
