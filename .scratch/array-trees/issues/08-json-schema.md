Type: grilling
Status: resolved

## Question

`[[04-corpus-golden-format]]` установил: корпус — JSON, один канонический генератор, golden-файлы на пару (дерево × операция). `[[01-representation-model]]` установил: `data` — массив одного generic-параметра `T` (шаблонное дерево, а не закрытый доменный список). Не зафиксирована точная схема:

- **какой конкретный `T` (или несколько `T`) инстанцируется в языконейтральном корпусе.** Раз дерево шаблонное, а корпус — один и тот же JSON для всех 4 реализаций (2×C++ template/generic, 2×Go generic), нужно зафиксировать: один нейтральный `T` для всего корпуса (например, `uint32` — простейший, однозначно маппится на любой язык) или несколько наборов корпуса под разные `T` (например, ещё и `T = строка`/`T = произвольная маленькая структура`), чтобы протестировать, что шаблонность действительно работает не только для тривиального числового типа;
- имена/типы полей файла дерева (`parent`, `sibling`, `data`, метаданные формы/размера/ожидаемой валидности из `[[03-test-corpus]]`);
- как кодировать выбранный `T` в JSON (простое значение, если `T` примитивен; объект с полями, если `T` — структура);
- форма golden-результата для каждой категории операций из `[[02-operation-set]]` (вектор чисел, перестановка + переведённые `parent′`/`sibling′`, пара id для LCA, структура отчёта об инвариантах §13 — что именно провалилось);
- как для перемешанной перестановки (форма 6, `[[03-test-corpus]]`) golden различает ожидаемый результат однопроходной стратегии (заведомо неверный) и итеративной/pointer-doubling (верный) — одна операция, две ожидаемые записи.

## Answer

Терминология зафиксирована в `CONTEXT.md` (`Shape`, `Corpus file`, `Payload type`, `Case`, `Golden file`).

### Corpus file — `corpus/<shape>.json`

```json
{
  "shape": "wide-shallow",
  "nodes_count": 13,
  "expected_valid": true,
  "payload_type": {"kind": "primitive", "value": "uint32"},
  "parent":  [0,0,1,1,2,3,4,5,6,4,6,9,7],
  "sibling": [0,1,2,2,4,5,6,7,8,6,8,11,12],
  "data":    [...]
}
```

- `shape` — enum из семи форм `[[03-test-corpus]]` (`example`, `wide-shallow`, `degenerate-chain`, `balanced-wide`, `star`, `shuffled-permutation`, `corrupted-invariants`) плюс отдельная восьмая — канарейка на структурный `payload_type`, только для дымового теста генеричности (не полный набор форм).
- Все имена полей — **snake_case** по всей схеме (корпус, кейсы, golden).
- Числа — обычные JSON-числа (не строки, не base64); `nodes_count` далёк от `2^53`, запас с большим кратным.
- `payload_type.value` для `kind: "primitive"` — строка с именем типа (`"uint32"`); для `kind: "struct"` — объект `{имя_поля: тип}`, где тип каждого поля — либо строка-примитив, либо вложенный объект-структура (рекурсивно, без повторной обёртки `kind`/`value` на вложенных уровнях). Канарейка использует `{"kind": "struct", "value": {"tag": "string", "weight": "uint32"}}`.

### Cases file — `cases/<shape>.json`

Список именованных вызовов операций из `[[02-operation-set]]` с аргументами:

```json
[
  {"id": "lca_naive_8_11", "op": "lca_naive", "args": {"a": 8, "b": 11}},
  {"id": "propagate_down_single_pass_seed2", "op": "propagate_down_single_pass", "args": {"seed": [2]}},
  {"id": "propagate_down_iterative_seed2", "op": "propagate_down_iterative", "args": {"seed": [2]}},
  {"id": "mutate_move_4_to_7", "op": "mutate_move", "args": {"x": 4, "q": 7}}
]
```

- Три propagation-стратегии из `[[02-operation-set]]` — три разных `op`, каждая со своим case/golden. На `shuffled-permutation` `propagate_*_single_pass` даёт *заведомо неверный* golden-результат — это ожидаемое поведение, не ошибка golden.
- На `corrupted-invariants` заводится только один case — `validate_invariants`; остальные операции на заведомо невалидном дереве не тестируются (поведение не определено теорией).

### Golden file — `golden/<shape>/<case_id>.json`

Всегда `{"result": <value>}`, форма `<value>` зависит от категории операции (однозначно следует из `op` в соответствующем case, отдельного тега типа в golden не нужно):

| Категория | Форма `result` |
|---|---|
| Производные вектора (`depth`/`rank`/`size`/`nsibling`/`lastchild`/`isleaf`/`pos`) | плоский массив длины `nodes_count` (числа или булевы); узлы без значения (лист без `lastchild` и т.п.) кодируются тем же соглашением, что и `sibling[i]==i`/`parent[i]==i` — значением id самого узла, не `null` |
| Propagation-маска (все 3 стратегии) | плоский массив булевых длины `nodes_count` |
| DFS/BFS-переупорядочивание | объект `{"new_to_old", "old_to_new", "parent", "sibling"}` — все четыре тем же snake_case, без штриха в имени (штрих в теории — это просто «после перевода») |
| LCA одиночный (naive/binary lifting) | число — id узла-ответа |
| LCA пакетный | массив чисел, тот же порядок, что пары в `args` |
| LCA множества | число |
| Мутации (`append`/`deleteLeaf`/`deleteSubtree`/`splice`/`move`) | **полный результирующий снимок**: `{"parent", "sibling", "data", "nodes_count"}` — единая конвенция для всех пяти видов мутаций, не delta, даже для дешёвых (`append`, `deleteLeaf`) |
| `validate_invariants` | `{"valid": bool, "violations": [{"check": <enum>, "nodes": [...]}]}`; `check` — одна из семи строк по проверкам §13 (`bounds`, `parent_order`/`sibling_order`, `single_root`, `sibling_same_parent`, `sibling_injective`, `reachable_from_root`, `propagation_converges`) |

### T-инстанциация корпуса

Основной корпус (все 7 форм) — `T = uint32`. Отдельно — одна маленькая канарейка (2–3 узла) со структурным `T` (`{"tag": "string", "weight": "uint32"}`), только чтобы проверить перенос нетривиального payload во всех 4 реализациях; не полный набор форм под неё.

Контекст: `[[01-representation-model]]`, `[[02-operation-set]]`, `[[03-test-corpus]]`, `[[04-corpus-golden-format]]`.
