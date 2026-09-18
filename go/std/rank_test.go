package std

import "testing"

func TestRankExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md, sibling vector;
	// expected rank computed by hand from
	// docs/learning/array-trees/02-path-coordinate-matrix.md §3.6.
	sibling := []Index{0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12}
	result, err := Rank(sibling, Index(len(sibling)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expected := []Index{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0}
	if !equalIndexSlice(result, expected) {
		t.Errorf("rank = %v, want %v", result, expected)
	}
}

func TestRankAllFirstChildren(t *testing.T) {
	sibling := []Index{0, 1, 2}
	result, err := Rank(sibling, Index(len(sibling)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expected := []Index{0, 0, 0}
	if !equalIndexSlice(result, expected) {
		t.Errorf("rank = %v, want %v", result, expected)
	}
}

func TestRankEmptyArray(t *testing.T) {
	result, err := Rank([]Index{}, 0)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if len(result) != 0 {
		t.Errorf("expected empty result, got %v", result)
	}
}

func TestRankDoesNotConvergeReturnsConvergenceError(t *testing.T) {
	sibling := []Index{1, 0}
	maxIterations := Index(len(sibling))
	_, err := Rank(sibling, maxIterations)
	if err == nil {
		t.Fatal("expected ConvergenceError")
	}
	convErr, ok := err.(ConvergenceError)
	if !ok {
		t.Fatalf("expected ConvergenceError, got %T", err)
	}
	if convErr.Iterations != maxIterations {
		t.Errorf("iterations = %d, want %d", convErr.Iterations, maxIterations)
	}
}
