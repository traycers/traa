package std

// PropagateUpSinglePass propagates a mask upward in a single sequential
// pass, in reverse topological order. Mirrors
// traaxx::propagate_up_single_pass
// (cxx/traaxx/sources/propagate_up_single_pass.cpp). Clones seed before
// mutating it (see PropagateDownSinglePass for why).
func PropagateUpSinglePass(parent []Index, seed BitMask) BitMask {
	result := seed.Clone()
	n := len(parent)
	for i := n; i > 0; i-- {
		idx := i - 1
		if result.Get(Index(idx)) {
			result.Set(parent[idx])
		}
	}
	return result
}
