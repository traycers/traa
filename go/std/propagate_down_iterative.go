package std

// PropagateDownIterative propagates a subtree mask downward from seed
// (gather with or along parent) until it reaches a fixed point, via the
// shared propagate primitive. Mirrors traaxx::propagate_down_iterative
// (cxx/traaxx/sources/propagate_down_iterative.cpp). Returns the number of
// iterations taken to converge — the CLI reports it for this op.
func PropagateDownIterative(parent []Index, seed BitMask, maxIterations Index) (BitMask, Index, error) {
	n := len(parent)
	if n == 0 {
		return seed, 0, nil
	}
	step := func(current BitMask) BitMask {
		next := NewBitMask(n)
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if current.Get(Index(i)) || current.Get(parent[i]) {
					next.AtomicOr(Index(i))
				}
			}
		})
		return next
	}
	return propagate(seed, step, BitMask.Equal, maxIterations)
}
