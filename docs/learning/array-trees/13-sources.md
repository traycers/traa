# 17. Источники

## 17.1. Первоисточники Aaron W. Hsu

- **Aaron W. Hsu. *A data parallel compiler hosted on the GPU*.** PhD dissertation, Indiana University, School of Informatics, Computing, and Engineering, ноябрь 2019.
  Постоянная ссылка: [hdl.handle.net/2022/24749](https://hdl.handle.net/2022/24749) · [прямой PDF](https://scholarworks.iu.edu/dspace/bitstreams/dcbd5240-8454-4533-bc0c-ac3ee7628b8e/download) (29 МБ) · [издание на Leanpub](https://leanpub.com/arcfide_dissertation).
  Полное изложение метода: представление AST векторами, преобразования деревьев без обхода указателей, DFS-порядок и вектор `parent` (§3.5–§3.7), propagation вместо рекурсии (§6). Основа всего этого конспекта. Отдельный подробный разбор диссертации целиком — [`../hsu-dissertation/`](../hsu-dissertation/index.md).

- **Aaron W. Hsu. *The key to a data parallel compiler*.** ARRAY'16 — 3rd ACM SIGPLAN International Workshop on Libraries, Languages, and Compilers for Array Programming, Санта-Барбара, 14 июня 2016, с. 32–40. DOI [10.1145/2935323.2935331](https://dl.acm.org/doi/10.1145/2935323.2935331).
  Более короткий и ранний источник, и именно тот, где введён термин **node coordinate matrix** ([«02. Path coordinate matrix»](02-path-coordinate-matrix.md)) и показан оператор Key (`⌸`, [«12. Словарь APL ↔ Go»](12-apl-go-glossary.md)) как способ выполнять произвольные вычисления над поддеревьями чисто массивными средствами.

- **Co-dfns** — компилятор APL, реализующий метод: [github.com/Co-dfns/Co-dfns](https://github.com/Co-dfns/Co-dfns).
  Справочные страницы APL Wiki: [Co-dfns](https://aplwiki.com/wiki/Co-dfns), [Aaron Hsu](https://aplwiki.com/wiki/Aaron_Hsu).

## 17.2. Смежные первоисточники

Техники, которые этот конспект использует как стандартные, но которые к Hsu не относятся:

- **Guy E. Blelloch. *Prefix Sums and Their Applications*.** CMU-CS-90-190, Carnegie Mellon University, 1990. [PDF](https://www.cs.cmu.edu/~guyb/papers/Ble93.pdf).
  Сегментированный scan ([«07. DFS-порядок»](07-dfs-order.md) §8.2) и префиксная сумма как основа компактификации ([«10. Изменение дерева»](10-mutations.md) §11.5).

- **Michael A. Bender, Martín Farach-Colton. *The LCA Problem Revisited*.** LATIN 2000, LNCS 1776, с. 88–94. [Springer](https://link.springer.com/chapter/10.1007/10719839_9).
  Euler tour + RMQ — предельный по скорости вариант LCA, упомянутый в [«09. LCA»](09-lca.md) §10.7.

- **Joseph JáJá. *An Introduction to Parallel Algorithms*.** Addison-Wesley, 1992.
  Классическое изложение pointer jumping ([«04. Распространение маски»](04-mask-propagation.md) §5.4) и параллельных обходов; свободной онлайн-версии нет.

## 17.3. Видео

**Деревья Hsu — доклады автора.**

- [**High-performance Tree Wrangling, the APL Way**](https://www.youtube.com/watch?v=hzPd3umu78g) // Dyalog '18, канал Dyalog User Meetings ([слайды PDF](https://www.dyalog.com/uploads/conference/dyalog18/presentations/U19_Tree_Wrangling_the_APL_Way.pdf)).
  Самый прямой доклад по теме этого конспекта: подход к работе с деревьями, дающий сокращение кода, производительность и параллелизуемость. Если смотреть что-то одно — это.
- **Parallel-by-construction Tree Manipulation with APL**, LambdaConf 2018 — [Часть 1](https://www.youtube.com/watch?v=lc4IjR1iJTg), [Часть 2](https://www.youtube.com/watch?v=X5_5MtOYNos).
  Формат воркшопа, а не доклада: то же самое, но длиннее и с разбором по шагам.
- [**Designing your Data: The Bread & Butter of APL Performance**](https://www.youtube.com/watch?v=ozlxUmdYsHA) // FnConf 2025.
  Свежий доклад про выбор представления данных — тот самый вопрос, который в [«02. Path coordinate matrix»](02-path-coordinate-matrix.md) §3.4–§3.6 решается в пользу `parent` + `sibling`.
- [**Co-dfns Report: GPU Performance, Workflow and Usability**](https://www.youtube.com/watch?v=xZru2zXSzMA) // Dyalog '16 и [**revolutionary GPU data compiler using APL**](https://www.youtube.com/watch?v=2uGg2n7BGZs) // ConfEngine.
  Контекст вокруг метода: зачем компилятор на GPU и что из этого вышло.

**Gather, scatter и сопутствующие примитивы.** Курс Udacity CS344 *Intro to Parallel Programming* (John Owens, UC Davis / David Luebke, NVIDIA) — короткие ролики, разбитые по темам:

- [**Communication Patterns**](https://www.youtube.com/watch?v=HPxjXSMIGc0) — обзорный: map, gather, scatter, stencil, transpose. Ключевая формулировка та же, что в [«06. Перестановки»](06-permutations.md) §7.1: gather — «много входов в один выход», scatter — «один вход во много выходов».
- [**Map and Gather**](https://www.youtube.com/watch?v=eZxbwnABmTM) — gather как выборка по вектору индексов (§7.1, `Ak[A]` в [«09. LCA»](09-lca.md) §10.5).
- [**Scatter To Gather Transformation**](https://www.youtube.com/watch?v=feGeUehFtQ4) — почему scatter стараются переписать в gather. Прямо про конфликт записи из [«04. Распространение маски»](04-mask-propagation.md) §5.2 и [«06. Перестановки»](06-permutations.md) §7.1.
- [**Blelloch Scan**](https://www.youtube.com/watch?v=mmYv3Haj6uc) и [**Hillis Steele vs Blelloch Scan**](https://www.youtube.com/watch?v=_5sM-4ODXaA) — префиксные суммы, на которых стоят [«07. DFS-порядок»](07-dfs-order.md) §8.2 (сегментированный scan) и [«10. Изменение дерева»](10-mutations.md) §11.5 (компактификация).
- [Плейлист курса целиком](https://www.youtube.com/playlist?list=PLAwxTw4SYaPnFKojVQrmyOGFCqHTxfdv2).

Собственное представление дерева и решения проекта traa (независимые от материала этого конспекта) зафиксированы в `.scratch/array-trees/` в корне этого репозитория.
