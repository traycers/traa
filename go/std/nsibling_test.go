package std

import "testing"

func TestNSiblingExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md, cross-checked against
	// docs/learning/array-trees/10-mutations.md §11.1
	sibling := []Index{0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12}
	result := NSibling(sibling)
	expected := []Index{0, 1, 3, 3, 4, 5, 9, 7, 10, 9, 10, 11, 12}
	if !equalIndexSlice(result, expected) {
		t.Errorf("nsibling = %v, want %v", result, expected)
	}
}

func TestNSiblingSingleNodeHasNoRightSibling(t *testing.T) {
	result := NSibling([]Index{0})
	if !equalIndexSlice(result, []Index{0}) {
		t.Errorf("nsibling = %v, want [0]", result)
	}
}

func TestNSiblingEmptyArray(t *testing.T) {
	result := NSibling([]Index{})
	if len(result) != 0 {
		t.Errorf("expected empty result, got %v", result)
	}
}
