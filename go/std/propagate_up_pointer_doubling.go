package std

// PropagateUpPointerDoubling propagates a mask upward in O(log d) steps via
// successively doubled ancestor pointers. Mirrors
// traaxx::propagate_up_pointer_doubling
// (cxx/traaxx/sources/propagate_up_pointer_doubling.cpp).
func PropagateUpPointerDoubling(parent []Index, seed BitMask, dMax Index) BitMask {
	n := len(parent)
	if n == 0 {
		return seed
	}
	levelCount := int(LevelCountFor(dMax))
	current := seed
	anc := make([]Index, n)
	copy(anc, parent)
	for k := 0; k < levelCount; k++ {
		next := current.Clone()
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if current.Get(Index(i)) {
					next.AtomicOr(anc[i])
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
