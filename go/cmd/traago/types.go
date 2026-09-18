package main

import "encoding/json"

// corpusFile mirrors corpus/<shape>.json
// (.scratch/array-trees/issues/08-json-schema.md). This CLI supports only
// the T=uint32 primitive payload for now (the same restriction
// cxx/traapp/sources/main.cpp currently has) — a generic payload-type
// loader (struct payloads, the canary shape) is intentionally out of scope
// here; see loadTree in main.go.
type corpusFile struct {
	Shape         string      `json:"shape"`
	NodesCount    int         `json:"nodes_count"`
	ExpectedValid bool        `json:"expected_valid"`
	PayloadType   payloadType `json:"payload_type"`
	Parent        []uint32    `json:"parent"`
	Sibling       []uint32    `json:"sibling"`
	Data          []uint32    `json:"data"`
}

type payloadType struct {
	Kind  string `json:"kind"`
	Value string `json:"value"`
}

// caseFile mirrors cases/<shape>/<case_id>.json: {"op": "...", "args": {...}}.
// args is absent for ops that take none.
type caseFile struct {
	Op   string          `json:"op"`
	Args json.RawMessage `json:"args"`
}

// manifestEntry mirrors one element of a --manifest file:
// [{"tree": "...", "case": "...", "repeat": N}, ...].
type manifestEntry struct {
	Tree   string `json:"tree"`
	Case   string `json:"case"`
	Repeat uint64 `json:"repeat"`
}

// singleEnvelope is the success envelope for single (--tree/--case) mode.
type singleEnvelope struct {
	Result            any     `json:"result"`
	TimingNs          []int64 `json:"timing_ns"`
	Iterations        *uint32 `json:"iterations"`
	PeakRSSDeltaBytes int64   `json:"peak_rss_delta_bytes"`
}

// batchEnvelope is one element of the success envelope array for
// --manifest (batch) mode — the same shape minus peak_rss_delta_bytes.
type batchEnvelope struct {
	Result     any     `json:"result"`
	TimingNs   []int64 `json:"timing_ns"`
	Iterations *uint32 `json:"iterations"`
}

// errorEnvelope is the error envelope written to stderr on exit 1.
type errorEnvelope struct {
	Error   string `json:"error"`
	Message string `json:"message"`
}
