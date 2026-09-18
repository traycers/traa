package std

// LcaNaive finds the lowest common ancestor of a and b by walking both up to
// the root, level by level, computing depth internally (the only LCA
// variant that does — a single, propagate-based call site that can fail to
// converge). Mirrors traaxx::lca_naive
// (cxx/traaxx/include/traaxx/lca_naive.hpp).
func LcaNaive[T any](tree *Tree[T], a, b Index) (Index, error) {
	nodeDepth, err := Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		return 0, err
	}
	parent := tree.Parent()
	for nodeDepth[a] > nodeDepth[b] {
		a = parent[a]
	}
	for nodeDepth[b] > nodeDepth[a] {
		b = parent[b]
	}
	for a != b {
		a = parent[a]
		b = parent[b]
	}
	return a, nil
}
