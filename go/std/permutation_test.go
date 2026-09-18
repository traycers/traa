package std

import "testing"

func TestInverseMatchesShuffledExample(t *testing.T) {
	// docs/learning/array-trees/06-permutations.md §7.4
	new2old := []Index{8, 3, 0, 11, 5, 1, 9, 12, 2, 6, 4, 10, 7}
	old2new := Inverse(new2old)
	expected := []Index{2, 5, 8, 1, 10, 4, 9, 12, 0, 6, 11, 3, 7}
	if !equalIndexSlice(old2new, expected) {
		t.Errorf("old2new = %v, want %v", old2new, expected)
	}
}

func TestGatherPlainlyPermutesData(t *testing.T) {
	new2old := []Index{2, 0, 1}
	data := []Index{100, 101, 102}
	gathered := Gather(data, new2old)
	expected := []Index{102, 100, 101}
	if !equalIndexSlice(gathered, expected) {
		t.Errorf("gathered = %v, want %v", gathered, expected)
	}
}

func TestRemapIndexVectorTranslatesShuffledParentSibling(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md (original) and
	// 06-permutations.md §7.4 (shuffled new2old and expected parent'/sibling')
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	sibling := []Index{0, 1, 2, 2, 4, 5, 6, 7, 8, 6, 8, 11, 12}
	new2old := []Index{8, 3, 0, 11, 5, 1, 9, 12, 2, 6, 4, 10, 7}
	old2new := Inverse(new2old)
	parentPrime := RemapIndexVector(parent, new2old, old2new)
	siblingPrime := RemapIndexVector(sibling, new2old, old2new)
	expectedParentPrime := []Index{9, 5, 2, 6, 1, 2, 10, 12, 5, 10, 8, 9, 4}
	expectedSiblingPrime := []Index{0, 8, 2, 3, 4, 5, 9, 7, 8, 9, 10, 0, 12}
	if !equalIndexSlice(parentPrime, expectedParentPrime) {
		t.Errorf("parent' = %v, want %v", parentPrime, expectedParentPrime)
	}
	if !equalIndexSlice(siblingPrime, expectedSiblingPrime) {
		t.Errorf("sibling' = %v, want %v", siblingPrime, expectedSiblingPrime)
	}
}

func TestIdentityPermutationIsNoOp(t *testing.T) {
	n := 5
	new2old := make([]Index, n)
	for i := range new2old {
		new2old[i] = Index(i)
	}
	old2new := Inverse(new2old)
	if !equalIndexSlice(old2new, new2old) {
		t.Errorf("old2new = %v, want %v", old2new, new2old)
	}
	parent := []Index{0, 0, 1, 1, 2}
	remapped := RemapIndexVector(parent, new2old, old2new)
	if !equalIndexSlice(remapped, parent) {
		t.Errorf("remapped = %v, want %v", remapped, parent)
	}
}
