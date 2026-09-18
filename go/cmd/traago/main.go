// Command traago is the Go std-track CLI runner, mirroring cxx/traapp's
// contract exactly (.scratch/array-trees/issues/09-cli-schema-details.md):
// same flags, same JSON envelopes, same fixed error codes — but dispatching
// every op this package's std library implements, not just depth (unlike
// cxx/traapp, which today only wires up depth — a separate, known-narrower
// effort on the C++ side).
package main

import (
	"encoding/json"
	"fmt"
	"os"
	"syscall"
	"time"

	std "traa/std"
)

type cliArgs struct {
	impl     string
	tree     string
	caseFile string
	manifest string
	warmup   uint64
	repeat   uint64
}

func parseFlags(argv []string) (cliArgs, error) {
	args := cliArgs{warmup: 3, repeat: 20}
	i := 0
	next := func(flag string) (string, error) {
		i++
		if i >= len(argv) {
			return "", newCliError("args_mismatch", "missing value for "+flag)
		}
		return argv[i], nil
	}
	for ; i < len(argv); i++ {
		flag := argv[i]
		switch flag {
		case "--impl":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			args.impl = v
		case "--tree":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			args.tree = v
		case "--case":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			args.caseFile = v
		case "--manifest":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			args.manifest = v
		case "--warmup":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			n, parseErr := parseUint(v)
			if parseErr != nil {
				return args, newCliError("args_mismatch", "invalid --warmup: "+v)
			}
			args.warmup = n
		case "--repeat":
			v, err := next(flag)
			if err != nil {
				return args, err
			}
			n, parseErr := parseUint(v)
			if parseErr != nil {
				return args, newCliError("args_mismatch", "invalid --repeat: "+v)
			}
			args.repeat = n
		default:
			return args, newCliError("args_mismatch", "unknown flag "+flag)
		}
	}
	if args.impl == "" {
		return args, newCliError("args_mismatch", "--impl is required")
	}
	if args.impl != "std" {
		return args, newCliError("args_mismatch", "--impl "+args.impl+" is not built into this binary (only std is)")
	}
	singleMode := args.tree != "" || args.caseFile != ""
	batchMode := args.manifest != ""
	if singleMode == batchMode {
		return args, newCliError("args_mismatch", "exactly one of --tree/--case or --manifest is required")
	}
	if singleMode && (args.tree == "" || args.caseFile == "") {
		return args, newCliError("args_mismatch", "--tree and --case must be given together")
	}
	return args, nil
}

func parseUint(s string) (uint64, error) {
	var n uint64
	if s == "" {
		return 0, fmt.Errorf("empty")
	}
	for _, c := range s {
		if c < '0' || c > '9' {
			return 0, fmt.Errorf("not a number: %s", s)
		}
		n = n*10 + uint64(c-'0')
	}
	return n, nil
}

func readJSONFile(path string, errCodeOnParse string, v any) error {
	data, err := os.ReadFile(path)
	if err != nil {
		if os.IsNotExist(err) {
			return newCliError("file_not_found", "cannot open "+path)
		}
		return newCliError("file_not_found", err.Error())
	}
	if err := json.Unmarshal(data, v); err != nil {
		return newCliError(errCodeOnParse, err.Error())
	}
	return nil
}

// loadTree loads a corpus file, supporting only {"kind":"primitive",
// "value":"uint32"} payloads for now — the same restriction
// cxx/traapp/sources/main.cpp currently has. A generic payload-type loader
// (struct payloads, the canary corpus shape) is intentionally out of scope:
// note this explicitly rather than over-build a loader nothing exercises
// yet.
func loadTree(path string) (*std.Tree[std.Index], error) {
	var corpus corpusFile
	if err := readJSONFile(path, "invalid_tree_json", &corpus); err != nil {
		return nil, err
	}
	if corpus.PayloadType.Kind != "primitive" || corpus.PayloadType.Value != "uint32" {
		return nil, newCliError("invalid_tree_json", "unsupported payload_type")
	}
	return std.NewTree(corpus.Parent, corpus.Sibling, corpus.Data), nil
}

func loadCase(path string) (caseFile, error) {
	var c caseFile
	err := readJSONFile(path, "invalid_case_json", &c)
	return c, err
}

func cloneTree(tree *std.Tree[std.Index]) *std.Tree[std.Index] {
	parent := append([]std.Index{}, tree.Parent()...)
	sibling := append([]std.Index{}, tree.Sibling()...)
	data := append([]std.Index{}, tree.Data()...)
	return std.NewTree(parent, sibling, data)
}

