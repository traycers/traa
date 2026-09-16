Type: grilling
Status: resolved
Blocked by: 07, 08

## Question

`docs/learning/array-trees/07-dfs-order.md` §8.2 (способ Б, единственный в объёме карты array-trees — см. `[[../../array-trees/issues/02-operation-set]]`): `depth` → `size` (снизу вверх по уровням) → `pos` сверху вниз (`pos[i] = pos[parent[i]] + 1 + sum(size левых братьев)`, сумма — сегментированный scan вдоль цепочки `sibling`) → `old2new = pos`, `new2old = inverse(pos)` → перевод `parent`/`sibling`/`data` через permutation-примитив ([[08-permutation-remap]]).

Нужно решить архитектуру этой операции целиком: сигнатура (принимает `const Tree<T,IndexT>&`, возвращает что — новый `Tree<T,IndexT>` плюс `new2old`/`old2new`, отдельную структуру `DfsReorderResult<T,IndexT>`?), откуда берутся `depth`/`size` (вызов [[07-derived-vectors]] внутри, или они передаются готовыми аргументами), и как реализован сегментированный scan для шага «сумма размеров левых братьев» — отдельный переиспользуемый примитив (сегментированный scan вдоль `sibling`, по аналогии с Blelloch) или разовый частный код внутри этой операции, раз больше нигде в наборе §4–§11+§13 такой scan не нужен.

## Answer

Форма возврата следует из уже зафиксированной в другой карте CLI JSON-схемы (`array-trees/issues/08-json-schema.md`: результат DFS/BFS-переупорядочивания — `{"new_to_old", "old_to_new", "parent", "sibling"}`) — таблицы перестановки обязаны быть видны наружу, просто `Tree` (из общей конвенции [[08-permutation-remap]]) недостаточно:

- свободная функция: `dfs_reorder(const Tree<T,IndexT>&) -> std::expected<DfsReorderResult<T,IndexT>, ConvergenceError>`, `DfsReorderResult` = `{ Tree<T,IndexT> tree; std::vector<IndexT> new_to_old; std::vector<IndexT> old_to_new; }`;
- метод: `Tree<T,IndexT>::dfs_reorder() -> std::expected<ReorderTables<IndexT>, ConvergenceError>`, `ReorderTables` = `{ std::vector<IndexT> new_to_old; std::vector<IndexT> old_to_new; }` (дерево не дублируется в результате — оно и есть `*this` после мутации).

`depth()`/`size()` ([[07-derived-vectors]]) вызываются **внутри** `dfs_reorder`, не передаются аргументами — единственная реальная точка их использования на пути к `pos`, и single-call API проще использовать правильно (не нужно вручную прокидывать `depth`/`size` той же версии дерева). Несходимость любого из них пробрасывается через `std::expected` на выходе самой `dfs_reorder`.

Сегментированный scan вдоль `sibling` (шаг «сумма размеров левых братьев» при вычислении `pos`) — **разовый код внутри `dfs_reorder.cpp`**, не отдельный примитив. Критерий выделения примитива по карте — переиспользование минимум в двух-трёх местах (`propagate`: 4+, `AncestorTable`: 3, permutation/remap: 3+); у этого scan ровно один потребитель в объёме карты — выделение абстракции без второго вызывающего кода было бы спекулятивным обобщением.

Контекст: [[07-derived-vectors]], [[08-permutation-remap]], [[10-bfs-reorder]] (симметричные решения).
