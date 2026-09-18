package std

import "testing"

func TestSizeExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md;
	// expected size given directly in docs/learning/array-trees/07-dfs-order.md §8.1
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	result := Size(parent)
	expected := []Index{13, 12, 7, 4, 6, 3, 3, 2, 1, 2, 1, 1, 1}
	if !equalIndexSlice(result, expected) {
		t.Errorf("size = %v, want %v", result, expected)
	}
}

func TestSizeSingleNode(t *testing.T) {
	result := Size([]Index{0})
	if !equalIndexSlice(result, []Index{1}) {
		t.Errorf("size = %v, want [1]", result)
	}
}

func TestSizeEmptyArray(t *testing.T) {
	result := Size([]Index{})
	if len(result) != 0 {
		t.Errorf("expected empty result, got %v", result)
	}
}

func TestSizeLinearChain(t *testing.T) {
	// parent[i] <= i for all i, as Size requires (it does not go through
	// propagate, so it needs the topological-order invariant to hold).
	parent := []Index{0, 0, 1, 2}
	result := Size(parent)
	expected := []Index{4, 3, 2, 1}
	if !equalIndexSlice(result, expected) {
		t.Errorf("size = %v, want %v", result, expected)
	}
}
