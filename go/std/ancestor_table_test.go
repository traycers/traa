package std

import "testing"

func TestAncestorTableExampleTreeLevels(t *testing.T) {
	// docs/learning/array-trees/01-example-tree.md, depth = 5
	// docs/learning/array-trees/04-mask-propagation.md §5.4 (A1/A2/A4)
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	table := NewAncestorTable(parent, 5)
	expectedA1 := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	expectedA2 := []Index{0, 0, 0, 0, 1, 1, 2, 3, 4, 2, 4, 4, 5}
	expectedA4 := []Index{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1}
	if !equalIndexSlice(table.Level(0), expectedA1) {
		t.Errorf("A1 = %v, want %v", table.Level(0), expectedA1)
	}
	if !equalIndexSlice(table.Level(1), expectedA2) {
		t.Errorf("A2 = %v, want %v", table.Level(1), expectedA2)
	}
	if !equalIndexSlice(table.Level(2), expectedA4) {
		t.Errorf("A4 = %v, want %v", table.Level(2), expectedA4)
	}
}

func TestAncestorTableLevelCountIsCeilLog2PlusOne(t *testing.T) {
	parent := []Index{0, 0, 1, 1, 2, 3, 4, 5, 6, 4, 6, 9, 7}
	table := NewAncestorTable(parent, 5)
	if table.LevelCount() != 4 {
		t.Errorf("level count = %d, want 4", table.LevelCount())
	}
}

func TestAncestorTableSingleNodeDepthOne(t *testing.T) {
	parent := []Index{0}
	table := NewAncestorTable(parent, 1)
	if table.LevelCount() != 1 {
		t.Errorf("level count = %d, want 1", table.LevelCount())
	}
	if !equalIndexSlice(table.Level(0), parent) {
		t.Errorf("A1 = %v, want %v", table.Level(0), parent)
	}
}