func peakRSSBytes() int64 {
	var usage syscall.Rusage
	_ = syscall.Getrusage(syscall.RUSAGE_SELF, &usage)
	return int64(usage.Maxrss) * 1024
}

// runScenario executes one (tree, op, args) scenario with the given
// warmup/repeat counts, returning the last iteration's result/iterations
// (all iterations are expected to agree, since the op is either read-only
// or freshly re-applied to a clone each time) plus the per-iteration hot
// timings.
func runScenario(tree *std.Tree[std.Index], entry opEntry, args json.RawMessage, warmup, repeat uint64) (opResult, []int64, error) {
	invoke := func() (opResult, error) {
		if entry.mutates {
			return entry.fn(cloneTree(tree), args)
		}
		return entry.fn(tree, args)
	}
	for i := uint64(0); i < warmup; i++ {
		if _, err := invoke(); err != nil {
			return opResult{}, nil, err
		}
	}
	timings := make([]int64, 0, repeat)
	var result opResult
	for i := uint64(0); i < repeat; i++ {
		start := time.Now()
		r, err := invoke()
		elapsed := time.Since(start)
		if err != nil {
			return opResult{}, nil, err
		}
		result = r
		timings = append(timings, elapsed.Nanoseconds())
	}
	return result, timings, nil
}

func runSingle(args cliArgs) (int, error) {
	tree, err := loadTree(args.tree)
	if err != nil {
		return 1, err
	}
	c, err := loadCase(args.caseFile)
	if err != nil {
		return 1, err
	}
	entry, ok := opRegistry[c.Op]
	if !ok {
		return 1, newCliError("unknown_op", "unsupported op: "+c.Op)
	}
	rssBefore := peakRSSBytes()
	result, timings, err := runScenario(tree, entry, c.Args, args.warmup, args.repeat)
	if err != nil {
		return 1, err
	}
	rssAfter := peakRSSBytes()
	envelope := singleEnvelope{
		Result:            result.Result,
		TimingNs:          timings,
		Iterations:        result.Iterations,
		PeakRSSDeltaBytes: rssAfter - rssBefore,
	}
	return writeSuccess(envelope)
}

func runBatch(args cliArgs) (int, error) {
	var manifest []manifestEntry
	if err := readJSONFile(args.manifest, "invalid_case_json", &manifest); err != nil {
		return 1, err
	}
	treeCache := map[string]*std.Tree[std.Index]{}
	envelopes := make([]batchEnvelope, 0, len(manifest))
	for _, m := range manifest {
		tree, ok := treeCache[m.Tree]
		if !ok {
			loaded, err := loadTree(m.Tree)
			if err != nil {
				return 1, err
			}
			tree = loaded
			treeCache[m.Tree] = tree
		}
		c, err := loadCase(m.Case)
		if err != nil {
			return 1, err
		}
		entry, ok := opRegistry[c.Op]
		if !ok {
			return 1, newCliError("unknown_op", "unsupported op: "+c.Op)
		}
		repeat := m.Repeat
		if repeat == 0 {
			repeat = args.repeat
		}
		result, timings, err := runScenario(tree, entry, c.Args, args.warmup, repeat)
		if err != nil {
			return 1, err
		}
		envelopes = append(envelopes, batchEnvelope{
			Result:     result.Result,
			TimingNs:   timings,
			Iterations: result.Iterations,
		})
	}
	return writeSuccess(envelopes)
}

func writeSuccess(v any) (int, error) {
	out, err := json.Marshal(v)
	if err != nil {
		return 1, newCliError("invalid_case_json", err.Error())
	}
	fmt.Println(string(out))
	return 0, nil
}

func writeError(err error) {
	code := "invalid_case_json"
	message := err.Error()
	if ce, ok := err.(*cliError); ok {
		code = ce.Code
		message = ce.Message
	}
	out, _ := json.Marshal(errorEnvelope{Error: code, Message: message})
	fmt.Fprintln(os.Stderr, string(out))
}

func run(argv []string) int {
	args, err := parseFlags(argv)
	if err != nil {
		writeError(err)
		return 1
	}
	var exitCode int
	if args.manifest != "" {
		exitCode, err = runBatch(args)
	} else {
		exitCode, err = runSingle(args)
	}
	if err != nil {
		writeError(err)
		return 1
	}
	return exitCode
}

func main() {
	os.Exit(run(os.Args[1:]))
}
