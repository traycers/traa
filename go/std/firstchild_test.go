package std

import "testing"

func TestFirstChildExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	sibling := []Index{0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12}
	result := FirstChild(parent, sibling)
	// leaves (8, 10, 11, 12) have no children — sentinel is self-reference.
	expected := []Index{1, 2, 4, 5, 6, 7, 8, 12, 8, 11, 10, 11, 12}
	if !equalIndexSlice(result, expected) {
		t.Errorf("firstchild = %v, want %v", result, expected)
	}
}

func TestFirstChildSingleNodeHasNoChildren(t *testing.T) {
	result := FirstChild([]Index{0}, []Index{0})
	if !equalIndexSlice(result, []Index{0}) {
		t.Errorf("firstchild = %v, want [0]", result)
	}
}
