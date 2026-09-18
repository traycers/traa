package std

// Inverse builds old2new from new2old (mutually inverse permutation
// tables), one O(n) scatter pass. Mirrors traaxx::inverse
// (cxx/traaxx/include/traaxx/permutation.hpp).
func Inverse(new2old []Index) []Index {
	n := len(new2old)
	old2new := make([]Index, n)
	parallelFor(n, func(lo, hi int) {
		for k := lo; k < hi; k++ {
			old2new[new2old[k]] = Index(k)
		}
	})
	return old2new
}

// Gather permutes a plain data vector by new2old: out[k] = data[new2old[k]].
// For data vectors (no index translation) — contrast RemapIndexVector, used
// for parent/sibling. Mirrors traaxx::gather<T>.
func Gather[T any](data []T, new2old []Index) []T {
	out := make([]T, len(new2old))
	parallelFor(len(new2old), func(lo, hi int) {
		for k := lo; k < hi; k++ {
			out[k] = data[new2old[k]]
		}
	})
	return out
}

// RemapIndexVector permutes AND translates an index vector (parent/sibling):
// out[k] = old2new[vec[new2old[k]]]. Mirrors traaxx::remap_index_vector.
func RemapIndexVector(vec, new2old, old2new []Index) []Index {
	out := make([]Index, len(new2old))
	parallelFor(len(new2old), func(lo, hi int) {
		for k := lo; k < hi; k++ {
			out[k] = old2new[vec[new2old[k]]]
		}
	})
	return out
}
