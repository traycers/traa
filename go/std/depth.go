package std

// Depth computes, for every node i, its distance from the root, via the
// shared propagate primitive (link = parent, f(v,p) = v[p]+1), mirroring
// traaxx::depth (cxx/traaxx/sources/depth.cpp). A root (parent[i] == i) gets
// depth 0 directly rather than running through the general recurrence.
//
// iterations is deliberately not returned: the CLI contract classifies depth
// as a plain derived vector, not a propagation-strategy operation, so its
// "iterations" field is always null regardless of how many propagate passes
// this took internally.
func Depth(parent []Index, maxIterations Index) ([]Index, error) {
	n := len(parent)
	if n == 0 {
		return []Index{}, nil
	}
	step := func(current []Index) []Index {
		next := make([]Index, n)
		parallelFor(n, func(lo, hi int) {
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
	state, _, err := propagate(make([]Index, n), step, equalIndexSlice, maxIterations)
	if err != nil {
		return nil, err
	}
	return state, nil
}
