#pragma once
#include <cstdint>
#include <vector>

namespace traaxx
{
    class BitMask
    {
    public:
        using const_iterator = std::vector<std::uint64_t>::const_iterator;

    public:
        explicit BitMask(std::size_t n);

    public:
        bool get(std::size_t i) const;
        void set(std::size_t i);
        void atomic_or(std::size_t i);
        std::size_t size() const;
        bool empty() const;
        std::vector<std::size_t> indices() const;
        void swap(BitMask &other);
        void invert();
        const_iterator cbegin() const;
        const_iterator cend() const;

    public:
        bool operator==(BitMask const &v) const;
        bool operator!=(BitMask const &v) const;
        BitMask operator&(BitMask const &v) const;
        BitMask operator|(BitMask const &v) const;
        BitMask operator~() const;
        BitMask &operator&=(BitMask const &v);
        BitMask &operator|=(BitMask const &v);

    private:
        std::size_t bit_count_ = 0U;
        std::vector<std::uint64_t> words_;
    };
}
