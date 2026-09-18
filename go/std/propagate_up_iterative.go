package std

// PropagateUpIterative propagates a mask upward from seed (scatter with or
// along parent) until it reaches a fixed point, via the shared propagate
// primitive. Mirrors traaxx::propagate_up_iterative
// (cxx/traaxx/sources/propagate_up_iterative.cpp).
func PropagateUpIterative(parent []Index, seed BitMask, maxIterations Index) (BitMask, Index, error) {
	n := len(parent)
	if n == 0 {
		return seed, 0, nil
	}
	step := func(current BitMask) BitMask {
		next := current.Clone()
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if current.Get(Index(i)) {
					next.AtomicOr(parent[i])
				}
			}
		})
		return next
	}
	return propagate(seed, step, BitMask.Equal, maxIterations)
}
