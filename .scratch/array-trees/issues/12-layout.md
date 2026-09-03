Type: grilling
Blocked by: 14

## Question

Как две C++ реализации сосуществуют внутри `cxx/traaxx/` (и как их видит `cxx/tests/t_traaxx/`), и как две Go реализации сосуществуют внутри `go/` — раздельные CMake-таргеты/namespace'ы, раздельные Go-модули/пакеты, отдельные поддиректории?

`[[07-toolchain-reality]]` закрыл фактическую часть: Go-сторона устоялась (портируемый пакет `simd`, Go 1.27, обязательный `GOEXPERIMENT=simd`, нестабильный API — принятый риск), а для C++ вместо мёртвого «boost.simd» остаются живые кандидаты (`std::experimental::simd` / xsimd / Highway) без финального выбора. Именно этот выбор — `[[14-cxx-simd-library-choice]]` — определяет, что подключать в `cxx/cmake/files/find_deps.cmake`/`find_pkgs.cmake` (header-only xsimd vs готовый системный `std::experimental::simd` vs собираемый Highway) и как это отражается в раскладке `cxx/traaxx/`.
