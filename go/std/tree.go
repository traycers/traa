package std

// Tree is traa's three-parallel-array (SoA) tree representation: parent,
// sibling, data, generic ONLY over the payload type T (the index type is
// fixed to Index = uint32, see index.go). Mirrors traaxx::Tree<T,IndexT>
// (cxx/traaxx/include/traaxx/traaxx.hpp).
//
// parent/sibling/data are unexported; read access goes through the
// accessors below (mirroring traaxx's const accessors), and the only
// sanctioned way to change a Tree's structure is one of its five mutation
// methods (Append/DeleteLeaf/Splice/DeleteSubtree/Move, in mutations.go).
// Every other operation in this package is a free function over *Tree[T]
// (read-only) or one returning a new *Tree[T] (copy-returning DFS/BFS
// reorder) — per .scratch/cxx-std-arch/issues/02-tree-encapsulation.md.
//
// Note on Go vs. C++ constness: accessors return the underlying slice
// directly rather than a defensive copy (matching Go idiom and traaxx's
// zero-copy const std::vector<IndexT>& return) — callers must treat the
// returned slices as read-only, since Go cannot enforce this at compile time
// the way a const& does.
type Tree[T any] struct {
	parent  []Index
	sibling []Index
	data    []T
}

// NewTree constructs a Tree from parent/sibling/data vectors, mirroring
// traaxx::Tree<T,IndexT>'s vector constructor.
func NewTree[T any](parent, sibling []Index, data []T) *Tree[T] {
	return &Tree[T]{parent: parent, sibling: sibling, data: data}
}

// Parent returns the tree's parent vector.
func (t *Tree[T]) Parent() []Index {
	return t.parent
}

// Sibling returns the tree's sibling vector.
func (t *Tree[T]) Sibling() []Index {
	return t.sibling
}

// Data returns the tree's payload vector.
func (t *Tree[T]) Data() []T {
	return t.data
}

// NodesCount returns the number of nodes in the tree.
func (t *Tree[T]) NodesCount() Index {
	return Index(len(t.parent))
}
