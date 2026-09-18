package std

import "testing"

// docs/learning/array-trees/01-example-tree.md
var maskExampleParent = []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}

var maskExampleN = Index(len(maskExampleParent))

const maskExampleDMax = Index(5)

func seedAt(n Index, bit Index) BitMask {
	mask := NewBitMask(int(n))
	mask.Set(bit)
	return mask
}

func expectBits(t *testing.T, mask BitMask, expected []Index) {
	t.Helper()
	set := map[Index]bool{}
	for _, e := range expected {
		set[e] = true
	}
	for i := Index(0); i < maskExampleN; i++ {
		if mask.Get(i) != set[i] {
			t.Errorf("bit %d = %v, want %v", i, mask.Get(i), set[i])
		}
	}
}

func TestPropagateMaskDownIterativeMatchesWorkedExample(t *testing.T) {
	result, _, err := PropagateDownIterative(maskExampleParent, seedAt(maskExampleN, 2), maskExampleN)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectBits(t, result, []Index{2, 4, 6, 8, 9, 10, 11})
}

func TestPropagateMaskUpIterativeMatchesWorkedExample(t *testing.T) {
	result, _, err := PropagateUpIterative(maskExampleParent, seedAt(maskExampleN, 10), maskExampleN)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	expectBits(t, result, []Index{0, 1, 2, 4, 6, 10})
}

func TestPropagateMaskDownSinglePassMatchesWorkedExample(t *testing.T) {
	result := PropagateDownSinglePass(maskExampleParent, seedAt(maskExampleN, 2))
	expectBits(t, result, []Index{2, 4, 6, 8, 9, 10, 11})
}

func TestPropagateMaskUpSinglePassMatchesWorkedExample(t *testing.T) {
	result := PropagateUpSinglePass(maskExampleParent, seedAt(maskExampleN, 10))
	expectBits(t, result, []Index{0, 1, 2, 4, 6, 10})
}

func TestPropagateMaskDownPointerDoublingMatchesWorkedExample(t *testing.T) {
	result := PropagateDownPointerDoubling(maskExampleParent, seedAt(maskExampleN, 2), maskExampleDMax)
	expectBits(t, result, []Index{2, 4, 6, 8, 9, 10, 11})
}

func TestPropagateMaskUpPointerDoublingMatchesWorkedExample(t *testing.T) {
	result := PropagateUpPointerDoubling(maskExampleParent, seedAt(maskExampleN, 10), maskExampleDMax)
	expectBits(t, result, []Index{0, 1, 2, 4, 6, 10})
}

func TestPropagateMaskAllThreeStrategiesAgreeOnDown(t *testing.T) {
	iterative, _, err := PropagateDownIterative(maskExampleParent, seedAt(maskExampleN, 3), maskExampleN)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	singlePass := PropagateDownSinglePass(maskExampleParent, seedAt(maskExampleN, 3))
	doubling := PropagateDownPointerDoubling(maskExampleParent, seedAt(maskExampleN, 3), maskExampleDMax)
	if !iterative.Equal(singlePass) {
		t.Errorf("iterative != single-pass")
	}
	if !iterative.Equal(doubling) {
		t.Errorf("iterative != pointer-doubling")
	}
}

func TestPropagateMaskAllThreeStrategiesAgreeOnUp(t *testing.T) {
	iterative, _, err := PropagateUpIterative(maskExampleParent, seedAt(maskExampleN, 11), maskExampleN)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	singlePass := PropagateUpSinglePass(maskExampleParent, seedAt(maskExampleN, 11))
	doubling := PropagateUpPointerDoubling(maskExampleParent, seedAt(maskExampleN, 11), maskExampleDMax)
	if !iterative.Equal(singlePass) {
		t.Errorf("iterative != single-pass")
	}
	if !iterative.Equal(doubling) {
		t.Errorf("iterative != pointer-doubling")
	}
}
