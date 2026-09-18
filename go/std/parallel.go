package std

import (
	"runtime"
	"sync"
)

// parallelFor is the one shared helper standing in for std::execution::par
// throughout traaxx: it chunks [0, n) across runtime.GOMAXPROCS(0) goroutines
// and calls fn(lo, hi) once per chunk, waiting for all chunks to finish
// before returning. Every place the C++ side uses std::execution::par on an
// elementwise transform/for_each over indices [0, n) reuses this helper
// instead of hand-rolling goroutine logic.
func parallelFor(n int, fn func(lo, hi int)) {
	if n <= 0 {
		return
	}
	workers := runtime.GOMAXPROCS(0)
	if workers > n {
		workers = n
	}
	if workers <= 1 {
		fn(0, n)
		return
	}
	chunk := (n + workers - 1) / workers
	var wg sync.WaitGroup
	for lo := 0; lo < n; lo += chunk {
		hi := lo + chunk
		if hi > n {
			hi = n
		}
		wg.Add(1)
		go func(lo, hi int) {
			defer wg.Done()
			fn(lo, hi)
		}(lo, hi)
	}
	wg.Wait()
}
