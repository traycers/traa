package std

import "testing"

// docs/learning/array-trees/01-example-tree.md
func exampleParent() []Index {
	return []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
}

func exampleSibling() []Index {
	return []Index{0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12}
}

func TestDfsReorderFreeFunctionMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/07-dfs-order.md §8.3
	parent := exampleParent()
	sibling := exampleSibling()
	data := make([]Index, len(parent))
	for i := range data {
		data[i] = Index(i)
	}
	tree := NewTree(parent, sibling, data)
	result, err := DfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectedNew2old := []Index{0, 1, 2, 4, 6, 8, 10, 9, 11, 3, 5, 7, 12}
	expectedOld2new := []Index{0, 1, 2, 9, 3, 10, 4, 11, 5, 7, 6, 8, 12}
	expectedParentPrime := []Index{0, 0, 1, 2, 3, 4, 4, 3, 7, 1, 9, 10, 11}
	expectedSiblingPrime := []Index{0, 1, 2, 3, 4, 5, 5, 4, 8, 2, 10, 11, 12}
	if !equalIndexSlice(result.NewToOld, expectedNew2old) {
		t.Errorf("new_to_old = %v, want %v", result.NewToOld, expectedNew2old)
	}
	if !equalIndexSlice(result.OldToNew, expectedOld2new) {
		t.Errorf("old_to_new = %v, want %v", result.OldToNew, expectedOld2new)
	}
	if !equalIndexSlice(result.Tree.Parent(), expectedParentPrime) {
		t.Errorf("parent' = %v, want %v", result.Tree.Parent(), expectedParentPrime)
	}
	if !equalIndexSlice(result.Tree.Sibling(), expectedSiblingPrime) {
		t.Errorf("sibling' = %v, want %v", result.Tree.Sibling(), expectedSiblingPrime)
	}
	if !equalIndexSlice(result.Tree.Data(), Gather(data, expectedNew2old)) {
		t.Errorf("data' = %v, want %v", result.Tree.Data(), Gather(data, expectedNew2old))
	}
}

func TestDfsReorderMethodMutatesInPlaceAndReturnsOnlyTables(t *testing.T) {
	tree := NewTree(exampleParent(), exampleSibling(), make([]Index, len(exampleParent())))
	result, err := tree.DfsReorder()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectedNew2old := []Index{0, 1, 2, 4, 6, 8, 10, 9, 11, 3, 5, 7, 12}
	expectedParentPrime := []Index{0, 0, 1, 2, 3, 4, 4, 3, 7, 1, 9, 10, 11}
	if !equalIndexSlice(result.NewToOld, expectedNew2old) {
		t.Errorf("new_to_old = %v, want %v", result.NewToOld, expectedNew2old)
	}
	if !equalIndexSlice(tree.Parent(), expectedParentPrime) {
		t.Errorf("parent = %v, want %v", tree.Parent(), expectedParentPrime)
	}
}

func TestDfsReorderSingleRootIsIdentity(t *testing.T) {
	tree := NewTree([]Index{0}, []Index{0}, []Index{42})
	result, err := DfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !equalIndexSlice(result.NewToOld, []Index{0}) {
		t.Errorf("new_to_old = %v, want [0]", result.NewToOld)
	}
	if !equalIndexSlice(result.OldToNew, []Index{0}) {
		t.Errorf("old_to_new = %v, want [0]", result.OldToNew)
	}
}

func TestDfsReorderEmptyTreeProducesEmptyTables(t *testing.T) {
	tree := NewTree[Index](nil, nil, nil)
	result, err := DfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if len(result.NewToOld) != 0 || len(result.OldToNew) != 0 {
		t.Errorf("expected empty tables")
	}
}

func TestDfsReorderDoesNotConvergeReturnsConvergenceError(t *testing.T) {
	// parent[i] never equals i, depth() never converges within n iterations.
	tree := NewTree([]Index{1, 0}, []Index{0, 1}, []Index{0, 1})
	_, err := DfsReorder(tree)
	if err == nil {
		t.Fatal("expected an error")
	}
}
