package std

import "testing"

func mutationsExampleTree() *Tree[Index] {
	// docs/learning/array-trees/01-example-tree.md
	parent := exampleParent()
	data := make([]Index, len(parent))
	for i := range data {
		data[i] = Index(i)
	}
	return NewTree(parent, exampleSibling(), data)
}

func TestMutationsAppendMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/10-mutations.md §11.2
	tree := mutationsExampleTree()
	x := tree.Append(9, 99)
	if x != 13 {
		t.Errorf("x = %d, want 13", x)
	}
	if tree.NodesCount() != 14 {
		t.Errorf("nodes count = %d, want 14", tree.NodesCount())
	}
	if tree.Parent()[13] != 9 {
		t.Errorf("parent[13] = %d, want 9", tree.Parent()[13])
	}
	if tree.Sibling()[13] != 11 {
		t.Errorf("sibling[13] = %d, want 11", tree.Sibling()[13])
	}
	if tree.Data()[13] != 99 {
		t.Errorf("data[13] = %d, want 99", tree.Data()[13])
	}
}

func TestMutationsAppendToChildlessParentSelfLoops(t *testing.T) {
	tree := mutationsExampleTree()
	x := tree.Append(8, 0)
	if tree.Sibling()[x] != x {
		t.Errorf("sibling[x] = %d, want %d", tree.Sibling()[x], x)
	}
}

func TestMutationsDeleteLeafMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/10-mutations.md §11.4
	tree := mutationsExampleTree()
	tree.DeleteLeaf(8)
	if tree.Sibling()[10] != 10 {
		t.Errorf("sibling[10] = %d, want 10", tree.Sibling()[10])
	}
	if tree.NodesCount() != 13 {
		t.Errorf("nodes count = %d, want 13", tree.NodesCount())
	}
}

func TestMutationsDeleteLeafOfLastChildIsNoOp(t *testing.T) {
	tree := mutationsExampleTree()
	before := append([]Index{}, tree.Sibling()...)
	tree.DeleteLeaf(11)
	if !equalIndexSlice(tree.Sibling(), before) {
		t.Errorf("sibling changed: %v, want %v", tree.Sibling(), before)
	}
}

func TestMutationsSpliceMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/10-mutations.md §11.6
	tree := mutationsExampleTree()
	tree.Splice(6)
	if tree.Parent()[8] != 4 {
		t.Errorf("parent[8] = %d, want 4", tree.Parent()[8])
	}
	if tree.Parent()[10] != 4 {
		t.Errorf("parent[10] = %d, want 4", tree.Parent()[10])
	}
	if tree.Sibling()[8] != 8 {
		t.Errorf("sibling[8] = %d, want 8", tree.Sibling()[8])
	}
	if tree.Sibling()[9] != 10 {
		t.Errorf("sibling[9] = %d, want 10", tree.Sibling()[9])
	}
}

func TestMutationsDeleteSubtreeMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/10-mutations.md §11.5
	tree := mutationsExampleTree()
	if err := tree.DeleteSubtree(3); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if tree.NodesCount() != 9 {
		t.Errorf("nodes count = %d, want 9", tree.NodesCount())
	}
	expectedParent := []Index{0, 0, 1, 2, 3, 4, 3, 4, 6}
	expectedSibling := []Index{0, 1, 2, 3, 4, 5, 4, 5, 8}
	if !equalIndexSlice(tree.Parent(), expectedParent) {
		t.Errorf("parent = %v, want %v", tree.Parent(), expectedParent)
	}
	if !equalIndexSlice(tree.Sibling(), expectedSibling) {
		t.Errorf("sibling = %v, want %v", tree.Sibling(), expectedSibling)
	}
}

func TestMutationsMoveMatchesWorkedExample(t *testing.T) {
	// docs/learning/array-trees/10-mutations.md §11.7
	tree := mutationsExampleTree()
	if err := tree.Move(4, 7); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if tree.Parent()[4] != 7 {
		t.Errorf("parent[4] = %d, want 7", tree.Parent()[4])
	}
	if tree.Sibling()[4] != 12 {
		t.Errorf("sibling[4] = %d, want 12", tree.Sibling()[4])
	}
}

func TestMutationsMoveUnderOwnDescendantIsRejectedAndTreeUnchanged(t *testing.T) {
	tree := mutationsExampleTree()
	parentBefore := append([]Index{}, tree.Parent()...)
	siblingBefore := append([]Index{}, tree.Sibling()...)
	err := tree.Move(1, 4)
	if err == nil {
		t.Fatal("expected an error")
	}
	moveErr, ok := err.(MoveError)
	if !ok {
		t.Fatalf("expected MoveError, got %T", err)
	}
	if moveErr.Kind != WouldCreateCycle {
		t.Errorf("kind = %v, want WouldCreateCycle", moveErr.Kind)
	}
	if !equalIndexSlice(tree.Parent(), parentBefore) {
		t.Errorf("parent changed: %v, want %v", tree.Parent(), parentBefore)
	}
	if !equalIndexSlice(tree.Sibling(), siblingBefore) {
		t.Errorf("sibling changed: %v, want %v", tree.Sibling(), siblingBefore)
	}
}

func TestMutationsMoveOntoSelfIsRejected(t *testing.T) {
	tree := mutationsExampleTree()
	err := tree.Move(4, 4)
	if err == nil {
		t.Fatal("expected an error")
	}
	moveErr, ok := err.(MoveError)
	if !ok {
		t.Fatalf("expected MoveError, got %T", err)
	}
	if moveErr.Kind != WouldCreateCycle {
		t.Errorf("kind = %v, want WouldCreateCycle", moveErr.Kind)
	}
}
