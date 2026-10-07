# EcoHeapSort — задание № 60

Реализована пирамидальная сортировка через публичный `IEcoSortAndSearch`:
`Sort(me, base, count, size, compare)`. IID интерфейсов и CID компонента сохранены.
Прежний `IEcoHeapSort` доступен на том же объекте. Оба интерфейса имеют общий
счётчик ссылок и возвращают один указатель `IEcoUnknown`.

Алгоритм строит максимальную кучу, переносит максимум в конец массива и
итеративно восстанавливает кучу. Число сравнений — O(n log n), дополнительная
память — O(1); обмен элементов выполняется побайтово без выделения памяти.
При фиксированном размере элемента время — O(n log n); побайтовая обработка
элемента требует O(size). Вызовов libc и сторонних зависимостей в коде нет.

При `count < 2` массив и компаратор не используются. Для остальных вызовов
нулевые `base`/`compare`, нулевой `size` и переполнение `count * size` приводят
к возврату без изменений. Индексы детей вычисляются только для внутренних
узлов кучи, что предотвращает их переполнение.

Сборка и запуск Linux x86_64 (Eco.Framework по умолчанию находится рядом):

```sh
make -C AssemblyFiles/Linux/gcc_v132 -f MakefileExe TARGET=1 ARCH=x86_64 DEBUG=0
./BuildFiles/Linux/x86_64/StaticRelease/EcoHeapSort
make -C AssemblyFiles/Linux/gcc_v132 -f MakefileExe TARGET=1 ARCH=x86_64 DEBUG=1
./BuildFiles/Linux/x86_64/StaticDebug/EcoHeapSort
make -C AssemblyFiles/Linux/gcc_v132 -f Makefile TARGET=2 ARCH=x86_64 DEBUG=0
make -C AssemblyFiles/Linux/gcc_v132 -f Makefile TARGET=2 ARCH=x86_64 DEBUG=1
```

Тестовый клиент проверяет граничные параметры, порядок и побайтовую сохранность
элементов типов `int`, `long`, `float`, `double`, `long double` и структур.
Проверяются повторы, отрицательные и предельные целые значения, исходно
упорядоченные массивы, обратный порядок и сортировка по убыванию. Дополнительно
перебираются все 3280 массивов длины 0–7 над {-1, 0, 1} в обоих направлениях,
размеры около степеней двойки и пять массивов из 1024 элементов с проверкой
границ массива и ограничения числа сравнений. Проверяются получение обоих
интерфейсов через `QueryComponent`, идентичность и время жизни объекта.
Ошибка проверки возвращает ненулевой результат `EcoMain`; успешный клиент
не печатает сообщения.

Проверка внешних ссылок: `nm -u` для объектных файлов компонента и клиента
в каталогах `BuildFiles/Linux/x86_64/*/obj`. Динамическая библиотека собирается
с `-nostdlib`: `readelf -d` не должен показывать зависимости `NEEDED`, а
`nm -D --undefined-only` — внешние функции. Готовые библиотеки Eco.Framework
и системный запуск клиента остаются частью существующего окружения сборки.

Ограничения контракта: сортировка не гарантирует стабильность; вызывающий код
отвечает за доступность и выравнивание массива и согласованность компаратора
(включая правила порядка для NaN, если такие значения используются).

# Project Structure

This document describes the organization of folders and files within the project repository.

## Overview

The project is organized using a strict folder naming convention, where each folder contains files of a specific type and purpose. All folder names use `PascalCase` styling and end with the `Files` suffix.

```
/
├── AssemblyFiles
├── BuildFiles
├── DependenciesFiles
├── DesignFiles
├── HeaderFiles
├── SharedFiles
├── SourceFiles
└── UnitTestFiles
```

---

## Folder Descriptions

### `AssemblyFiles`

Contains scripts, configuration files, and tools necessary for the project's build process (e.g., `Makefile`, `pom.xml`, files for CI/CD, or automation scripts).

### `BuildFiles`

Contains the compiled output files of the project (executables, DLL libraries, `.jar` files, or other build artifacts), ready for deployment or execution.

### `DependenciesFiles`

Contains third-party libraries, frameworks, and external dependencies of the project that are not part of the core source code. These can be compiled libraries or source files of external packages.

### `DesignFiles`

Contains all artifacts related to the design and planning of the project: technical specifications (TS), UML diagrams, user interface mockups (UI/UX), database schemas, and other project documentation.

### `HeaderFiles`

Contains **private** header files (`.h`, `.hpp`) that describe the internal class structures and implementation details of the ACOM/COM components. These files are not intended for public consumption by external clients.

### `SharedFiles`

Contains **public** shared ACOM/COM interface header files (`.h`, `.hpp`) and Interface Definition Language source files (`.idl` files). These files provide external clients with the necessary information to interact with your component.

### `SourceFiles`

The core folder of the project. Contains the **main source code** of the application (`.c`, `.cpp`, `.py`, `.js`, `.java`, etc.).

### `UnitTestFiles`

Contains unit test files, integration tests, and other scripts for automated testing of the code located within `SourceFiles`.
