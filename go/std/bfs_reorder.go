package std

import "sort"

// bfsReorderTables computes the BFS-order permutation tables independently
// of DFS (way B, §9.2): level 0 = roots; for each level L=1..d_max, collect
// nodes at that depth, stable-sort by (new position of parent, rank), and
// assign consecutive positions. Mirrors
// cxx/traaxx/sources/bfs_reorder.cpp's bfs_reorder_tables.
func bfsReorderTables(parent, sibling []Index) (ReorderTables, error) {
	n := len(parent)
	if n == 0 {
		return ReorderTables{NewToOld: []Index{}, OldToNew: []Index{}}, nil
	}
	nodeDepth, err := Depth(parent, Index(n))
	if err != nil {
		return ReorderTables{}, err
	}
	nodeRank, err := Rank(sibling, Index(n))
	if err != nil {
		return ReorderTables{}, err
	}
	dMax := Index(0)
	for _, d := range nodeDepth {
		if d > dMax {
			dMax = d
		}
	}
	new2old := make([]Index, n)
	old2new := make([]Index, n)
	position := Index(0)
	for level := Index(0); level <= dMax; level++ {
		levelNodes := []Index{}
		for i := 0; i < n; i++ {
			if nodeDepth[i] == level {
				levelNodes = append(levelNodes, Index(i))
			}
		}
		sort.SliceStable(levelNodes, func(a, b int) bool {
			nodeA, nodeB := levelNodes[a], levelNodes[b]
			var keyA, keyB Index
			if level != 0 {
				keyA = old2new[parent[nodeA]]
				keyB = old2new[parent[nodeB]]
			}
			if keyA != keyB {
				return keyA < keyB
			}
			return nodeRank[nodeA] < nodeRank[nodeB]
		})
		for _, oldID := range levelNodes {
			new2old[position] = oldID
			old2new[oldID] = position
			position++
		}
	}
	return ReorderTables{NewToOld: new2old, OldToNew: old2new}, nil
}

// BfsReorderResult is the read-only, copy-returning form's result. Mirrors
// traaxx::BfsReorderResult<T,IndexT>.
type BfsReorderResult[T any] struct {
	Tree     *Tree[T]
	NewToOld []Index
	OldToNew []Index
}

// BfsReorder computes the BFS reorder of tree without mutating it. Mirrors
// the free-function form traaxx::bfs_reorder(const Tree<T,IndexT>&).
func BfsReorder[T any](tree *Tree[T]) (BfsReorderResult[T], error) {
	tables, err := bfsReorderTables(tree.Parent(), tree.Sibling())
	if err != nil {
		return BfsReorderResult[T]{}, err
	}
	reordered := NewTree(
		RemapIndexVector(tree.Parent(), tables.NewToOld, tables.OldToNew),
		RemapIndexVector(tree.Sibling(), tables.NewToOld, tables.OldToNew),
		Gather(tree.Data(), tables.NewToOld),
	)
	return BfsReorderResult[T]{Tree: reordered, NewToOld: tables.NewToOld, OldToNew: tables.OldToNew}, nil
}

// BfsReorder mutates the tree in place into BFS order, returning only the
// permutation tables. Mirrors the method form
// traaxx::Tree<T,IndexT>::bfs_reorder().
func (t *Tree[T]) BfsReorder() (ReorderTables, error) {
	tables, err := bfsReorderTables(t.parent, t.sibling)
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
