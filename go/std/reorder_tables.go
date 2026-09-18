package std

// ReorderTables is the pair of mutually-inverse permutation tables produced
// by a reorder operation (DFS/BFS), mirroring traaxx::ReorderTables<IndexT>.
type ReorderTables struct {
	NewToOld []Index
	OldToNew []Index
}
