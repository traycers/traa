package std

// PropagateDownSinglePass propagates a mask downward in a single sequential
// pass, requiring the topological order invariant parent[i] <= i. Mirrors
// traaxx::propagate_down_single_pass
// (cxx/traaxx/sources/propagate_down_single_pass.cpp). Clones seed before
// mutating it, so — unlike a raw Go slice — the caller's mask is left
// untouched, matching the C++ pass-by-value seed parameter.
func PropagateDownSinglePass(parent []Index, seed BitMask) BitMask {
	result := seed.Clone()
	n := len(parent)
	for i := 0; i < n; i++ {
		if result.Get(parent[i]) {
			result.Set(Index(i))
		}
	}
	return result
}
