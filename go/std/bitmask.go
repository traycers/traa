package std

import "sync/atomic"

// BitMask is a packed bitset on uint64 words, mirroring traaxx::BitMask
// (cxx/traaxx/include/traaxx/bitmask.hpp + sources/bitmask.cpp): one bit per
// node, not a byte per node and not a []bool, per
// docs/adr/0001-parallel-scatter-aggregation-uses-bitmask.md — the packed
// word representation is what makes the scatter-OR in mask propagation
// race-free at the word level via AtomicOr.
type BitMask struct {
	bitCount int
	words    []uint64
}

func wordCount(n int) int {
	return (n + 63) / 64
}

func wordIndex(i int) int {
	return i / 64
}

func bitFor(i int) uint64 {
	return uint64(1) << (uint(i) % 64)
}

// NewBitMask constructs a zero-initialized BitMask of n bits.
func NewBitMask(n int) BitMask {
	return BitMask{bitCount: n, words: make([]uint64, wordCount(n))}
}

// Get reports whether bit i is set.
func (m BitMask) Get(i Index) bool {
	word := m.words[wordIndex(int(i))]
	return word&bitFor(int(i)) != 0
}

// Set sets bit i (non-atomic — for sequential/single-writer use).
func (m BitMask) Set(i Index) {
	m.words[wordIndex(int(i))] |= bitFor(int(i))
}

// AtomicOr atomically ORs bit i into the mask's word, for race-free
// concurrent scatter-writes to different bits (ADR 0001).
func (m BitMask) AtomicOr(i Index) {
	atomic.OrUint64(&m.words[wordIndex(int(i))], bitFor(int(i)))
}

// Size returns the number of bits this mask was constructed with.
func (m BitMask) Size() Index {
	return Index(m.bitCount)
}

// Empty reports whether this mask has zero bits.
func (m BitMask) Empty() bool {
	return m.bitCount == 0
}

// Indices returns, in ascending order, the positions of every set bit.
func (m BitMask) Indices() []Index {
	result := []Index{}
	for i := 0; i < m.bitCount; i++ {
		if m.Get(Index(i)) {
			result = append(result, Index(i))
		}
	}
	return result
}

// Clone returns an independent copy of m.
func (m BitMask) Clone() BitMask {
	words := make([]uint64, len(m.words))
	copy(words, m.words)
	return BitMask{bitCount: m.bitCount, words: words}
}

// Invert flips every valid bit in place (bits at/after bitCount in the final
// word are left at zero — the "padding" bits are never valid content).
func (m BitMask) Invert() {
	parallelFor(len(m.words), func(lo, hi int) {
		for w := lo; w < hi; w++ {
			m.words[w] = ^m.words[w]
		}
	})
	remainder := m.bitCount % 64
	if len(m.words) > 0 && remainder != 0 {
		validMask := (uint64(1) << uint(remainder)) - 1
		m.words[len(m.words)-1] &= validMask
	}
}

// Equal reports whether two masks have the same size and content.
func (m BitMask) Equal(other BitMask) bool {
	if m.bitCount != other.bitCount {
		return false
	}
	for i := range m.words {
		if m.words[i] != other.words[i] {
			return false
		}
	}
	return true
}

// And returns the intersection of m and other, or an empty (size-0) mask if
// their sizes differ (mirroring traaxx::BitMask::operator&).
func (m BitMask) And(other BitMask) BitMask {
	if m.bitCount != other.bitCount {
		return NewBitMask(0)
	}
	result := m.Clone()
	result.AndAssign(other)
	return result
}

// Or returns the union of m and other, or an empty (size-0) mask if their
// sizes differ (mirroring traaxx::BitMask::operator|).
func (m BitMask) Or(other BitMask) BitMask {
	if m.bitCount != other.bitCount {
		return NewBitMask(0)
	}
	result := m.Clone()
	result.OrAssign(other)
	return result
}

// Not returns an inverted copy of m, leaving m unchanged.
func (m BitMask) Not() BitMask {
	result := m.Clone()
	result.Invert()
	return result
}

// AndAssign ANDs other into m in place; a no-op if sizes differ.
func (m BitMask) AndAssign(other BitMask) {
	if m.bitCount != other.bitCount {
		return
	}
	parallelFor(len(m.words), func(lo, hi int) {
		for w := lo; w < hi; w++ {
			m.words[w] &= other.words[w]
		}
	})
}

// OrAssign ORs other into m in place; a no-op if sizes differ.
func (m BitMask) OrAssign(other BitMask) {
	if m.bitCount != other.bitCount {
		return
	}
	parallelFor(len(m.words), func(lo, hi int) {
		for w := lo; w < hi; w++ {
			m.words[w] |= other.words[w]
		}
	})
}
