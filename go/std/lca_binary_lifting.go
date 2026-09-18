package std

// lcaClimb is the shared binary-lifting climb used by both LcaBinaryLifting
// and LcaBatch, mirroring traaxx::detail::lca_climb
// (cxx/traaxx/include/traaxx/lca_binary_lifting.hpp).
func lcaClimb(parent, nodeDepth []Index, table AncestorTable, a, b Index) Index {
	if nodeDepth[a] < nodeDepth[b] {
		a, b = b, a
	}
	diff := nodeDepth[a] - nodeDepth[b]
	for k := Index(0); k < table.LevelCount(); k++ {
		if (diff>>k)&1 != 0 {
			a = table.Level(k)[a]
		}
	}
	if a == b {
		return a
	}
	for k := table.LevelCount(); k > 0; k-- {
		level := k - 1
		if table.Level(level)[a] != table.Level(level)[b] {
			a = table.Level(level)[a]
			b = table.Level(level)[b]
		}
	}
	return parent[a]
}

// LcaBinaryLifting finds the LCA of a single pair (a, b) using a
// pre-built AncestorTable and pre-computed depth vector — the caller builds
// and owns both, so repeated queries amortize the O(n log d) table-build
// cost. Mirrors traaxx::lca_binary_lifting
// (cxx/traaxx/include/traaxx/lca_binary_lifting.hpp).
func LcaBinaryLifting[T any](tree *Tree[T], table AncestorTable, nodeDepth []Index, a, b Index) Index {
	return lcaClimb(tree.Parent(), nodeDepth, table, a, b)
}
