package std

import "testing"

func TestTreeConstructsFromVectors(t *testing.T) {
	parent := []Index{0, 0, 1}
	sibling := []Index{0, 1, 1}
	data := []Index{10, 20, 30}
	tree := NewTree(parent, sibling, data)
	if !equalIndexSlice(tree.Parent(), parent) {
		t.Errorf("parent = %v, want %v", tree.Parent(), parent)
	}
	if !equalIndexSlice(tree.Sibling(), sibling) {
		t.Errorf("sibling = %v, want %v", tree.Sibling(), sibling)
	}
	if !equalIndexSlice(tree.Data(), data) {
		t.Errorf("data = %v, want %v", tree.Data(), data)
	}
}

func TestTreeNodesCountMatchesParentSize(t *testing.T) {
	tree := NewTree([]Index{0, 0, 1, 1}, []Index{0, 1, 1, 3}, []Index{1, 2, 3, 4})
	if tree.NodesCount() != 4 {
		t.Errorf("nodes count = %d, want 4", tree.NodesCount())
	}
}

func TestTreeDefaultConstructedIsEmpty(t *testing.T) {
	tree := NewTree[Index](nil, nil, nil)
	if tree.NodesCount() != 0 {
		t.Errorf("nodes count = %d, want 0", tree.NodesCount())
	}
	if len(tree.Parent()) != 0 || len(tree.Sibling()) != 0 || len(tree.Data()) != 0 {
		t.Errorf("expected empty tree")
	}
}

func TestTreeSupportsNonUint32Payload(t *testing.T) {
	tree := NewTree([]Index{0}, []Index{0}, []float64{3.5})
	if tree.Data()[0] != 3.5 {
		t.Errorf("data = %v, want [3.5]", tree.Data())
	}
}
