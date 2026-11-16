# Conan integration (Components system)

Simplified [Conan 2.x](https://conan.io/) integration for projects built on the
Components system. It is provided as reusable CMake functions, analogous to the
existing `COMPONENTS_CCR_*` configure functions, so any of your projects can set
Conan up in a couple of lines.

## What was added

| Location | Purpose |
| --- | --- |
| `ComponentsSystem/Components-CMake/functions/conan.txt` | The Conan module: `COMPONENTS_CCR_CONAN_*` functions (self-managing). |
| `ComponentsSystem/Components-CMake/functions/CMakeLists.txt` | Includes `conan.txt`. |
| `ComponentsSystem/Components-Core/CMakeLists.txt` | Conan **consumption** helpers for components (`COMPONENTS_CONAN_*`); never installs Conan itself. |
| `ComponentsSystem/Components-Database/CMakeLists.txt` | Example consumer wiring: prefers a Conan `SQLite3` when available, otherwise keeps the system `sqlite3`. |
| `conanfile.txt` | Project dependency definition (Boost only). |
| `CMakeLists.txt` | Calls `COMPONENTS_CCR_CONAN_SETUP(...)` and resolves Boost through the module. |

> Conan is **not** added as a git submodule. Unlike GoogleTest, Conan is a package
> manager (a standalone CLI), not a CMake source subproject — there is nothing to
> `add_subdirectory()`. The installed `conan` executable is used as-is.

## How it works

`COMPONENTS_CCR_CONAN_SETUP(...)` is a no-op unless it can and should do work:

1. If Conan was already configured (a previous call, the CMake-Conan provider, or
   Qt Creator's package-manager auto-setup), it only re-applies the generated
   find paths and returns.
2. If the `conanfile` or the `conan` executable is missing, it prints a status
   message and returns (no failure unless `REQUIRED` is passed).
3. Otherwise it detects a default Conan profile if needed, runs `conan install`,
   reads the generators folder from Conan's JSON output, and prepends it to
   `CMAKE_PREFIX_PATH` / `CMAKE_MODULE_PATH`.

Because the generators folder is injected into the find paths, ordinary
`find_package()` calls work without a toolchain file.

## Using it in a project

```cmake
add_subdirectory(ComponentsSystem)

COMPONENTS_CCR_CONAN_SETUP(
    CONANFILE "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt"
    BUILD     "missing"            # optional, default "missing"
    # PROFILE "default"            # optional
    # OPTIONS "boost/*:shared=False"
    # SETTINGS "compiler.cppstd=17"
    # REQUIRED                     # fail the configure on error
)

COMPONENTS_CCR_CONAN_FIND_LIBRARY(boost_system)   # -> Boost::system
COMPONENTS_CCR_CONAN_FIND_LIBRARY(boost_thread)   # -> Boost::thread
COMPONENTS_CCR_CONAN_REQUIRE(SQLite3)             # -> SQLite::SQLite3
```

### Functions

- `COMPONENTS_CCR_CONAN_SETUP([...])` — self-managing bootstrap (see above).
- `COMPONENTS_CCR_CONAN_FIND_LIBRARY(<alias> [components...])` — `find_package`
  wrapper; when Conan is active it also maps `boost_<component>` to the Conan
  `Boost` package and bridges the legacy raw link names.
- `COMPONENTS_CCR_CONAN_REQUIRE(<package> [components...])` — alias of the above.
- `COMPONENTS_CCR_CONAN_LINK(<target> <libs...>)` — link helper.
- `COMPONENTS_CCR_CONAN_IS_CONFIGURED(<out_var>)` / `..._AVAILABLE(<out_var>)` —
  query whether Conan is active.

## Using it inside a component (`Components-*`)

Components never install Conan. They consume a configuration provided by the
hosting project (or an IDE) and build against system libraries when none is
present:

```cmake
COMPONENTS_CONAN_IS_AVAILABLE(_conan_on)
if (_conan_on)
    COMPONENTS_CONAN_REQUIRE(SQLite3)
endif()
```

- `COMPONENTS_CONAN_IS_AVAILABLE(<out_var>)`
- `COMPONENTS_CONAN_REQUIRE(<package> [components...])`
- `COMPONENTS_CONAN_LINK_LIBRARY(<target> <libs...>)`

## Workflows

- **Qt Creator**: Qt Creator's package-manager auto-setup (`CMAKE_PROJECT_INCLUDE_BEFORE`)
  detects `conanfile.txt` and runs Conan automatically. `COMPONENTS_CCR_CONAN_SETUP`
  detects this and stays out of the way. To disable it, set
  `QT_CREATOR_SKIP_CONAN_SETUP=ON` or `COMPONENTS_CCR_CONAN_SKIP`-style cache vars.
- **Command line / CI**:

  ```sh
  cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug
  ```

  The module runs `conan install` during configure. Generated files land in
  `build/Debug/conan/` (git-ignored via the `build/` ignore rule).

## Dependency scope

Only **Boost** is managed by Conan for now (see `conanfile.txt`). Everything else
(oneTBB, websocketpp, bitsery, drogon, gtest, Qt, OpenSSL, libsodium, SQLite3)
remains vendored / system-provided, as before. Extend `[requires]` in
`conanfile.txt` to move more dependencies to Conan.

### Boost naming note

The system/Debian Boost provides per-component config files
(`boost_system-config.cmake`), so the existing `find_package(boost_system)` calls
work today. Conan's `boost` recipe instead generates `BoostConfig.cmake` with
targets such as `Boost::system`. `COMPONENTS_CCR_CONAN_FIND_LIBRARY` bridges both:
when Conan is active it resolves `boost_*` through `Boost` and creates `boost_*`
interface targets so existing link names keep working.
