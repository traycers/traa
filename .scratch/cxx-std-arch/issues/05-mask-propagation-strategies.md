Type: grilling
Status: resolved
Blocked by: 01

## Question

`docs/learning/array-trees/04-mask-propagation.md` и `05-propagation-pattern.md` фиксируют три отдельные стратегии распространения маски, каждая — свой `op` в CLI-схеме (`[[../../array-trees/issues/09-cli-schema-details]]`: `propagate_down_iterative`/`propagate_down_single_pass`/`propagate_down_pointer_doubling`, аналогично для `up`):

- §5.1/§5.2 — итеративная до сходимости (вниз — gather с `or`, вверх — scatter с редукцией `or`);
- §5.3 — однопроходная топологическая (последовательная, требует `parent[i] <= i`/`sibling[i] <= i`, `O(n)` за один проход, без цикла до сходимости);
- §5.4 — pointer doubling (`O(log d)`, поверх векторов предков `A1, A2, A4, ...`).

Нужно решить архитектуру этих трёх как трёх сущностей: как каждая соотносится с общим propagate-примитивом ([[01-propagate-primitive]]) — итеративная явно его прямое применение (`f=or`), но однопроходная концептуально другой алгоритм (не итерация до сходимости, а один детерминированный проход) и pointer doubling нуждается в таблице предков ([[06-ancestor-table]]). Также нужно зафиксировать представление самой маски (`std::vector<bool>`, `std::vector<std::uint8_t>`, битовый набор?) и сигнатуры вниз/вверх (маска поддерева — gather вниз; маска предков — scatter вверх, §5.1 vs §5.2 — это одна операция с параметром направления, или две разные функции?).

## Answer

**Отношение трёх стратегий к `propagate<...>` ([[01-propagate-primitive]]):** только итеративная (§5.1/§5.2) реально построена поверх примитива — link=`parent`, `f=or`, оба направления (уже отмечено в 01). Однопроходная (§5.3) и pointer doubling (§5.4) — **не** через `propagate<...>`: однопроходная — не итерация до сходимости, а один детерминированный проход с последовательной зависимостью внутри прохода; pointer doubling — число итераций (`K+1` уровней) фиксировано заранее из [[06-ancestor-table]], не детектируется сравнением состояний между проходами, как это делает `propagate<...>`. Обе — самостоятельный код, pointer doubling использует `AncestorTable::level(k)` в собственном цикле.

CLI-схема (`array-trees/08-json-schema.md`) уже называет `op` раздельно по направлению (`propagate_down_single_pass`, `propagate_down_iterative`, аналогично `_up_*` и `_pointer_doubling`) — шесть разных `op`, значит шесть публичных функций, каждая в своём файле по [[04-file-layout]]: `propagate_down_iterative`/`propagate_up_iterative` (обе делегируют в `propagate<...>` с разной семантикой направления), `propagate_down_single_pass`/`propagate_up_single_pass`, `propagate_down_pointer_doubling`/`propagate_up_pointer_doubling`.

**Представление маски:** упакованный битовый набор на словах `std::uint64_t` (`⌈n/64⌉` слов), не байт на узел и не `std::vector<bool>`. На amd64 это прямое попадание в нативную атомарную инструкцию (`lock or`): scatter-с-редукцией вверх (§5.2) пишет через `std::atomic_ref<std::uint64_t>::fetch_or` на слово целиком — одна RMW-инструкция, без CAS-цикла. Экономия памяти в 8 раз против байта на узел оправдана явным вниманием карты (`array-trees`) к `peak_rss_delta_bytes`/`parallel STL`. `std::vector<bool>`-специализация отдельно исключена: битовая упаковка с proxy-ссылками, не даёт контролируемой атомарности на уровне слова.

Оформлено как отдельный **четвёртый переиспользуемый тип `BitMask`** (по аналогии с `AncestorTable`, не как `gather`/`remap_index_vector` — здесь есть инвариант «число слов = `⌈n/64⌉`», который проще держать за интерфейсом типа): `get(IndexT) -> bool`, `set(IndexT)`, `atomic_or(IndexT)`, `size() -> IndexT`. Используется во всех шести функциях этого тикета и потенциально в `reachable_from_root` (§13, [[13-invariant-validation]]) — это тоже по сути распространение маски от корня.

Контекст: [[01-propagate-primitive]], [[04-file-layout]], [[06-ancestor-table]], [[13-invariant-validation]].
