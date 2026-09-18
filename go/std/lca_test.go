package std

import "testing"

func lcaExampleTree() *Tree[Index] {
	// docs/learning/array-trees/01-example-tree.md
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	sibling := []Index{0, 1, 2, 2, 4, 5, 6, 6, 8, 9, 9, 11, 12}
	data := make([]Index, len(parent))
	return NewTree(parent, sibling, data)
}

func TestLcaNaiveMatchesWorkedTraces(t *testing.T) {
	tree := lcaExampleTree()
	cases := []struct{ a, b, want Index }{
		{8, 11, 4},
		{8, 10, 6},
		{10, 12, 1},
		{2, 8, 2},
	}
	for _, c := range cases {
		got, err := LcaNaive(tree, c.a, c.b)
		if err != nil {
			t.Fatalf("unexpected error: %v", err)
		}
		if got != c.want {
			t.Errorf("LcaNaive(%d, %d) = %d, want %d", c.a, c.b, got, c.want)
		}
	}
}

func TestLcaBinaryLiftingAgreesWithNaive(t *testing.T) {
	tree := lcaExampleTree()
	nodeDepth, err := Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	dMax := Index(0)
	for _, d := range nodeDepth {
		if d > dMax {
			dMax = d
		}
	}
	table := NewAncestorTable(tree.Parent(), dMax)
	cases := []struct{ a, b, want Index }{
		{8, 11, 4},
		{8, 10, 6},
		{10, 12, 1},
		{2, 8, 2},
	}
	for _, c := range cases {
		got := LcaBinaryLifting(tree, table, nodeDepth, c.a, c.b)
		if got != c.want {
			t.Errorf("LcaBinaryLifting(%d, %d) = %d, want %d", c.a, c.b, got, c.want)
		}
	}
}

func TestLcaBatchAgreesWithBinaryLifting(t *testing.T) {
	tree := lcaExampleTree()
	nodeDepth, err := Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	dMax := Index(0)
	for _, d := range nodeDepth {
		if d > dMax {
			dMax = d
		}
	}
	table := NewAncestorTable(tree.Parent(), dMax)
	a := []Index{8, 8, 10, 2}
	b := []Index{11, 10, 12, 8}
	result := LcaBatch(tree, table, nodeDepth, a, b)
	expected := []Index{4, 6, 1, 2}
	if !equalIndexSlice(result, expected) {
		t.Errorf("LcaBatch = %v, want %v", result, expected)
	}
}

func TestLcaSetOfThreeAcrossTwoGroups(t *testing.T) {
	tree := lcaExampleTree()
	s := []Index{8, 11, 12}
	if got := LcaSet(tree, s); got != 1 {
		t.Errorf("LcaSet(%v) = %d, want 1", s, got)
	}
}

func TestLcaSetOfTwoWithinOneGroup(t *testing.T) {
	tree := lcaExampleTree()
	s := []Index{8, 11}
	if got := LcaSet(tree, s); got != 4 {
		t.Errorf("LcaSet(%v) = %d, want 4", s, got)
	}
}

func TestLcaSetSingletonReturnsItself(t *testing.T) {
	tree := lcaExampleTree()
	s := []Index{8}
	if got := LcaSet(tree, s); got != 8 {
		t.Errorf("LcaSet(%v) = %d, want 8", s, got)
	}
}

func TestLcaAncestorPairReturnsAncestor(t *testing.T) {
	tree := lcaExampleTree()
	got, err := LcaNaive(tree, 2, 2)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if got != 2 {
		t.Errorf("LcaNaive(2, 2) = %d, want 2", got)
	}
}
