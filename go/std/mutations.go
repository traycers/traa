package std

// The five mutation methods, mirroring cxx/traaxx/sources/tree_mutations.cpp
// and traaxx::Tree<T,IndexT>'s five mutation methods
// (.scratch/cxx-std-arch/issues/12-mutations.md). None of them keep an
// incremental cache of nsibling/lastchild: each recomputes the derived
// vectors it needs from the current parent/sibling on every call — an
// explicit, deliberate O(n) divergence from the source theory's claimed
// O(1) append/deleteLeaf, in exchange for a Tree with no private state
// beyond parent/sibling/data.

// Append adds a new node as a child of p (becoming its new first/last
// child), with payload value, returning the new node's id. Mirrors
// traaxx::Tree<T,IndexT>::append. Go has no default-argument sugar, so the
// value parameter (T{} default in C++) is always required here.
func (t *Tree[T]) Append(p Index, value T) Index {
	x := Index(len(t.parent))
	pLastChild := LastChild(t.parent, t.sibling)[p]
	t.parent = append(t.parent, p)
	if pLastChild == p {
		t.sibling = append(t.sibling, x)
	} else {
		t.sibling = append(t.sibling, pLastChild)
	}
	t.data = append(t.data, value)
	return x
}

// DeleteLeaf removes leaf x from the sibling chain (logical delete — the
// slot is not physically compacted). Precondition: x is a leaf; violating it
// is a contract bug, not something this method validates (mirrors the C++
// assert-only precondition). Mirrors traaxx::Tree<T,IndexT>::deleteLeaf.
func (t *Tree[T]) DeleteLeaf(x Index) {
	r := NSibling(t.sibling)[x]
	if r != x {
		wasFirst := t.sibling[x] == x
		t.sibling[r] = t.sibling[x]
		if wasFirst {
			t.sibling[r] = r
		}
	}
}

// Splice removes node x, promoting its children to its former parent's
// child list in its place. Precondition: x is not the root. Mirrors
// traaxx::Tree<T,IndexT>::splice.
func (t *Tree[T]) Splice(x Index) {
	l := t.sibling[x]
	r := NSibling(t.sibling)[x]
	f := FirstChild(t.parent, t.sibling)[x]
	lc := LastChild(t.parent, t.sibling)[x]
	p := t.parent[x]
	for i, v := range t.parent {
		if v == x {
			t.parent[i] = p
		}
	}
	if f != x {
		if l != x {
			t.sibling[f] = l
		} else {
			t.sibling[f] = f
		}
		if r != x {
			t.sibling[r] = lc
		}
	} else if r != x {
		t.sibling[r] = l
		if l == x {
			t.sibling[r] = r
		}
	}
}

// DeleteSubtree removes the whole subtree rooted at x and compacts the
// remaining nodes' arrays (parent/sibling/data), shrinking the tree. Can
// fail if the cycle-check mask propagation (propagate_down_iterative from x)
// does not converge. Mirrors traaxx::Tree<T,IndexT>::deleteSubtree.
func (t *Tree[T]) DeleteSubtree(x Index) error {
	n := Index(len(t.parent))
	seed := NewBitMask(int(n))
	seed.Set(x)
	mask, _, err := PropagateDownIterative(t.parent, seed, n)
	if err != nil {
		return err
	}
	r := NSibling(t.sibling)[x]
	if r != x {
		wasFirst := t.sibling[x] == x
		t.sibling[r] = t.sibling[x]
		if wasFirst {
			t.sibling[r] = r
		}
	}
	new2old := []Index{}
	old2new := make([]Index, n)
	for i := Index(0); i < n; i++ {
		if !mask.Get(i) {
			old2new[i] = Index(len(new2old))
			new2old = append(new2old, i)
		}
	}
	t.parent = RemapIndexVector(t.parent, new2old, old2new)
	t.sibling = RemapIndexVector(t.sibling, new2old, old2new)
	t.data = Gather(t.data, new2old)
	return nil
}

// Move relocates node (or subtree root) x to become a child of q. Fails
// with MoveError{Kind: WouldCreateCycle} if q lies within x's own subtree
// (the ordinary, expected rejection), or MoveError{Kind: TreeCorrupted} if
// the cycle-check mask propagation itself fails to converge. The cycle
// check always uses the iterative mask strategy (propagate_down_iterative),
// never a DFS-position test — a move may run on a tree whose topological
// order invariant is already broken by a prior splice/move. Mirrors
// traaxx::Tree<T,IndexT>::move.
func (t *Tree[T]) Move(x, q Index) error {
	n := Index(len(t.parent))
	seed := NewBitMask(int(n))
	seed.Set(x)
	mask, iterations, err := PropagateDownIterative(t.parent, seed, n)
	if err != nil {
		convErr := err.(ConvergenceError)
		return MoveError{Kind: TreeCorrupted, Iterations: convErr.Iterations}
	}
	_ = iterations
	if mask.Get(q) {
		return MoveError{Kind: WouldCreateCycle}
	}
	qLastChild := LastChild(t.parent, t.sibling)[q]
	r := NSibling(t.sibling)[x]
	if r != x {
		wasFirst := t.sibling[x] == x
		t.sibling[r] = t.sibling[x]
		if wasFirst {
			t.sibling[r] = r
		}
	}
	t.parent[x] = q
	if qLastChild == q {
		t.sibling[x] = x
	} else {
		t.sibling[x] = qLastChild
	}
	return nil
}
