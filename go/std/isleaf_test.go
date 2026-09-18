package std

import "testing"

func TestIsLeafExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md, docs/learning/array-trees/03-derived-vectors.md §4
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	result := IsLeaf(parent)
	expected := []bool{
		false, false, false, false, false, false, false, false, true, false, true, true, true,
	}
	if len(result) != len(expected) {
		t.Fatalf("length = %d, want %d", len(result), len(expected))
	}
	for i := range expected {
		if result[i] != expected[i] {
			t.Errorf("index %d: got %v, want %v", i, result[i], expected[i])
		}
	}
}

func TestIsLeafSingleNodeRootIsLeaf(t *testing.T) {
	result := IsLeaf([]Index{0})
	if len(result) != 1 || !result[0] {
		t.Errorf("expected [true], got %v", result)
	}
}

func TestIsLeafEmptyArray(t *testing.T) {
	result := IsLeaf([]Index{})
	if len(result) != 0 {
		t.Errorf("expected empty result, got %v", result)
	}
}

func TestIsLeafIsolatedRootAmongOtherNodesIsLeaf(t *testing.T) {
	// node 0 is its own (childless) root; nodes 1/2 form a separate rooted
	// subtree under node 2 - node 0's self-loop must not count as "has a child"
	parent := []Index{0, 2, 2}
	result := IsLeaf(parent)
	expected := []bool{true, true, false}
	for i := range expected {
		if result[i] != expected[i] {
			t.Errorf("index %d: got %v, want %v", i, result[i], expected[i])
		}
	}
}
