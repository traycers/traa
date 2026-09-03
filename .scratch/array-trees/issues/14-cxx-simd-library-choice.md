Type: grilling
Blocked by: 07

## Question

`[[07-toolchain-reality]]` подтвердил: Boost.SIMD мёртв (никогда не входил в Boost), формулировку «boost.simd» для второй C++-реализации нужно заменить на конкретную живую библиотеку. Кандидаты, каждый подтверждён по первоисточнику в `research/07-toolchain-reality.md`:

- **`std::experimental::simd`** (Parallelism TS2) — уже физически есть в системном GCC этого окружения (`/usr/include/c++/15/experimental/simd`), ничего доустанавливать не нужно; API — TS-версии, не финальный C++26 `std::simd` (в GCC не полностью реализован, требует `-std=c++26`, тогда как `cxx/CMakeLists.txt` собирается под C++23).
- **xsimd** — header-only, BSD-3-Clause, зрелая (используется Firefox/Apache Arrow/Pandas), нужно явно завести в `Dockerfile.dev`+`CMakeLists.txt`/`PROJECT_DEPS`.
- **Google Highway** — самая широкая платформенная поддержка и masked/portable API, но не header-only — требует полноценной сборки и линковки, более тяжёлая интеграция для этого проекта.

Какую библиотеку зафиксировать как вторую C++-реализацию (взамен «boost.simd»)?
