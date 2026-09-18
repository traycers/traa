package std

import "testing"

func makeIndices(n int) []Index {
	indices := make([]Index, n)
	for i := range indices {
		indices[i] = Index(i)
	}
	return indices
}

func depthLikeStep(parent []Index) func(current []Index) []Index {
	return func(current []Index) []Index {
		next := make([]Index, len(current))
		parallelFor(len(next), func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if parent[i] == Index(i) {
					next[i] = 0
				} else {
					next[i] = current[parent[i]] + 1
				}
			}
		})
		return next
	}
}

func TestPropagateConvergesOnExampleTree(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	n := len(parent)
	state, iterations, err := propagate(make([]Index, n), depthLikeStep(parent), equalIndexSlice, Index(n))
	if err != nil {
		t.Fatalf("expected convergence, got error: %v", err)
	}
	expected := []Index{0, 1, 2, 2, 3, 3, 4, 4, 5, 4, 5, 5, 5}
	if !equalIndexSlice(state, expected) {
		t.Errorf("state = %v, want %v", state, expected)
	}
	if iterations != 6 {
		t.Errorf("iterations = %d, want 6", iterations)
	}
}

func TestPropagateRootNotAtZeroConverges(t *testing.T) {
	parent := []Index{1, 1, 1}
	n := len(parent)
	state, iterations, err := propagate(make([]Index, n), depthLikeStep(parent), equalIndexSlice, Index(n))
	if err != nil {
		t.Fatalf("expected convergence, got error: %v", err)
	}
	expected := []Index{1, 0, 1}
	if !equalIndexSlice(state, expected) {
		t.Errorf("state = %v, want %v", state, expected)
	}
	if iterations != 2 {
		t.Errorf("iterations = %d, want 2", iterations)
	}
}

func TestPropagateDoesNotConvergeReturnsConvergenceError(t *testing.T) {
	// parent[i] never equals i, no fixed point reachable within max_iterations
	parent := []Index{1, 0}
	n := len(parent)
	maxIterations := Index(n)
	_, _, err := propagate(make([]Index, n), depthLikeStep(parent), equalIndexSlice, maxIterations)
	if err == nil {
		t.Fatal("expected ConvergenceError, got nil")
	}
	convErr, ok := err.(ConvergenceError)
	if !ok {
		t.Fatalf("expected ConvergenceError, got %T", err)
	}
	if convErr.Iterations != maxIterations {
		t.Errorf("iterations = %d, want %d", convErr.Iterations, maxIterations)
	}
}

func TestPropagateAlreadyFixedPointConvergesInOneIteration(t *testing.T) {
	state := []Index{5, 5, 5}
	step := func(current []Index) []Index {
		next := make([]Index, len(current))
		copy(next, current)
		return next
	}
	result, iterations, err := propagate(state, step, equalIndexSlice, 10)
	if err != nil {
		t.Fatalf("expected convergence, got error: %v", err)
	}
	if !equalIndexSlice(result, state) {
		t.Errorf("state = %v, want %v", result, state)
	}
	if iterations != 1 {
		t.Errorf("iterations = %d, want 1", iterations)
	}
}

func TestPropagateGenericOverStateType(t *testing.T) {
	// StateT need not be []Index — any type supporting an equal predicate and
	// plain copy works.
	state := []float64{1.5, 2.5}
	equal := func(a, b []float64) bool {
		if len(a) != len(b) {
			return false
		}
		for i := range a {
			if a[i] != b[i] {
				return false
			}
		}
		return true
	}
	step := func(current []float64) []float64 {
		next := make([]float64, len(current))
		copy(next, current)
		return next
	}
	result, iterations, err := propagate(state, step, equal, 3)
	if err != nil {
		t.Fatalf("expected convergence, got error: %v", err)
	}
	if len(result) != 2 || result[0] != 1.5 || result[1] != 2.5 {
		t.Errorf("state = %v, want %v", result, state)
	}
	if iterations != 1 {
		t.Errorf("iterations = %d, want 1", iterations)
	}
}
