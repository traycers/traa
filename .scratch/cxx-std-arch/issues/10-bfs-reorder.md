Type: grilling
Status: resolved
Blocked by: 07, 08

## Question

`docs/learning/array-trees/08-bfs-order.md` §9.2 (способ Б — независимо от DFS, зафиксировано [[03-bfs-independent]]): уровень 0 — корни на позиции 0; для каждого уровня `L=1..d` — отбор узлов с `depth=L`, сортировка по ключу (позиция родителя в уже построенном BFS, `rank`), выдача подряд идущих позиций; затем перевод `parent`/`sibling`/`data` тем же permutation-примитивом ([[08-permutation-remap]]), что и DFS.

Нужно решить архитектуру: сигнатура операции (симметрична ли она [[09-dfs-reorder]] — тот же вид результата, `BfsReorderResult<T,IndexT>`?), как получить `depth` и `rank` (вызов [[07-derived-vectors]] внутри) и как организован послойный цикл — `d` проходов с сортировкой каждого уровня как отдельный шаг, или это тоже сводится к более общему примитиву (например разновидность segmented sort/scan, если такой заведётся в [[09-dfs-reorder]])?

## Answer

Симметрично [[09-dfs-reorder]] во всём, кроме входных производных векторов и механики послойного шага:

- свободная функция: `bfs_reorder(const Tree<T,IndexT>&) -> std::expected<BfsReorderResult<T,IndexT>, ConvergenceError>`, `BfsReorderResult` = `{ Tree<T,IndexT> tree; std::vector<IndexT> new_to_old; std::vector<IndexT> old_to_new; }`;
- метод: `Tree<T,IndexT>::bfs_reorder() -> std::expected<ReorderTables<IndexT>, ConvergenceError>` (тот же тип `ReorderTables<IndexT>`, что и в 09).

`depth()` и `rank()` ([[07-derived-vectors]]) вычисляются внутри `bfs_reorder`, несходимость пробрасывается тем же `std::expected` — то же обоснование, что в 09 (Q17): единственная точка использования на пути к результату, single-call API.

Послойный цикл (§9.2) — `d` обычных проходов; на каждом уровне `std::stable_sort` (или эквивалент) по составному ключу (уже вычисленная новая позиция родителя, `rank`) прямо внутри `bfs_reorder.cpp`. Общего примитива с сегментированным scan из 09 нет — механика не совпадает (там scan вдоль `sibling` внутри одного прохода, здесь сортировка по уровню), переиспользовать нечего, тот же критерий «минимум два потребителя», что и в 09.

Контекст: [[07-derived-vectors]], [[08-permutation-remap]], [[09-dfs-reorder]] (симметричные решения).
