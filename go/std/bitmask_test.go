package std

import (
	"sync"
	"testing"
)

func TestBitMaskConstructedZeroInitialized(t *testing.T) {
	mask := NewBitMask(70)
	for i := Index(0); i < mask.Size(); i++ {
		if mask.Get(i) {
			t.Errorf("bit %d should be unset", i)
		}
	}
}

func TestBitMaskSizeReportsBitCount(t *testing.T) {
	mask := NewBitMask(130)
	if mask.Size() != 130 {
		t.Errorf("size = %d, want 130", mask.Size())
	}
}

func TestBitMaskSetRoundTrip(t *testing.T) {
	mask := NewBitMask(70)
	mask.Set(0)
	mask.Set(63)
	mask.Set(64)
	mask.Set(69)
	for _, i := range []Index{0, 63, 64, 69} {
		if !mask.Get(i) {
			t.Errorf("bit %d should be set", i)
		}
	}
	for _, i := range []Index{1, 62, 65, 68} {
		if mask.Get(i) {
			t.Errorf("bit %d should be unset", i)
		}
	}
}

func TestBitMaskAtomicOrRoundTrip(t *testing.T) {
	mask := NewBitMask(10)
	mask.AtomicOr(2)
	mask.AtomicOr(5)
	if !mask.Get(2) || !mask.Get(5) {
		t.Errorf("bits 2 and 5 should be set")
	}
	if mask.Get(0) || mask.Get(9) {
		t.Errorf("bits 0 and 9 should be unset")
	}
}

func TestBitMaskAtomicOrDoesNotClobberNeighborBitsInSameWord(t *testing.T) {
	mask := NewBitMask(64)
	mask.AtomicOr(3)
	mask.AtomicOr(40)
	for i := Index(0); i < mask.Size(); i++ {
		expected := i == 3 || i == 40
		if mask.Get(i) != expected {
			t.Errorf("bit %d = %v, want %v", i, mask.Get(i), expected)
		}
	}
}

func TestBitMaskEqualityComparesContent(t *testing.T) {
	a := NewBitMask(20)
	b := NewBitMask(20)
	if !a.Equal(b) {
		t.Errorf("a should equal b")
	}
	a.Set(5)
	if a.Equal(b) {
		t.Errorf("a should not equal b")
	}
	b.Set(5)
	if !a.Equal(b) {
		t.Errorf("a should equal b again")
	}
}

func TestBitMaskInvertFlipsAllValidBits(t *testing.T) {
	mask := NewBitMask(70)
	mask.Set(0)
	mask.Set(64)
	mask.Invert()
	for i := Index(0); i < mask.Size(); i++ {
		expected := i != 0 && i != 64
		if mask.Get(i) != expected {
			t.Errorf("bit %d = %v, want %v", i, mask.Get(i), expected)
		}
	}
}

func TestBitMaskInvertTwiceIsIdentity(t *testing.T) {
	mask := NewBitMask(70)
	mask.Set(3)
	mask.Set(69)
	original := mask.Clone()
	mask.Invert()
	mask.Invert()
	if !mask.Equal(original) {
		t.Errorf("double invert should be identity")
	}
}

func TestBitMaskInvertClearsPaddingBitsInLastWord(t *testing.T) {
	// size is not a multiple of 64 - inverting must not leave stray 1-bits
	// past bitCount in the last word, or two masks built the same way would
	// stop comparing equal after each is inverted.
	a := NewBitMask(70)
	b := NewBitMask(70)
	a.Invert()
	b.Invert()
	if !a.Equal(b) {
		t.Errorf("a should equal b after inverting both")
	}
}

func TestBitMaskEmptyReportsZeroSizedMask(t *testing.T) {
	empty := NewBitMask(0)
	nonEmpty := NewBitMask(1)
	if !empty.Empty() {
		t.Errorf("empty mask should report Empty() == true")
	}
	if nonEmpty.Empty() {
		t.Errorf("non-empty mask should report Empty() == false")
	}
	// Invert on a zero-sized mask must not panic (no last word to touch).
	empty.Invert()
}

