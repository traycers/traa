package std

import "sort"

// InvariantCheck enumerates the eight §13 invariant checks (not seven — the
// spec's "seven" counts table rows, where bounds and order are one row each;
// order is split here into parent_order/sibling_order). String values match
// the CLI JSON schema's snake_case check names exactly, so InvariantCheck
// can be marshaled as-is. Mirrors traaxx::InvariantCheck.
type InvariantCheck string

const (
	InvariantBounds               InvariantCheck = "bounds"
	InvariantParentOrder          InvariantCheck = "parent_order"
	InvariantSiblingOrder         InvariantCheck = "sibling_order"
	InvariantSingleRoot           InvariantCheck = "single_root"
	InvariantSiblingSameParent    InvariantCheck = "sibling_same_parent"
	InvariantSiblingInjective     InvariantCheck = "sibling_injective"
	InvariantReachableFromRoot    InvariantCheck = "reachable_from_root"
	InvariantPropagationConverges InvariantCheck = "propagation_converges"
)

// Violation reports one failed invariant check and the node ids involved.
// Mirrors traaxx::Violation<IndexT>.
type Violation struct {
	Check InvariantCheck
	Nodes []Index
}

// ValidationResult is the result of ValidateInvariants. Iterations is the
// convergence-iteration count of the single propagate_down_iterative(seed =
// roots) call stage 2 makes (feeding both reachable_from_root and
// propagation_converges) — nil if stage 2 did not run (bounds unclean) or if
// it ran but the map key isn't meaningful (kept as a pointer so the CLI can
// serialize it as JSON null in exactly that case). Mirrors
// traaxx::ValidationResult<IndexT>, extended with Iterations for the CLI
// contract's "iterations required alongside timing for validate_invariants"
// rule — the C++ struct has no such field since it never had to serialize
// to that envelope.
type ValidationResult struct {
	Valid      bool
	Violations []Violation
	Iterations *Index
}

// ValidateInvariants runs the eight §13 checks in two stages: stage 1
// (bounds, parent_order, sibling_order, single_root, sibling_injective)
// always runs; stage 2 (sibling_same_parent, reachable_from_root,
// propagation_converges) only if bounds is clean — three of the stage-1
// checks index parent[i]/sibling[i] as an array index, which would be
// unsafe (a Go slice index panic, standing in for the C++ side's UB) if
// bounds failed. Mirrors traaxx::validate_invariants
// (cxx/traaxx/include/traaxx/validate_invariants.hpp).
func ValidateInvariants[T any](tree *Tree[T]) ValidationResult {
	parent := tree.Parent()
	sibling := tree.Sibling()
	n := tree.NodesCount()
	violations := []Violation{}
	boundsNodes := []Index{}
	for i := Index(0); i < n; i++ {
		if parent[i] >= n || sibling[i] >= n {
			boundsNodes = append(boundsNodes, i)
		}
	}
	boundsClean := len(boundsNodes) == 0
	if !boundsClean {
		violations = append(violations, Violation{Check: InvariantBounds, Nodes: boundsNodes})
	}
	parentOrderNodes := []Index{}
	siblingOrderNodes := []Index{}
	rootNodes := []Index{}
	for i := Index(0); i < n; i++ {
		if parent[i] > i {
			parentOrderNodes = append(parentOrderNodes, i)
		}
		if sibling[i] > i {
			siblingOrderNodes = append(siblingOrderNodes, i)
		}
		if parent[i] == i {
			rootNodes = append(rootNodes, i)
		}
	}
	if len(parentOrderNodes) != 0 {
		violations = append(violations, Violation{Check: InvariantParentOrder, Nodes: parentOrderNodes})
	}
	if len(siblingOrderNodes) != 0 {
		violations = append(violations, Violation{Check: InvariantSiblingOrder, Nodes: siblingOrderNodes})
	}
	if len(rootNodes) != 1 {
		violations = append(violations, Violation{Check: InvariantSingleRoot, Nodes: rootNodes})
	}
	siblingValues := []Index{}
	for i := Index(0); i < n; i++ {
		if sibling[i] != i {
			siblingValues = append(siblingValues, sibling[i])
		}
	}
	sort.Slice(siblingValues, func(a, b int) bool { return siblingValues[a] < siblingValues[b] })
	injectiveNodes := []Index{}
	for k := 1; k < len(siblingValues); k++ {
		if siblingValues[k] == siblingValues[k-1] &&
			(len(injectiveNodes) == 0 || injectiveNodes[len(injectiveNodes)-1] != siblingValues[k]) {
			injectiveNodes = append(injectiveNodes, siblingValues[k])
		}
	}
	if len(injectiveNodes) != 0 {
		violations = append(violations, Violation{Check: InvariantSiblingInjective, Nodes: injectiveNodes})
	}
	var iterations *Index
	if boundsClean {
		sameParentNodes := []Index{}
		for i := Index(0); i < n; i++ {
			if sibling[i] != i && parent[sibling[i]] != parent[i] {
				sameParentNodes = append(sameParentNodes, i)
			}
		}
		if len(sameParentNodes) != 0 {
			violations = append(violations, Violation{Check: InvariantSiblingSameParent, Nodes: sameParentNodes})
		}
		seed := NewBitMask(int(n))
		for _, root := range rootNodes {
			seed.Set(root)
		}
		mask, iters, err := PropagateDownIterative(parent, seed, n)
		if err != nil {
			convErr := err.(ConvergenceError)
			iterations = &convErr.Iterations
			violations = append(violations, Violation{Check: InvariantPropagationConverges, Nodes: []Index{}})
		} else {
			iterations = &iters
			unreachedNodes := []Index{}
			for i := Index(0); i < n; i++ {
				if !mask.Get(i) {
					unreachedNodes = append(unreachedNodes, i)
				}
			}
			if len(unreachedNodes) != 0 {
				violations = append(violations, Violation{Check: InvariantReachableFromRoot, Nodes: unreachedNodes})
			}
		}
	}
	return ValidationResult{Valid: len(violations) == 0, Violations: violations, Iterations: iterations}
}
