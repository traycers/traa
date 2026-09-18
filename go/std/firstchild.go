package std

// FirstChild computes, for every node p, the id of its first child (the
// child with no left sibling, i.e. sibling[j] == j) — a direct O(n) scatter
// pass. A childless node's entry is its own sentinel. Mirrors
// traaxx::firstchild (cxx/traaxx/sources/firstchild.cpp).
func FirstChild(parent, sibling []Index) []Index {
	n := len(parent)
	result := make([]Index, n)
	for i := range result {
		result[i] = Index(i)
	}
	parallelFor(n, func(lo, hi int) {
		for j := lo; j < hi; j++ {
			if sibling[j] == Index(j) && parent[j] != Index(j) {
				result[parent[j]] = Index(j)
			}
		}
	})
	return result
}
