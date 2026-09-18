package std

// propagate is the one shared iterative-propagation primitive, mirroring
// traaxx's template<...> propagate<...> (cxx/traaxx/include/traaxx/propagate.hpp):
// repeatedly apply step to the current state until it stops changing
// (compared via equal), or fail with ConvergenceError after maxIterations.
//
// Unlike the C++ template (which relies on std::equal + StateT::swap over a
// mutable current/next pair), this port takes step as current -> next
// (returning a fresh state) plus an explicit equal predicate — the cleanest
// shape that serves both instantiations used in this package ([]Index for
// depth/rank, BitMask for the two iterative mask-propagation strategies)
// without a shared interface constraint neither can satisfy uniformly.
//
// Iteration-count semantics are preserved exactly: the loop applies step,
// compares, and returns iteration+1 (not iteration) on convergence — the
// count includes the final, no-op-producing pass. See
// cxx/tests/t_traaxx/sources/test_propagate.cpp (six for the example tree,
// one for an already-fixed-point state).
func propagate[S any](initial S, step func(current S) S, equal func(a, b S) bool, maxIterations Index) (S, Index, error) {
	current := initial
	for iteration := Index(0); iteration < maxIterations; iteration++ {
		next := step(current)
		changed := !equal(current, next)
		current = next
		if !changed {
			return current, iteration + 1, nil
		}
	}
	var zero S
	return zero, 0, ConvergenceError{Iterations: maxIterations}
}

func equalIndexSlice(a, b []Index) bool {
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