func TestBitMaskAndReturnsIntersectionWithoutMutatingOperands(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(70)
	a.Set(3)
	a.Set(64)
	b.Set(3)
	b.Set(69)
	result := a.And(b)
	if !result.Get(3) {
		t.Errorf("bit 3 should be set in intersection")
	}
	if result.Get(64) || result.Get(69) {
		t.Errorf("bits 64/69 should not be set in intersection")
	}
	if !a.Get(64) || !b.Get(69) {
		t.Errorf("operands must be unmodified")
	}
}

func TestBitMaskOrReturnsUnionWithoutMutatingOperands(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(70)
	a.Set(3)
	b.Set(69)
	result := a.Or(b)
	if !result.Get(3) || !result.Get(69) {
		t.Errorf("union should have both bits set")
	}
	if a.Get(69) || b.Get(3) {
		t.Errorf("operands must be unmodified")
	}
}

func TestBitMaskNotReturnsInvertedCopyWithoutMutatingOperand(t *testing.T) {
	mask := NewBitMask(70)
	result := mask.Not()
	if !result.Get(0) {
		t.Errorf("inverted copy should have bit 0 set")
	}
	if mask.Get(0) {
		t.Errorf("original must be unmodified")
	}
}

func TestBitMaskAndAssignMutatesInPlace(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(70)
	a.Set(3)
	a.Set(64)
	b.Set(3)
	a.AndAssign(b)
	if !a.Get(3) {
		t.Errorf("bit 3 should remain set")
	}
	if a.Get(64) {
		t.Errorf("bit 64 should be cleared")
	}
}

func TestBitMaskOrAssignMutatesInPlace(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(70)
	a.Set(3)
	b.Set(69)
	a.OrAssign(b)
	if !a.Get(3) || !a.Get(69) {
		t.Errorf("both bits should be set")
	}
}

func TestBitMaskAndOnMismatchedSizesReturnsEmpty(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(64)
	result := a.And(b)
	if !result.Empty() {
		t.Errorf("mismatched-size And should return an empty mask")
	}
}

func TestBitMaskOrOnMismatchedSizesReturnsEmpty(t *testing.T) {
	a := NewBitMask(70)
	b := NewBitMask(64)
	result := a.Or(b)
	if !result.Empty() {
		t.Errorf("mismatched-size Or should return an empty mask")
	}
}

func TestBitMaskAndAssignOnMismatchedSizesIsNoop(t *testing.T) {
	a := NewBitMask(70)
	a.Set(3)
	b := NewBitMask(64)
	a.AndAssign(b)
	if !a.Get(3) {
		t.Errorf("bit 3 should remain set")
	}
	if a.Size() != 70 {
		t.Errorf("size should remain 70")
	}
}

func TestBitMaskOrAssignOnMismatchedSizesIsNoop(t *testing.T) {
	a := NewBitMask(70)
	a.Set(3)
	b := NewBitMask(64)
	a.OrAssign(b)
	if !a.Get(3) {
		t.Errorf("bit 3 should remain set")
	}
	if a.Get(64) {
		t.Errorf("bit 64 should remain unset")
	}
	if a.Size() != 70 {
		t.Errorf("size should remain 70")
	}
}

func TestBitMaskIndicesReturnsPositionsOfSetBitsInOrder(t *testing.T) {
	mask := NewBitMask(70)
	mask.Set(3)
	mask.Set(64)
	mask.Set(69)
	idx := mask.Indices()
	expected := []Index{3, 64, 69}
	if !equalIndexSlice(idx, expected) {
		t.Errorf("indices = %v, want %v", idx, expected)
	}
}

func TestBitMaskIndicesIsEmptyWhenNoBitsSet(t *testing.T) {
	mask := NewBitMask(70)
	if len(mask.Indices()) != 0 {
		t.Errorf("expected no set bits")
	}
}

func TestBitMaskConcurrentAtomicOrOnSameWordIsRaceFree(t *testing.T) {
	mask := NewBitMask(64)
	var wg sync.WaitGroup
	for i := Index(0); i < 64; i++ {
		wg.Add(1)
		go func(i Index) {
			defer wg.Done()
			mask.AtomicOr(i)
		}(i)
	}
	wg.Wait()
	for i := Index(0); i < mask.Size(); i++ {
		if !mask.Get(i) {
			t.Errorf("bit %d should be set", i)
		}
	}
}
