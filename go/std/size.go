package std

// Size computes, for every node i, the size (node count) of the subtree
// rooted at i, via a single-pass backward topological scan — NOT the
// propagate primitive: size[parent[i]] += size[i] is not idempotent, so
// naively repeating it never converges on any branching tree (the same
// reasoning that rules out propagate for lca_set). Requires the topological
// order invariant parent[i] <= i. Mirrors traaxx::size
// (cxx/traaxx/sources/size.cpp).
func Size(parent []Index) []Index {
	n := len(parent)
	result := make([]Index, n)
	for i := range result {
		result[i] = 1
	}
	for i := n; i > 1; i-- {
		idx := i - 1
		result[parent[idx]] += result[idx]
	}
	return result
}
