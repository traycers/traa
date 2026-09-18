package std

// LevelCountFor computes the number of ancestor-table levels (K+1) needed to
// reach any node at distance up to dMax via pointer doubling: ceil(log2(dMax))
// plus one level of margin. Mirrors traaxx::level_count_for
// (cxx/traaxx/sources/level_count_for.cpp).
func LevelCountFor(dMax Index) Index {
	if dMax <= 1 {
		return 1
	}
	k := Index(0)
	reach := Index(1)
	for reach < dMax {
		reach *= 2
		k++
	}
	return k + 1
}
