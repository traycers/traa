package main

import (
	"encoding/json"
	"fmt"

	std "traa/std"
)

// opResult is what an op handler produces: the CLI-schema "result" value
// (already shaped per .scratch/array-trees/issues/08-json-schema.md) plus
// the "iterations" field (nil serializes as JSON null — required for LCA,
// mutations, DFS/BFS-reorder and the plain derived vectors; filled for the
// three propagation strategies and for validate_invariants, per
// .scratch/array-trees/issues/09-cli-schema-details.md).
type opResult struct {
	Result     any
	Iterations *uint32
}

// opFunc computes one operation over tree given its case-file args (raw,
// possibly absent — parseArgs below handles that). Only the propagation
// primitive itself can fail to converge; per this repo's fixed corpus
// (.scratch/array-trees/issues/09-json-schema.md: only corrupted-invariants
// exercises non-convergent structure, and only through validate_invariants),
// this is not expected to be hit outside validate_invariants — see the
// comment on errInvalidTreeStructure below for how it's surfaced when it is.
type opFunc func(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error)

// opEntry pairs a handler with whether it mutates its tree argument. The
// CLI clones the tree before every warmup/repeat invocation of a mutating
// op, so --repeat > 1 measures the same single mutation N times rather than
// applying it N times in sequence (the five mutation methods mutate their
// receiver in place — see go/std/mutations.go).
type opEntry struct {
	fn      opFunc
	mutates bool
}

var opRegistry = map[string]opEntry{
	"depth":      {fn: opDepth},
	"rank":       {fn: opRank},
	"size":       {fn: opSize},
	"nsibling":   {fn: opNSibling},
	"isleaf":     {fn: opIsLeaf},
	"firstchild": {fn: opFirstChild},
	"lastchild":  {fn: opLastChild},

	"propagate_down_iterative":        {fn: opPropagateDownIterative},
	"propagate_up_iterative":          {fn: opPropagateUpIterative},
	"propagate_down_single_pass":      {fn: opPropagateDownSinglePass},
	"propagate_up_single_pass":        {fn: opPropagateUpSinglePass},
	"propagate_down_pointer_doubling": {fn: opPropagateDownPointerDoubling},
	"propagate_up_pointer_doubling":   {fn: opPropagateUpPointerDoubling},

	"dfs_reorder": {fn: opDfsReorder},
	"bfs_reorder": {fn: opBfsReorder},

	"lca_naive":          {fn: opLcaNaive},
	"lca_binary_lifting": {fn: opLcaBinaryLifting},
	"lca_batch":          {fn: opLcaBatch},
	"lca_set":            {fn: opLcaSet},

	"mutate_append":         {fn: opMutateAppend, mutates: true},
	"mutate_delete_leaf":    {fn: opMutateDeleteLeaf, mutates: true},
	"mutate_splice":         {fn: opMutateSplice, mutates: true},
	"mutate_delete_subtree": {fn: opMutateDeleteSubtree, mutates: true},
	"mutate_move":           {fn: opMutateMove, mutates: true},

	"validate_invariants": {fn: opValidateInvariants},
}

// parseArgs unmarshals a case file's (possibly absent) args field into v,
// reporting args_mismatch — not invalid_case_json, since the case's JSON
// itself parsed fine; it's this op's argument *shape* that's wrong — on
// any failure or on an args-required-but-absent mismatch.
func parseArgs(args json.RawMessage, v any) error {
	if len(args) == 0 || string(args) == "null" {
		return newCliError("args_mismatch", "missing required args")
	}
	if err := json.Unmarshal(args, v); err != nil {
		return newCliError("args_mismatch", "malformed args: "+err.Error())
	}
	return nil
}

func maskToBoolSlice(mask std.BitMask, n int) []bool {
	result := make([]bool, n)
	for i := 0; i < n; i++ {
		result[i] = mask.Get(std.Index(i))
	}
	return result
}

// errAsConvergenceIterations extracts, when possible, the iteration budget
// a ConvergenceError reports, for the (expected to be unreachable outside
// validate_invariants, given the fixed corpus) case where a propagation
// this CLI treats as unconditional fails to converge. Such a failure is
// surfaced as invalid_tree_json: the tree parsed as valid JSON, but its
// structure doesn't support this computation — the closest fit among the
// five fixed error codes, since none of them names "operation-level
// semantic failure" and this repo's corpus never actually exercises the
// path (only corrupted-invariants is malformed, and it only has a
// validate_invariants case).
func wrapConvergenceError(op string, err error) error {
	if convErr, ok := err.(std.ConvergenceError); ok {
		return newCliError("invalid_tree_json",
			fmt.Sprintf("%s: propagation did not converge after %d iterations", op, convErr.Iterations))
	}
	return err
}

func opDepth(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	result, err := std.Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("depth", err)
	}
	return opResult{Result: result}, nil
}

func opRank(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	result, err := std.Rank(tree.Sibling(), tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("rank", err)
	}
	return opResult{Result: result}, nil
}

