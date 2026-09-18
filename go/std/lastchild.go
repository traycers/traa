package std

// LastChild computes, for every node p, the id of its last child (the child
// with no right sibling pointing to it) via a BitMask scatter marking "has a
// right sibling", then a gather pass. Mirrors traaxx::lastchild
// (cxx/traaxx/sources/lastchild.cpp).
func LastChild(parent, sibling []Index) []Index {
	n := len(parent)
	hasRightSibling := NewBitMask(n)
	parallelFor(n, func(lo, hi int) {
		for k := lo; k < hi; k++ {
			s := sibling[k]
			if s != Index(k) {
				hasRightSibling.AtomicOr(s)
			}
		}
	})
	result := make([]Index, n)
	for i := range result {
		result[i] = Index(i)
	}
	parallelFor(n, func(lo, hi int) {
		for j := lo; j < hi; j++ {
			if parent[j] != Index(j) && !hasRightSibling.Get(Index(j)) {
				result[parent[j]] = Index(j)
			}
		}
	})
	return result
}
