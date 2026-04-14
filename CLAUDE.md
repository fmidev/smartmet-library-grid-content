# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

`smartmet-library-grid-content` is a C++ library that defines three service APIs (ContentServer, DataServer, QueryServer) plus session/user management for the SmartMet Server grid data support. Each API has an abstract `ServiceInterface` with multiple backend implementations (Redis, in-memory cache, PostgreSQL, CORBA client, HTTP client). The library produces `libsmartmet-grid-content.so`.

## Build Commands

```bash
make                    # Build (generates CORBA stubs first if enabled)
make CORBA=disabled     # Build without CORBA support
make clean              # Clean build artifacts
make install            # Install to PREFIX=/usr
make rpm                # Build RPM package
make format             # Run clang-format
```

There is no test suite in this repository. Testing is done via integration with SmartMet Server components.

## Dependencies

**SmartMet libraries:** grid-files, spine, macgyver, gis

**External:** hiredis (Redis), libpq (PostgreSQL), libcurl (HTTP client), omniORB (CORBA), luajit, gdal, ICU, Boost

## Code Formatting

The `.clang-format` file sets `DisableFormat: true` — clang-format is effectively disabled for this project. Preserve existing formatting style when editing.

## Architecture

### ServiceInterface Pattern (Template Method)

All five service APIs follow the same pattern. Each `ServiceInterface` declares public/protected method pairs:

```
public:  int getProducerInfoById(sessionId, producerId, &producerInfo)
protected: int _getProducerInfoById(sessionId, producerId, &producerInfo)
```

The public method handles cross-cutting concerns (logging, timing, observation) then delegates to the protected `_`-prefixed method. Backend implementations override only the protected methods. This keeps monitoring behavior uniform across all backends.

The `Implementation` enum at `src/contentServer/definition/ServiceInterface.h:34` tracks which backend is active: Interface, Redis, Cache, CorbaClient, HttpClient, Memory, Postgres.

### Three Service Layers

**ContentServer** (`src/contentServer/`) — Metadata registry. Tracks producers, generations, geometries, files, and content records. ~100+ service methods. Has the most backend implementations:
- `cache/` — In-memory with event-driven sync from a master source (typically Redis)
- `redis/` — Persistent storage via hiredis, supports primary/secondary instances
- `postgresql/` — Database backend via libpq
- `memory/` — Lightweight in-memory, no event monitoring
- `merge/` — Combines multiple service implementations
- `corba/client/` and `http/client/` — Remote access proxies

**DataServer** (`src/dataServer/`) — Grid data access. Fetches values from grid files by point, polygon, rectangle, with interpolation support. Uses ContentServer to locate files, then memory-maps them for fast access.
- `implementation/` — Main implementation that memory-maps grid files
- `corba/client/` — Remote access

**QueryServer** (`src/queryServer/`) — High-level query execution. Combines ContentServer (find files) and DataServer (read data) to answer queries with parameter lists, time ranges, and coordinate specifications.
- `implementation/` — Main implementation with Lua-based function evaluation
- `corba/client/` — Remote access

### Supporting Subsystems

**Functions** (`src/functions/`) — ~54 built-in mathematical/meteorological functions (arithmetic, statistics, wind calculations, feelsLike, etc.). Base class `Function` with three execution call types for different arity patterns. `FunctionCollection` is the registry/factory.

**Lua** (`src/lua/`) — LuaJIT integration for dynamic function definition. `LuaFileCollection` manages scripts with hot-reload. 20 parallel execution handles (`NUM_OF_LUA_HANDLES`) for concurrent calls.

**Session/User Management** (`src/sessionManagement/`, `src/userManagement/`) — Authentication and session lifecycle, following the same ServiceInterface pattern.

### CORBA Layer (Optional)

CORBA support is controlled by `CORBA=enabled|disabled` in the Makefile. When enabled:
- IDL files in `src/*/corba/idl/` define service interfaces for network access
- `stubs/` directories contain auto-generated code (built by `make create_stubs`)
- `convert/` directories handle C++ ↔ CORBA type conversion
- `python/` directory generates Python bindings via omniidl

### Network Access Pattern

Services can be accessed locally (direct implementation) or remotely (CORBA/HTTP client proxies). A typical distributed setup:

```
App → CacheImpl → ClientImpl → [CORBA|HTTP] → Remote ServiceImpl
```

All implementations inherit from the same `ServiceInterface`, so the calling code is unaware whether it's using a local or remote backend.

## Key Types (Namespace `T::`)

Type aliases are defined via `grid-files/grid/Typedefs.h`. Commonly used: `SessionId`, `ProducerId`, `GenerationId`, `GeometryId`, `FileId`, `MessageIndex`, `ParamKeyType`, `ParamLevel`, `ParamLevelId`, `ForecastType`, `ForecastNumber`, `SourceId`.

## CI

CircleCI builds RPMs on RHEL 8 and RHEL 10 Docker images using `ci-build deps` and `ci-build rpm`. No test step — CI validates that the library compiles and packages cleanly.
