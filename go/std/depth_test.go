package std

import "testing"

func TestDepthExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	result, err := Depth(parent, Index(len(parent)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expected := []Index{0, 1, 2, 2, 3, 3, 4, 4, 5, 4, 5, 5, 5}
	if !equalIndexSlice(result, expected) {
		t.Errorf("depth = %v, want %v", result, expected)
	}
}

func TestDepthRoot(t *testing.T) {
	parent := []Index{0}
	result, err := Depth(parent, Index(len(parent)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !equalIndexSlice(result, []Index{0}) {
		t.Errorf("depth = %v, want [0]", result)
	}
}

func TestDepthRootNotAtZero(t *testing.T) {
	parent := []Index{1, 1, 1}
	result, err := Depth(parent, Index(len(parent)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expected := []Index{1, 0, 1}
	if !equalIndexSlice(result, expected) {
		t.Errorf("depth = %v, want %v", result, expected)
	}
}

func TestDepthMultipleRoots(t *testing.T) {
	parent := []Index{0, 2, 2, 0}
	result, err := Depth(parent, Index(len(parent)))
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expected := []Index{0, 1, 0, 1}
	if !equalIndexSlice(result, expected) {
		t.Errorf("depth = %v, want %v", result, expected)
	}
}

func TestDepthEmptyArray(t *testing.T) {
	result, err := Depth([]Index{}, 0)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if len(result) != 0 {
		t.Errorf("expected empty result, got %v", result)
	}
}

func TestDepthNoRoot(t *testing.T) {
	// parent[i] never equals i, so propagation never converges within max_iterations
	parent := []Index{1, 0}
	maxIterations := Index(len(parent))
	_, err := Depth(parent, maxIterations)
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
