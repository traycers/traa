package std

import "testing"

func hasViolation(result ValidationResult, check InvariantCheck) bool {
	for _, v := range result.Violations {
		if v.Check == check {
			return true
		}
	}
	return false
}

func validateExampleTree() *Tree[Index] {
	// docs/learning/array-trees/01-example-tree.md
	parent := exampleParent()
	sibling := exampleSibling()
	data := make([]Index, len(parent))
	return NewTree(parent, sibling, data)
}

func TestValidateInvariantsValidExampleTreeHasNoViolations(t *testing.T) {
	result := ValidateInvariants(validateExampleTree())
	if !result.Valid {
		t.Errorf("expected valid tree")
	}
	if len(result.Violations) != 0 {
		t.Errorf("expected no violations, got %v", result.Violations)
	}
}

func TestValidateInvariantsDetectsOutOfBounds(t *testing.T) {
	tree := validateExampleTree()
	parent := append([]Index{}, tree.Parent()...)
	parent[5] = 999
	tree = NewTree(parent, tree.Sibling(), tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantBounds) {
		t.Errorf("expected bounds violation")
	}
}

func TestValidateInvariantsDetectsParentOrderViolation(t *testing.T) {
	tree := validateExampleTree()
	parent := append([]Index{}, tree.Parent()...)
	parent[1] = 5
	tree = NewTree(parent, tree.Sibling(), tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantParentOrder) {
		t.Errorf("expected parent_order violation")
	}
}

func TestValidateInvariantsDetectsSiblingOrderViolation(t *testing.T) {
	tree := validateExampleTree()
	sibling := append([]Index{}, tree.Sibling()...)
	sibling[1] = 5
	tree = NewTree(tree.Parent(), sibling, tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantSiblingOrder) {
		t.Errorf("expected sibling_order violation")
	}
}

func TestValidateInvariantsDetectsZeroRoots(t *testing.T) {
	tree := validateExampleTree()
	parent := append([]Index{}, tree.Parent()...)
	parent[0] = 1
	tree = NewTree(parent, tree.Sibling(), tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantSingleRoot) {
		t.Errorf("expected single_root violation")
	}
}

func TestValidateInvariantsDetectsMultipleRoots(t *testing.T) {
	tree := validateExampleTree()
	parent := append([]Index{}, tree.Parent()...)
	parent[4] = 4
	tree = NewTree(parent, tree.Sibling(), tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantSingleRoot) {
		t.Errorf("expected single_root violation")
	}
}

func TestValidateInvariantsDetectsSiblingSameParentViolation(t *testing.T) {
	tree := validateExampleTree()
	sibling := append([]Index{}, tree.Sibling()...)
	// node 9's real predecessor is 4 (self-loop, first child of 4); point it
	// at node 3 instead (parent[3]=1 != parent[9]=4), without breaking
	// sibling order (3<=9).
	sibling[9] = 3
	tree = NewTree(tree.Parent(), sibling, tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantSiblingSameParent) {
		t.Errorf("expected sibling_same_parent violation")
	}
}

func TestValidateInvariantsDetectsSiblingInjectiveViolation(t *testing.T) {
	tree := validateExampleTree()
	sibling := append([]Index{}, tree.Sibling()...)
	// node 10 originally claims 8 as its predecessor; make it also claim 6,
	// which node 9 already claims — two children now claim the same
	// predecessor.
	sibling[10] = 6
	tree = NewTree(tree.Parent(), sibling, tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantSiblingInjective) {
		t.Errorf("expected sibling_injective violation")
	}
}

func TestValidateInvariantsDetectsUnreachableComponent(t *testing.T) {
	// Node 0 is the sole root; nodes 3 and 4 form a 2-cycle disconnected from
	// it (parent[3]=4, parent[4]=3) — parent_order fails for node 3, but
	// bounds/single_root still hold, so stage 2 still runs.
	parent := []Index{0, 0, 1, 4, 3}
	sibling := []Index{0, 1, 2, 3, 4}
	data := make([]Index, len(parent))
	tree := NewTree(parent, sibling, data)
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantParentOrder) {
		t.Errorf("expected parent_order violation")
	}
	if !hasViolation(result, InvariantReachableFromRoot) {
		t.Errorf("expected reachable_from_root violation")
	}
	if hasViolation(result, InvariantPropagationConverges) {
		t.Errorf("did not expect propagation_converges violation")
	}
	for _, v := range result.Violations {
		if v.Check == InvariantReachableFromRoot {
			expected := []Index{3, 4}
			if !equalIndexSlice(v.Nodes, expected) {
				t.Errorf("reachable_from_root nodes = %v, want %v", v.Nodes, expected)
			}
		}
	}
}

func TestValidateInvariantsBoundsFailureSuppressesStageTwo(t *testing.T) {
	tree := validateExampleTree()
	parent := append([]Index{}, tree.Parent()...)
	sibling := append([]Index{}, tree.Sibling()...)
	parent[5] = 999 // bounds violation
	sibling[9] = 3  // would also trip sibling_same_parent, if stage 2 ran
	tree = NewTree(parent, sibling, tree.Data())
	result := ValidateInvariants(tree)
	if result.Valid {
		t.Errorf("expected invalid tree")
	}
	if !hasViolation(result, InvariantBounds) {
		t.Errorf("expected bounds violation")
	}
	if hasViolation(result, InvariantSiblingSameParent) {
		t.Errorf("stage 2 should not have run")
	}
	if hasViolation(result, InvariantReachableFromRoot) {
		t.Errorf("stage 2 should not have run")
	}
	if hasViolation(result, InvariantPropagationConverges) {
		t.Errorf("stage 2 should not have run")
	}
	if result.Iterations != nil {
		t.Errorf("iterations should be nil when stage 2 did not run, got %v", *result.Iterations)
	}
}