func opSize(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	return opResult{Result: std.Size(tree.Parent())}, nil
}

func opNSibling(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	return opResult{Result: std.NSibling(tree.Sibling())}, nil
}

func opIsLeaf(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	return opResult{Result: std.IsLeaf(tree.Parent())}, nil
}

func opFirstChild(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	return opResult{Result: std.FirstChild(tree.Parent(), tree.Sibling())}, nil
}

func opLastChild(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	return opResult{Result: std.LastChild(tree.Parent(), tree.Sibling())}, nil
}

// seedArgs is the args shape attested by
// .scratch/array-trees/issues/09-cli-schema-details.md for all six mask
// propagation ops: {"seed": [node ids...]}.
type seedArgs struct {
	Seed []std.Index `json:"seed"`
}

func buildSeedMask(n int, args json.RawMessage) (std.BitMask, error) {
	var a seedArgs
	if err := parseArgs(args, &a); err != nil {
		return std.BitMask{}, err
	}
	mask := std.NewBitMask(n)
	for _, s := range a.Seed {
		mask.Set(s)
	}
	return mask, nil
}

func opPropagateDownIterative(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	mask, iterations, err := std.PropagateDownIterative(tree.Parent(), seed, tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("propagate_down_iterative", err)
	}
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

func opPropagateUpIterative(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	mask, iterations, err := std.PropagateUpIterative(tree.Parent(), seed, tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("propagate_up_iterative", err)
	}
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

func opPropagateDownSinglePass(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	mask := std.PropagateDownSinglePass(tree.Parent(), seed)
	iterations := uint32(1)
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

func opPropagateUpSinglePass(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	mask := std.PropagateUpSinglePass(tree.Parent(), seed)
	iterations := uint32(1)
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

func treeDMax(tree *std.Tree[std.Index]) (std.Index, error) {
	depth, err := std.Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		return 0, err
	}
	dMax := std.Index(0)
	for _, d := range depth {
		if d > dMax {
			dMax = d
		}
	}
	return dMax, nil
}

func opPropagateDownPointerDoubling(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	dMax, err := treeDMax(tree)
	if err != nil {
		return opResult{}, wrapConvergenceError("propagate_down_pointer_doubling", err)
	}
	mask := std.PropagateDownPointerDoubling(tree.Parent(), seed, dMax)
	iterations := std.LevelCountFor(dMax)
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

func opPropagateUpPointerDoubling(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	n := int(tree.NodesCount())
	seed, err := buildSeedMask(n, args)
	if err != nil {
		return opResult{}, err
	}
	dMax, err := treeDMax(tree)
	if err != nil {
		return opResult{}, wrapConvergenceError("propagate_up_pointer_doubling", err)
	}
	mask := std.PropagateUpPointerDoubling(tree.Parent(), seed, dMax)
	iterations := std.LevelCountFor(dMax)
	return opResult{Result: maskToBoolSlice(mask, n), Iterations: &iterations}, nil
}

// reorderResultJSON mirrors the DFS/BFS-reorder result shape from
// .scratch/array-trees/issues/08-json-schema.md exactly: new_to_old,
// old_to_new, parent, sibling — data is deliberately not part of this shape.
type reorderResultJSON struct {
	NewToOld []std.Index `json:"new_to_old"`
	OldToNew []std.Index `json:"old_to_new"`
	Parent   []std.Index `json:"parent"`
	Sibling  []std.Index `json:"sibling"`
}

func opDfsReorder(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	result, err := std.DfsReorder(tree)
	if err != nil {
		return opResult{}, wrapConvergenceError("dfs_reorder", err)
	}
	return opResult{Result: reorderResultJSON{
		NewToOld: result.NewToOld,
		OldToNew: result.OldToNew,
		Parent:   result.Tree.Parent(),
		Sibling:  result.Tree.Sibling(),
	}}, nil
}

func opBfsReorder(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	result, err := std.BfsReorder(tree)
	if err != nil {
		return opResult{}, wrapConvergenceError("bfs_reorder", err)
	}
	return opResult{Result: reorderResultJSON{
		NewToOld: result.NewToOld,
		OldToNew: result.OldToNew,
		Parent:   result.Tree.Parent(),
		Sibling:  result.Tree.Sibling(),
	}}, nil
}

// abArgs is the args shape attested for lca_naive: {"a": ..., "b": ...}.
type abArgs struct {
	A std.Index `json:"a"`
	B std.Index `json:"b"`
}

func opLcaNaive(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a abArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	result, err := std.LcaNaive(tree, a.A, a.B)
	if err != nil {
		return opResult{}, wrapConvergenceError("lca_naive", err)
	}
	return opResult{Result: result}, nil
}

func opLcaBinaryLifting(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a abArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	depth, err := std.Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("lca_binary_lifting", err)
	}
	dMax := std.Index(0)
	for _, d := range depth {
		if d > dMax {
			dMax = d
		}
	}
	table := std.NewAncestorTable(tree.Parent(), dMax)
	result := std.LcaBinaryLifting(tree, table, depth, a.A, a.B)
	return opResult{Result: result}, nil
}

// batchArgs is the args shape assumed for lca_batch, following the C++
// signature's own parameter names: {"a": [...], "b": [...]} — same-length
// vectors of query pairs. Not attested directly by the frozen CLI schema
// (only lca_naive's single-pair {a,b} is), documented here as an assumption.
type batchArgs struct {
	A []std.Index `json:"a"`
	B []std.Index `json:"b"`
}

func opLcaBatch(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a batchArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	depth, err := std.Depth(tree.Parent(), tree.NodesCount())
	if err != nil {
		return opResult{}, wrapConvergenceError("lca_batch", err)
	}
	dMax := std.Index(0)
	for _, d := range depth {
		if d > dMax {
			dMax = d
		}
	}
	table := std.NewAncestorTable(tree.Parent(), dMax)
	result := std.LcaBatch(tree, table, depth, a.A, a.B)
	return opResult{Result: result}, nil
}

// setArgs is the args shape assumed for lca_set, following the C++
// parameter name s: {"s": [node ids...]}. Also not directly attested.
type setArgs struct {
	S []std.Index `json:"s"`
}

func opLcaSet(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a setArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	result := std.LcaSet(tree, a.S)
	return opResult{Result: result}, nil
}

// mutationSnapshot is the shared golden-result shape for all five
// mutations: a full post-mutation snapshot, never a delta.
type mutationSnapshot struct {
	Parent     []std.Index `json:"parent"`
	Sibling    []std.Index `json:"sibling"`
	Data       []std.Index `json:"data"`
	NodesCount std.Index   `json:"nodes_count"`
}

func snapshotOf(tree *std.Tree[std.Index]) mutationSnapshot {
	return mutationSnapshot{
		Parent:     tree.Parent(),
		Sibling:    tree.Sibling(),
		Data:       tree.Data(),
		NodesCount: tree.NodesCount(),
	}
}

// appendArgs is the args shape assumed for mutate_append, following the
// C++ method signature Tree::append(p, value): {"p": ..., "value": ...}.
type appendArgs struct {
	P     std.Index `json:"p"`
	Value std.Index `json:"value"`
}

func opMutateAppend(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a appendArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	tree.Append(a.P, a.Value)
	return opResult{Result: snapshotOf(tree)}, nil
}

// xArgs is the args shape assumed for the three single-node mutations
// (mutate_delete_leaf, mutate_splice, mutate_delete_subtree): {"x": ...}.
type xArgs struct {
	X std.Index `json:"x"`
}

func opMutateDeleteLeaf(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a xArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	tree.DeleteLeaf(a.X)
	return opResult{Result: snapshotOf(tree)}, nil
}

func opMutateSplice(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a xArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	tree.Splice(a.X)
	return opResult{Result: snapshotOf(tree)}, nil
}

func opMutateDeleteSubtree(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a xArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	if err := tree.DeleteSubtree(a.X); err != nil {
		return opResult{}, wrapConvergenceError("mutate_delete_subtree", err)
	}
	return opResult{Result: snapshotOf(tree)}, nil
}

// xqArgs is the args shape attested for mutate_move:
// .scratch/array-trees/issues/08-json-schema.md's
// cases/example/mutate_move_4_to_7.json example: {"x": ..., "q": ...}.
type xqArgs struct {
	X std.Index `json:"x"`
	Q std.Index `json:"q"`
}

func opMutateMove(tree *std.Tree[std.Index], args json.RawMessage) (opResult, error) {
	var a xqArgs
	if err := parseArgs(args, &a); err != nil {
		return opResult{}, err
	}
	if err := tree.Move(a.X, a.Q); err != nil {
		// Move's own MoveError (WouldCreateCycle / TreeCorrupted) is not a
		// ConvergenceError, so wrapConvergenceError won't touch it — surface
		// it the same way (invalid_tree_json), for the same reason: none of
		// the five fixed codes names "operation-level semantic failure",
		// and the fixed corpus never actually exercises this path (no
		// mutation cases are defined against corrupted-invariants).
		return opResult{}, newCliError("invalid_tree_json", "mutate_move: "+err.Error())
	}
	return opResult{Result: snapshotOf(tree)}, nil
}

type violationJSON struct {
	Check string      `json:"check"`
	Nodes []std.Index `json:"nodes"`
}

type validationResultJSON struct {
	Valid      bool            `json:"valid"`
	Violations []violationJSON `json:"violations"`
}

func opValidateInvariants(tree *std.Tree[std.Index], _ json.RawMessage) (opResult, error) {
	result := std.ValidateInvariants(tree)
	violations := make([]violationJSON, len(result.Violations))
	for i, v := range result.Violations {
		violations[i] = violationJSON{Check: string(v.Check), Nodes: v.Nodes}
	}
	return opResult{
		Result:     validationResultJSON{Valid: result.Valid, Violations: violations},
		Iterations: result.Iterations,
	}, nil
}
