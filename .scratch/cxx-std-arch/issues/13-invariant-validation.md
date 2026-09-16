Type: grilling
Status: resolved
Blocked by: 01, 02

## Question

`docs/learning/array-trees/11-summary-invariants-complexity.md` §13 — семь проверок (`bounds`, `parent_order`/`sibling_order`, `single_root`, `sibling_same_parent`, `sibling_injective`, `reachable_from_root`, `propagation_converges`), результат по JSON-схеме (`[[../../array-trees/issues/08-json-schema]]`): `{"valid": bool, "violations": [{"check": <enum>, "nodes": [...]}]}`.

Нужно решить архитектуру `validate_invariants`: C++-представление enum проверок (`enum class` со строковыми именами snake_case, как в JSON?) и структуры нарушения; выполняются ли все семь проверок всегда (набрать полный список `violations`) или короткое замыкание на первой ошибке, которая делает остальные проверки бессмысленными (например `bounds` до `parent_order`, раз проверка порядка не имеет смысла на индексах вне диапазона); как `reachable_from_root` и `propagation_converges` переиспользуют propagate-примитив ([[01-propagate-primitive]]) в его небросающей форме (зафиксировано в [[01-propagate-primitive]]) — читают его флаг сходимости напрямую; сигнатура (свободная функция над `const Tree<T,IndexT>&` → `ValidationResult`, зафиксировано операцией только для чтения в [[02-tree-encapsulation]]).

## Answer

**Восемь значений enum, не семь** (разрешает несостыковку, уже присутствующую в зафиксированной CLI-схеме `array-trees/08-json-schema.md`, которая называет это «семью строками», но буквально перечисляет `parent_order`/`sibling_order` раздельно): `enum class InvariantCheck { bounds, parent_order, sibling_order, single_root, sibling_same_parent, sibling_injective, reachable_from_root, propagation_converges };`. «Семь» в источнике (`11-summary-invariants-complexity.md` §13) считает строки таблицы (`bounds` и `order` там — по одной строке на оба поля), а не итоговые enum-литералы; `bounds` остаётся объединённым (одно значение на `parent`+`sibling` — нарушение почти всегда означает «битые данные с диска», где неважно, какое именно поле не в диапазоне), `order` расщеплён на `parent_order`/`sibling_order` (осмысленно различать: сломан `parent` — под угрозой ацикличность; сломан `sibling` — только цепочка братьев).

`ValidationResult = { bool valid; std::vector<Violation> violations; }`, `Violation = { InvariantCheck check; std::vector<IndexT> nodes; }`.

**Двухступенчатое выполнение — не по эффективности, а по безопасности памяти.** Три из восьми проверок используют значение `parent[i]`/`sibling[i]` **как индекс** в другой вектор (`sibling_same_parent`: `parent[sibling[i]]`; `reachable_from_root`/`propagation_converges`: обе через `propagate<...>`, который делает `v[link[i]]`) — если `bounds` не пройдена хотя бы для одного узла, выполнение этих трёх было бы UB (чтение за пределами вектора), не просто бессмысленным результатом.

- **Ступень 1** (всегда; только сравнение значений, индексирования нет): `bounds`, `parent_order`, `sibling_order`, `single_root`, `sibling_injective`. Собираются все найденные нарушения.
- **Ступень 2** (только если `bounds` на ступени 1 чиста; иначе пропускается целиком — эти проверки не попадают в `violations` вообще, не как «нарушены», а как «не выполнялись»): `sibling_same_parent`, `reachable_from_root`, `propagation_converges`.

**`reachable_from_root`/`propagation_converges` — одна пропагация, не две.** Оба — интерпретации результата одного вызова `propagate_down_iterative(seed={root})` из [[05-mask-propagation-strategies]] (`std::expected<BitMask, ConvergenceError>`):
- `has_value() == false` → нарушение `propagation_converges`; `reachable_from_root` в этом случае не выполняется вовсе (маска недостроена), не попадает в `violations`;
- `has_value() == true` → проверяются все `n` бит; неустановленные — нарушение `reachable_from_root`, их id — в `nodes`.

**Сигнатура:** `validate_invariants(const Tree<T,IndexT>&) -> ValidationResult` — свободная read-only функция (02), **не** `std::expected`: несходимость здесь не сигнал ошибки вызывающему коду (в отличие от `depth`/`rank`/`size`/`deleteSubtree`), а обычный диагностируемый случай — сама цель функции; уже зафиксировано в CLI-схеме («провал валидации — не ошибка, обычный успешный ответ, `exit 0`»).

Контекст: [[01-propagate-primitive]], [[02-tree-encapsulation]], [[05-mask-propagation-strategies]].
