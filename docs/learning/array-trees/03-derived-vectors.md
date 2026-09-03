# 4. Производные векторы

Всё, что не `parent` и не `sibling`, вычисляется. Базовый набор:

| Вектор | Смысл | Как получить |
|--------|-------|--------------|
| `depth[i]` | глубина узла | propagation по `parent` ([`05-propagation-pattern.md`](05-propagation-pattern.md) §6.2) |
| `rank[i]` | номер среди братьев | propagation по `sibling` ([`02-path-coordinate-matrix.md`](02-path-coordinate-matrix.md) §3.6) |
| `nsibling[i]` | правый сосед (или `i`) | обращение `sibling`: `nsibling[sibling[j]] = j` для `sibling[j] != j` |
| `lastchild[p]` | последний ребёнок | узел `j` с `parent[j] = p`, у которого нет правого соседа |
| `firstchild[p]` | первый ребёнок | узел `j` с `parent[j] = p` и `sibling[j] = j` |
| `size[i]` | размер поддерева (в узлах) | накопление снизу вверх по уровням ([`07-dfs-order.md`](07-dfs-order.md) §8.2) |
| `isleaf[i]` | лист | `i` не встречается в `parent` (кроме самоссылки корня) |

Для примера из [«01. Сквозной пример»](01-example-tree.md):

```
id       =  0  1  2  3  4  5  6  7  8  9 10 11 12
size     = 13 12  7  4  6  3  3  2  1  2  1  1  1
isleaf   =  .  .  .  .  .  .  .  .  x  .  x  x  x
```

Обращение `sibling` — типовой приём: вектор «влево» превращается в вектор «вправо» одной операцией scatter ([`06-permutations.md`](06-permutations.md) §7.1), без сортировки:

```go
// nsibling[i] == i означает «правого соседа нет» (последний ребёнок)
func nextSibling(sibling []uint32) []uint32 {
	ns := make([]uint32, len(sibling))
	for i := range ns {
		ns[i] = uint32(i)
	}
	for j, s := range sibling {
		if int(s) != j { // не первый ребёнок → j стоит справа от s
			ns[s] = uint32(j)
		}
	}
	return ns
}
```
