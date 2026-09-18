package std

import "testing"

func TestLevelCountForSingleNodeDepthOne(t *testing.T) {
	if got := LevelCountFor(1); got != 1 {
		t.Errorf("got %d, want 1", got)
	}
}

func TestLevelCountForDepthZeroTreatedAsOne(t *testing.T) {
	if got := LevelCountFor(0); got != 1 {
		t.Errorf("got %d, want 1", got)
	}
}

func TestLevelCountForIsCeilLog2PlusOne(t *testing.T) {
	// ceil(log2(5)) = 3, plus one level of margin.
	if got := LevelCountFor(5); got != 4 {
		t.Errorf("got %d, want 4", got)
	}
}

func TestLevelCountForExactPowerOfTwo(t *testing.T) {
	if got := LevelCountFor(4); got != 3 {
		t.Errorf("got %d, want 3", got)
	}
	if got := LevelCountFor(8); got != 4 {
		t.Errorf("got %d, want 4", got)
	}
}
