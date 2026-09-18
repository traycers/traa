// Package std is traa's Go standard-library track: a port of cxx/traaxx
// (the frozen architecture in .scratch/cxx-std-arch/) to idiomatic Go.
//
// The tree representation is three parallel slices (SoA) — parent, sibling,
// data — exactly mirroring traaxx::Tree<T,IndexT>. Index is fixed to a plain
// uint32 alias (not int) because the C++ side only ever instantiates
// IndexT = std::uint32_t (see the explicit template instantiation at the
// bottom of cxx/traaxx/sources/depth.cpp); using Go's 8-byte int would double
// the memory traffic of the parent/sibling arrays relative to the C++ side,
// which would corrupt the point of comparing implementations on matched data
// layout.
package std

// Index is the node-id / array-index type used throughout this package. It
// is a type alias (not a defined type) for uint32, matching traaxx's
// IndexT = std::uint32_t exactly — so it can be used interchangeably with
// uint32 at every boundary (JSON encoding included) with no conversions.
type Index = uint32

// ConvergenceError reports that a propagate-based operation did not reach a
// fixed point within the given iteration budget. It mirrors traaxx's single
// reused ConvergenceError<IndexT>, reused across every propagate-based
// operation in this package (depth, rank, the two iterative mask-propagation
// strategies, validate_invariants) rather than one error type per operation.
type ConvergenceError struct {
	Iterations Index
}

func (e ConvergenceError) Error() string {
	return "propagation did not converge within the iteration budget"
}

// MoveErrorKind enumerates the two independent reasons Tree.Move can fail,
// mirroring traaxx::MoveError<IndexT>::Kind.
type MoveErrorKind int

const (
	// WouldCreateCycle is the ordinary, expected outcome: q lies within the
	// subtree rooted at x, so the move would break the tree's acyclicity.
	WouldCreateCycle MoveErrorKind = iota
	// TreeCorrupted means the cycle-check mask propagation itself failed to
	// converge — a pathological case, distinct from WouldCreateCycle.
	TreeCorrupted
)

// MoveError mirrors traaxx::MoveError<IndexT>: a move can fail for either an
// ordinary structural reason (WouldCreateCycle) or a pathological one
// (TreeCorrupted, iterations meaningful only in that case).
type MoveError struct {
	Kind       MoveErrorKind
	Iterations Index
}

func (e MoveError) Error() string {
	switch e.Kind {
	case WouldCreateCycle:
		return "move would create a cycle"
	default:
		return "tree corrupted: cycle-check propagation did not converge"
	}
}
