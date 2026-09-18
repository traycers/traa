package std

// NSibling computes, for every node i, the id of its "next sibling" pointer
// target j such that sibling[j] == i (i.e. i's predecessor in the sibling
// chain, following traaxx's naming) — a direct O(n) scatter pass, not
// propagation. A node with no such j is its own sentinel. Mirrors
// traaxx::nsibling (cxx/traaxx/sources/nsibling.cpp).
func NSibling(sibling []Index) []Index {
	n := len(sibling)
	result := make([]Index, n)
	for i := range result {
		result[i] = Index(i)
	}
	parallelFor(n, func(lo, hi int) {
		for j := lo; j < hi; j++ {
			s := sibling[j]
			if s != Index(j) {
				result[s] = Index(j)
			}
		}
	})
	return result
}
