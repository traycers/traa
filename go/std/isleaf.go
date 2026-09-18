package std

// IsLeaf computes, for every node i, whether it has no children. It scatters
// "i is a parent" marks from every node's parent[i] (excluding self-loops —
// see docs/adr/0001-parallel-scatter-aggregation-uses-bitmask.md: a root's
// self-loop is not a real parent-child edge, so an unguarded scatter would
// misclassify an isolated childless root as non-leaf) into a BitMask via
// AtomicOr, then inverts it. Mirrors traaxx::isleaf
// (cxx/traaxx/sources/isleaf.cpp).
func IsLeaf(parent []Index) []bool {
	n := len(parent)
	if n == 0 {
		return []bool{}
	}
	isParent := NewBitMask(n)
	parallelFor(n, func(lo, hi int) {
		for i := lo; i < hi; i++ {
			p := parent[i]
			if p != Index(i) {
				isParent.AtomicOr(p)
			}
		}
	})
	isParent.Invert()
	result := make([]bool, n)
	for i := range result {
		result[i] = isParent.Get(Index(i))
	}
	return result
}
