Type: grilling
Status: resolved

## Question

Сейчас в `cxx/traaxx` один файл на операцию: `include/traaxx/depth.hpp` + `sources/depth.cpp` (+ зеркальный `tests/t_traaxx/sources/test_depth.cpp`). Новый набор — около 20 функций/методов по шести категориям (производные векторы, propagation-маска ×3 стратегии, DFS, BFS, LCA ×4 варианта, мутации ×5, валидация). Продолжать паттерн «один файл на операцию», или сгруппировать по категориям (`derived_vectors.hpp`, `propagation.hpp`, `lca.hpp`, `mutations.hpp`, ...)?

## Answer

**Один файл на операцию** — паттерн `depth.hpp`/`depth.cpp`/`test_*.cpp` продолжается без изменений на весь новый набор. Имена файлов — близкие к именам `op` из `[[../../array-trees/issues/09-cli-schema-details]]` там, где они уже зафиксированы схемой (например `lca_naive`, `mutate_append`), и по смыслу операции там, где `op` схемой ещё не назван (например переиспользуемые примитивы вроде propagation-шаблона или ancestor table, у которых нет собственного `op` — они не вызываются из CLI напрямую).

Точный список имён файлов и то, какие функции физически попадают в один и тот же `.hpp`/`.cpp` (например три стратегии propagation-маски — три файла или один с тремя функциями) — решается по ходу соответствующих тикетов ([[05-mask-propagation-strategies]] и далее), не фиксируется здесь заранее.

Контекст: [[05-mask-propagation-strategies]], [[06-ancestor-table]], [[07-derived-vectors]], [[08-permutation-remap]], [[09-dfs-reorder]], [[10-bfs-reorder]], [[11-lca-variants]], [[12-mutations]], [[13-invariant-validation]].
