package std

// LcaBatch answers m LCA queries in parallel (independent pairs), reusing
// the same scalar climb as LcaBinaryLifting with an early exit per level —
// not the SIMD-oriented branch-free "where"/select form from the source
// theory (§10.5), which targets a different target (this is the std,
// parallel-STL-equivalent track, not SIMD). Mirrors traaxx::lca_batch
// (cxx/traaxx/include/traaxx/lca_batch.hpp).
func LcaBatch[T any](tree *Tree[T], table AncestorTable, nodeDepth []Index, a, b []Index) []Index {
	parent := tree.Parent()
	result := make([]Index, len(a))
	parallelFor(len(a), func(lo, hi int) {
		for i := lo; i < hi; i++ {
			result[i] = lcaClimb(parent, nodeDepth, table, a[i], b[i])
		}
	})
	return result
}
