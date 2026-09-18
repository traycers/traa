package std

// dfsReorderTables computes the DFS-order permutation tables (§8.2, "way B":
// depth -> size -> pos via a segmented scan along the sibling chain for
// "sum of left siblings' sizes" -> old2new = pos, new2old = inverse(pos)).
// The segmented-scan step is one-off code here (not a shared primitive) —
// it has exactly one consumer in this package, mirroring
// cxx/traaxx/sources/dfs_reorder.cpp's dfs_reorder_tables.
func dfsReorderTables(parent []Index) (ReorderTables, error) {
	n := len(parent)
	if n == 0 {
		return ReorderTables{NewToOld: []Index{}, OldToNew: []Index{}}, nil
	}
	if _, err := Depth(parent, Index(n)); err != nil {
		return ReorderTables{}, err
	}
	nodeSize := Size(parent)
	pos := make([]Index, n)
	running := make([]Index, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		pos[i] = pos[p] + 1 + running[p]
		running[p] += nodeSize[i]
	}
	old2new := pos
	new2old := Inverse(old2new)
	return ReorderTables{NewToOld: new2old, OldToNew: old2new}, nil
}

// DfsReorderResult is the read-only, copy-returning form's result: a new
// reordered Tree plus the permutation tables that produced it. Mirrors
// traaxx::DfsReorderResult<T,IndexT>.
type DfsReorderResult[T any] struct {
	Tree     *Tree[T]
	NewToOld []Index
	OldToNew []Index
}

// DfsReorder computes the DFS reorder of tree without mutating it, returning
// a new Tree plus the permutation tables. Mirrors the free-function form
// traaxx::dfs_reorder(const Tree<T,IndexT>&).
func DfsReorder[T any](tree *Tree[T]) (DfsReorderResult[T], error) {
	tables, err := dfsReorderTables(tree.Parent())
	if err != nil {
		return DfsReorderResult[T]{}, err
	}
	reordered := NewTree(
		RemapIndexVector(tree.Parent(), tables.NewToOld, tables.OldToNew),
		RemapIndexVector(tree.Sibling(), tables.NewToOld, tables.OldToNew),
		Gather(tree.Data(), tables.NewToOld),
	)
	return DfsReorderResult[T]{Tree: reordered, NewToOld: tables.NewToOld, OldToNew: tables.OldToNew}, nil
}

// DfsReorder mutates the tree in place into DFS order, returning only the
// permutation tables (the tree itself, mutated, *is* the result). Mirrors
// the method form traaxx::Tree<T,IndexT>::dfs_reorder().
func (t *Tree[T]) DfsReorder() (ReorderTables, error) {
	tables, err := dfsReorderTables(t.parent)
	if err != nil {
		return ReorderTables{}, err
	}
	newParent := RemapIndexVector(t.parent, tables.NewToOld, tables.OldToNew)
	newSibling := RemapIndexVector(t.sibling, tables.NewToOld, tables.OldToNew)
	newData := Gather(t.data, tables.NewToOld)
	t.parent = newParent
	t.sibling = newSibling
	t.data = newData
	return tables, nil
}
