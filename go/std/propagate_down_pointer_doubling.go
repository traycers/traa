package std

// PropagateDownPointerDoubling propagates a mask downward in O(log d) steps
// via successively doubled ancestor pointers (A1, A2, A4, ...), rather than
// iterating to a detected fixed point: the number of steps (level_count) is
// fixed up front from dMax. Mirrors traaxx::propagate_down_pointer_doubling
// (cxx/traaxx/sources/propagate_down_pointer_doubling.cpp).
func PropagateDownPointerDoubling(parent []Index, seed BitMask, dMax Index) BitMask {
	n := len(parent)
	if n == 0 {
		return seed
	}
	levelCount := int(LevelCountFor(dMax))
	current := seed
	anc := make([]Index, n)
	copy(anc, parent)
	for k := 0; k < levelCount; k++ {
		next := NewBitMask(n)
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if current.Get(Index(i)) || current.Get(anc[i]) {
					next.AtomicOr(Index(i))
				}
			}
		})
		current = next
		if k+1 < levelCount {
			nextAnc := make([]Index, n)
			parallelFor(n, func(lo, hi int) {
				for i := lo; i < hi; i++ {
					nextAnc[i] = anc[anc[i]]
				}
			})
			anc = nextAnc
		}
	}
	return current
}
