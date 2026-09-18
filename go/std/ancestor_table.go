package std

// AncestorTable holds, for each power-of-two distance 2^k, the vector of
// "ancestor at distance 2^k" per node — A1=parent, A2=A1[A1], A4=A2[A2], ...
// Used by pointer-doubling mask propagation and both binary-lifting LCA
// variants. Mirrors traaxx::AncestorTable
// (cxx/traaxx/include/traaxx/ancestor_table.hpp +
// cxx/traaxx/sources/ancestor_table.cpp). Level count (K+1) is derived from
// the tree's actual maximum depth (via LevelCountFor), not a conservative
// ceil(log2(n)) estimate.
type AncestorTable struct {
	levels [][]Index
}

// NewAncestorTable builds the table for the given parent vector and maximum
// depth dMax.
func NewAncestorTable(parent []Index, dMax Index) AncestorTable {
	n := len(parent)
	levelCount := int(LevelCountFor(dMax))
	levels := make([][]Index, 0, levelCount)
	first := make([]Index, n)
	copy(first, parent)
	levels = append(levels, first)
	for k := 1; k < levelCount; k++ {
		previous := levels[len(levels)-1]
		next := make([]Index, n)
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				next[i] = previous[previous[i]]
			}
		})
		levels = append(levels, next)
	}
	return AncestorTable{levels: levels}
}

// Level returns the raw "ancestor at distance 2^k" vector for level k. No
// convenience method for an arbitrary diff is provided — both pointer
// doubling and binary-lifting LCA decompose diff into bits of k in their own
// loop.
func (t AncestorTable) Level(k Index) []Index {
	return t.levels[k]
}

// LevelCount returns the number of levels (K+1) this table holds.
func (t AncestorTable) LevelCount() Index {
	return Index(len(t.levels))
}
