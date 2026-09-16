Type: grilling
Status: resolved
Blocked by: 01, 02

## Question

`docs/learning/array-trees/03-derived-vectors.md` §4 (плюс `depth` из §6.2 и `pos`/`size` из §8.2) — набор чистых функций от `parent`/`sibling`: `depth`, `rank`, `nsibling`, `firstchild`/`lastchild`, `size` (размер поддерева), `isleaf`, `pos` (DFS-позиция, требует `size`, пересекается с [[09-dfs-reorder]]).

Нужно решить архитектуру этой категории целиком:
- как `depth()` переписывается поверх общего propagate-примитива ([[01-propagate-primitive]]), сохраняя ли (или меняя) текущую сигнатуру `depth(const std::vector<IndexT>&, IndexT max_iterations) -> DepthResult<IndexT>` и throw-поведение снаружи;
- какие из остальных векторов (`rank`, `nsibling`, `firstchild`/`lastchild`, `isleaf`) — тоже пропускаются через propagate-примитив (например `rank` явно да, по `sibling`), а какие — прямые O(n) вычисления без итераций (`nsibling`/`isleaf`/`firstchild`/`lastchild` — по сути один проход-обращение, не propagation);
- `size` (снизу вверх по уровням, §8.2 шаг 2) — отдельная операция этой категории, или её место в [[09-dfs-reorder]] (она нужна только там)?
- единая ли сигнатура по категории (`std::vector<IndexT>` вход → `std::vector<IndexT>`/`std::vector<bool>` выход, без структуры-обёртки типа `DepthResult`, раз только `depth` нуждается в счётчике итераций) или каждая операция сохраняет свою обёртку с диагностикой сходимости. Ограничение от [[01-propagate-primitive]]: `depth()` снаружи по-прежнему бросает на `!converged`, но `iterations` — обязательное поле CLI-вывода для propagation-операций (`[[../../array-trees/issues/09-cli-schema-details]]`), поэтому вариант «голые векторы без обёртки» для `depth` целиком не проходит — какая-то форма возврата числа итераций (пусть не обязательно `DepthResult` в нынешнем виде) для неё обязана остаться.

## Answer

**Реопен [[01-propagate-primitive]]:** throw-поведение, зафиксированное в 01 для `depth()` (и по аналогии — для любой операции этой категории), заменяется на `std::expected`. Меняется весь паттерн, не только `depth()`: единообразно для всех публичных операций-обёрток над `propagate` по карте (07 здесь, а также 05/11/13 там, где они попадают в тот же случай) — иначе получаются два разных стиля сигнализации ошибок внутри одной категории. Сам примитив `propagate<...>` теперь возвращает `std::expected<T, E>` напрямую (не внутреннюю структуру-с-флагом, которую надо конвертировать на каждом вызывающем конце), с единым типом ошибки `E` на все операции категории — например `struct ConvergenceError { IndexT iterations; }`; публичная обёртка просто ретранслирует то, что вернул примитив. Тулчейн подтверждён: `clang 21.1.8` (Ubuntu 26.04, пакет apt) собирает `<expected>` под `-std=c++23` (уже стоит в `cxx/CMakeLists.txt`) без проблем.

`depth()` сохраняет входную сигнатуру `depth(const std::vector<IndexT>&, IndexT max_iterations)`, меняется только возвращаемый тип (`std::expected<std::vector<IndexT>, ConvergenceError>` вместо бросающего `DepthResult`) и тело (делегирование в `propagate`). `max_iterations` остаётся параметром — лимит остановки для самого примитива.

Разбиение категории по способу вычисления:
- через `propagate` (итеративная свёртка, возвращают `std::expected<std::vector<IndexT>, ConvergenceError>`): `depth` (по `parent`, `f = v[p]+1`), `rank` (по `sibling`), `size` (снизу вверх по уровням, `f = +`, §8.2 шаг 2 — см. ниже);
- прямой O(n) проход без итераций (голый `std::vector<...>`, `std::expected` не нужен — сходимости там нет по определению): `nsibling`, `isleaf` (→ `std::vector<bool>`), `firstchild`, `lastchild`.

`size` — отдельная функция этой категории (07), а не часть операции DFS-переупорядочивания ([[09-dfs-reorder]]). Это чистая функция от `parent` в исходной индексации, по форме вычисления идентичная `rank`/счётчикам LCA-множества (scatter с редукцией `+` вдоль `parent`), и используется не только для `pos` в DFS-reorder, но и самостоятельно — тест предка `pos[a] <= pos[b] < pos[a]+size[a]` (§10.7) нужен независимо от того, вызывался ли DFS-reorder. `[[09-dfs-reorder]]` потребляет её как готовый вход, не пересчитывает внутри себя.

Контекст: [[01-propagate-primitive]] (амендмент), [[05-mask-propagation-strategies]], [[09-dfs-reorder]], [[11-lca-variants]], [[13-invariant-validation]] (все три читают/пробрасывают `std::expected` вместо флага/throw).
