# Parallel scatter-aggregation over parent/sibling uses BitMask, not std::vector<bool>

Some derived-vector operations in `cxx/traaxx` need a per-node boolean that can't be answered from that node's own fields — e.g. `isleaf[i]` ("does `i` have children?") requires collecting marks scattered from every other node's `parent[j]` value, unlike a local check such as "is `i` a root" (`parent[i] != i`, a pure gather, trivially parallel, no synchronization needed). We parallelize this scatter via `BitMask<IndexT>::atomic_or`, never via a `std::vector<bool>` scratch buffer: the C++ standard explicitly excludes `vector<bool>` from the guarantee that concurrent writes to different elements of a container are race-free (it's the one container carved out of that guarantee, being bit-packed) — this holds regardless of whether the writes are chunked per-element or per-word.

Self-loops (`parent[i] == i`, how a root marks itself) are always excluded from the scatter, by the same reasoning `depth()` gives the root `0` directly instead of running it through the general `depth[parent[i]]+1` recurrence: a self-loop isn't a real parent-child edge. Skipping this exclusion misclassifies an isolated, childless root sitting among other nodes in a forest (e.g. `parent = [0, 2, 2]` — node 0 must be a leaf, but an unguarded scatter has it mark itself as "has a child").

## Consequences

- When the operation's return type is fixed to `std::vector<bool>` (e.g. `isleaf`), only the final `BitMask` → `vector<bool>` materialization is sequential — that's a cheap linear pass, not the scatter itself, and the same `vector<bool>` exclusion applies to it too.
- `BitMask::invert()` exists for the "mark, then invert" shape this pattern produces — prefer it over hand-writing the bit negation inline.
