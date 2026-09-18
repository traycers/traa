package std

// Rank computes, for every node i, its distance along the sibling chain from
// the first-child sentinel, via the shared propagate primitive (link =
// sibling, f(v,s) = v[s]+1), mirroring traaxx::rank
// (cxx/traaxx/sources/rank.cpp). iterations is not returned for the same
// reason as Depth: rank is a plain derived vector in the CLI contract.
func Rank(sibling []Index, maxIterations Index) ([]Index, error) {
	n := len(sibling)
	if n == 0 {
		return []Index{}, nil
	}
	step := func(current []Index) []Index {
		next := make([]Index, n)
		parallelFor(n, func(lo, hi int) {
			for i := lo; i < hi; i++ {
				if sibling[i] == Index(i) {
					next[i] = 0
				} else {
					next[i] = current[sibling[i]] + 1
				}
			}
		})
		return next
	}
	state, _, err := propagate(make([]Index, n), step, equalIndexSlice, maxIterations)
	if err != nil {
		return nil, err
	}
	return state, nil
}
