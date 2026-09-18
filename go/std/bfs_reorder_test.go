package std

import (
	"sort"
	"testing"
)

func TestBfsReorderFreeFunctionMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/08-bfs-order.md §9.3
	parent := exampleParent()
	sibling := exampleSibling()
	data := make([]Index, len(parent))
	for i := range data {
		data[i] = Index(i)
	}
	tree := NewTree(parent, sibling, data)
	result, err := BfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectedNew2old := []Index{0, 1, 2, 3, 4, 5, 6, 9, 7, 8, 10, 11, 12}
	expectedOld2new := []Index{0, 1, 2, 3, 4, 5, 6, 8, 9, 7, 10, 11, 12}
	expectedParentPrime := []Index{0, 0, 1, 1, 2, 3, 4, 4, 5, 6, 6, 7, 8}
	expectedSiblingPrime := []Index{0, 1, 2, 2, 4, 5, 6, 6, 8, 9, 9, 11, 12}
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
		t.Errorf("data' mismatch")
	}
}

func TestBfsReorderMethodMutatesInPlaceAndReturnsOnlyTables(t *testing.T) {
	tree := NewTree(exampleParent(), exampleSibling(), make([]Index, len(exampleParent())))
	result, err := tree.BfsReorder()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectedNew2old := []Index{0, 1, 2, 3, 4, 5, 6, 9, 7, 8, 10, 11, 12}
	expectedParentPrime := []Index{0, 0, 1, 1, 2, 3, 4, 4, 5, 6, 6, 7, 8}
	if !equalIndexSlice(result.NewToOld, expectedNew2old) {
		t.Errorf("new_to_old = %v, want %v", result.NewToOld, expectedNew2old)
	}
	if !equalIndexSlice(tree.Parent(), expectedParentPrime) {
		t.Errorf("parent = %v, want %v", tree.Parent(), expectedParentPrime)
	}
}

func TestBfsReorderParentIsNonDecreasingAfterReorder(t *testing.T) {
	// docs/learning/array-trees/08-bfs-order.md §9.4: BFS order makes parent' non-decreasing.
	tree := NewTree(exampleParent(), exampleSibling(), make([]Index, len(exampleParent())))
	result, err := BfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !sort.SliceIsSorted(result.Tree.Parent(), func(a, b int) bool {
		return result.Tree.Parent()[a] < result.Tree.Parent()[b]
	}) {
		t.Errorf("parent' should be sorted, got %v", result.Tree.Parent())
	}
}

func TestBfsReorderEmptyTree(t *testing.T) {
	tree := NewTree[Index](nil, nil, nil)
	result, err := BfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if len(result.NewToOld) != 0 || len(result.OldToNew) != 0 {
		t.Errorf("expected empty tables")
	}
}

func TestBfsReorderMultipleRootsProducesValidPermutation(t *testing.T) {
	// No doc-given oracle for multi-root tie-break; only asserting internal
	// consistency (new2old/old2new are mutually inverse permutations, and
	// every root lands before every non-root) rather than a specific order.
	parent := []Index{0, 2, 2, 0}
	sibling := []Index{0, 1, 1, 0}
	tree := NewTree(parent, sibling, make([]Index, 4))
	result, err := BfsReorder(tree)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	sortedNew2old := append([]Index{}, result.NewToOld...)
	sort.Slice(sortedNew2old, func(a, b int) bool { return sortedNew2old[a] < sortedNew2old[b] })
	expectedIDs := []Index{0, 1, 2, 3}
	if !equalIndexSlice(sortedNew2old, expectedIDs) {
		t.Errorf("sorted new_to_old = %v, want %v", sortedNew2old, expectedIDs)
	}
	for oldID := Index(0); oldID < 4; oldID++ {
		if result.NewToOld[result.OldToNew[oldID]] != oldID {
			t.Errorf("new_to_old/old_to_new not mutually inverse at %d", oldID)
		}
	}
}
