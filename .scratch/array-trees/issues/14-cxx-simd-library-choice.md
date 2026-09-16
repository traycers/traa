Type: grilling
Blocked by: 07
Status: resolved

## Question

`[[07-toolchain-reality]]` подтвердил: Boost.SIMD мёртв (никогда не входил в Boost), формулировку «boost.simd» для второй C++-реализации нужно заменить на конкретную живую библиотеку. Кандидаты, каждый подтверждён по первоисточнику в `research/07-toolchain-reality.md`:

- **`std::experimental::simd`** (Parallelism TS2) — уже физически есть в системном GCC этого окружения (`/usr/include/c++/15/experimental/simd`), ничего доустанавливать не нужно; API — TS-версии, не финальный C++26 `std::simd` (в GCC не полностью реализован, требует `-std=c++26`, тогда как `cxx/CMakeLists.txt` собирается под C++23).
- **xsimd** — header-only, BSD-3-Clause, зрелая (используется Firefox/Apache Arrow/Pandas), нужно явно завести в `Dockerfile.dev`+`CMakeLists.txt`/`PROJECT_DEPS`.
- **Google Highway** — самая широкая платформенная поддержка и masked/portable API, но не header-only — требует полноценной сборки и линковки, более тяжёлая интеграция для этого проекта.

Какую библиотеку зафиксировать как вторую C++-реализацию (взамен «boost.simd»)?

## Answer

**xsimd.** Проверка по первоисточникам (исходники xtensor-stack/xsimd и google/highway) подтвердила: обе библиотеки функционально закрывают ключевые операции — gather (`batch::gather`, аппаратно на AVX2/AVX512F) и маскированный выбор (`xsimd::select`), нужные для pointer doubling (§5.4, `Ak[A]`) и batch LCA (§10.5, `where(...)`). У Highway есть готовый one-shot `MaskedGatherIndexOr`, у xsimd то же самое собирается в два вызова (`gather` + `select`) — функционально почти равноценно на типичном x86-64 с AVX2 (оба получают аппаратное ускорение; без AVX2/SVE обе одинаково падают на скалярный fallback).

Решающий фактор — **простота API и интеграции**: xsimd — header-only, типы-значения с перегруженными операторами, минимум церемоний. Highway — не header-only, вся библиотека построена вокруг multi-target runtime-диспетчеризации (`foreach_target.h`, explicit descriptor-теги, `HWY_ATTR`) — оправдано для рантайм-поддержки многих архитектур одним бинарём, но избыточно для конкретного известного тулчейна (`cxx/Dockerfile.dev`, x86-64).

**Google Highway устанавливается в `cxx/Dockerfile.dev` тоже**, но только для экспериментов — не входит в формальное сравнение 4 реализаций (parallel STL / xsimd / Go stdlib / Go simd остаются четырьмя, не пятью).

Контекст: `[[02-operation-set]]`, `[[07-toolchain-reality]]`, research: `research/07-toolchain-reality.md`.
