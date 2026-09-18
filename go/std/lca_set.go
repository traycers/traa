package std

// LcaSet finds the lowest common ancestor of an entire set s of nodes via a
// single backward topological scan (cnt[parent[i]] += cnt[i], NOT the
// propagate primitive — that recurrence is not idempotent, so naive repeat
// never converges on a branching tree, same reasoning as size()). Every
// ancestor of the true LCA (inclusive, up to the root) ends up with
// cnt[i]==len(s); that set is a single root-to-LCA chain, so the LCA is the
// unique qualifying node with no qualifying child — this avoids a separate
// depth() call/failure mode. Precondition: s is non-empty and every element
// is a valid node id. Mirrors traaxx::lca_set
// (cxx/traaxx/include/traaxx/lca_set.hpp).
func LcaSet[T any](tree *Tree[T], s []Index) Index {
	parent := tree.Parent()
	n := len(parent)
	cnt := make([]Index, n)
	for _, x := range s {
		cnt[x]++
	}
	for i := n; i > 1; i-- {
		idx := i - 1
		cnt[parent[idx]] += cnt[idx]
	}
	target := Index(len(s))
	hasQualifyingChild := make([]bool, n)
	for i := 0; i < n; i++ {
		if cnt[i] == target && parent[i] != Index(i) {
			hasQualifyingChild[parent[i]] = true
		}
	}
	for i := 0; i < n; i++ {
		if cnt[i] == target && !hasQualifyingChild[i] {
			return Index(i)
		}
	}
	return parent[0]
}
