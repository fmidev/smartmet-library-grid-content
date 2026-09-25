# grid-content developer guide

This guide is for developers who change `smartmet-library-grid-content` or build on it.
It covers the architecture, the main code paths, and the conventions you need before
editing the code.

grid-content sits on top of [grid-files](https://github.com/fmidev/smartmet-library-grid-files),
which reads the actual GRIB, NetCDF, QueryData and GeoTIFF files. Read the grid-files
[developer guide](https://github.com/fmidev/smartmet-library-grid-files/blob/master/docs/developer-guide.md)
first if you have not worked with `GridFile` and `Message` before. The
[README](../README.md) gives a user-level overview of the three service APIs.

## Contents

1. [What the library does](#1-what-the-library-does)
2. [Building](#2-building)
3. [Source tree](#3-source-tree)
4. [The service-interface pattern](#4-the-service-interface-pattern)
5. [Content model](#5-content-model)
6. [Content Server backends](#6-content-server-backends)
7. [Events and synchronisation](#7-events-and-synchronisation)
8. [Data Server](#8-data-server)
9. [Query Server](#9-query-server)
10. [Functions: C++ and Lua](#10-functions-c-and-lua)
11. [Remote access: CORBA and HTTP](#11-remote-access-corba-and-http)
12. [How the pieces are deployed](#12-how-the-pieces-are-deployed)
13. [Concurrency](#13-concurrency)
14. [Coding conventions](#14-coding-conventions)
15. [Common tasks](#15-common-tasks)
16. [Known pitfalls](#16-known-pitfalls)

---

## 1. What the library does

grid-content provides the three services that SmartMet Server uses to serve gridded
data:

| Service | Question it answers | Main implementation |
|---------|---------------------|---------------------|
| **Content Server** (`ContentServer::`) | *What data exists?* It is a registry of producers, generations (model runs), geometries, files and content records (one record per field in a file). | `RedisImplementation` (persistent) with a `CacheImplementation` (in memory) in front of it |
| **Data Server** (`DataServer::`) | *What are the values?* It opens the files with grid-files and answers value, vector, contour and coordinate requests for a given `(fileId, messageIndex)`. | `DataServer::ServiceImplementation` |
| **Query Server** (`QueryServer::`) | *Give me parameter X for producer P at these places and times.* It resolves names, producers, levels and times into content records, fetches values from the Data Server, interpolates in time and level, and evaluates functions. | `QueryServer::ServiceImplementation` |

Plugins (timeseries through the grid engine, wms, wfs, edr, download, cross_section,
grid-gui, grid-admin) use these services through the grid engine
(`brainstorm/engines/grid`). The engine creates and wires the service objects. Content
is written into the Content Server by the tools in `tools-grid` (`filesys2smartmet`,
`radon2smartmet`), which scan data directories or the Radon database.

There is also session and user management (`sessionManagement/`,
`userManagement/`) following the same interface pattern. The servers do not enforce
it; see [§16](#16-known-pitfalls).

## 2. Building

```bash
make                    # generates CORBA stubs, then builds libsmartmet-grid-content.so
make CORBA=disabled     # build without omniORB (see the pitfall in §16)
make install            # headers -> $(includedir)/smartmet/grid-content/..., lib -> $(libdir)
make rpm
make doc                # Doxygen HTML into doc/html
make clean              # also deletes the generated CORBA stubs
```

Things to know:

* **CORBA stubs are generated, not committed.** `make` runs `create_stubs`, which calls
  `omniidl` on `src/*/corba/idl/*.idl` and writes the result into
  `src/*/corba/stubs/`. Only the `Makefile` in each `stubs/` directory is in git.
  `make clean` deletes the stubs again.
* **Source selection is by wildcard.** `SRCS` globs a fixed set of directory patterns
  (`src/*/definition/*.cpp`, `src/*/cache/*.cpp`, `src/*/corba/*/*.cpp`, …). A new
  file in an existing directory is picked up automatically. A new directory needs a
  `vpath` entry, a glob in `SRCS` (for both the CORBA and the non-CORBA branch), an
  `objdir` entry and `install` lines.
* **Unresolved-symbol check.** After linking, `ldd -r` must find no undefined symbols,
  or the `.so` is deleted.
* **No formatting.** `.clang-format` has `DisableFormat: true`, and `make format`
  points at paths that do not exist. Keep the existing hand-aligned style.
* **No unit tests.** There is no `test/` directory. Changes are tested through the
  grid engine and plugin test suites (`brainstorm/engines/grid/test`, the timeseries,
  wms and edr grid tests) and with the `tools-grid` command-line clients
  (`cs_*`, `ds_*`, `qs_*`), which call single service methods.

Dependencies: grid-files, spine, macgyver, gis, hiredis, libpq, libcurl, LuaJIT, GDAL,
ICU, fmt, and omniORB when CORBA is enabled.

## 3. Source tree

```
src/
  contentServer/
    definition/     ServiceInterface + the record and list types (ProducerInfo, FileInfo,
                    ContentInfo, EventInfo, … and their *List containers), ServiceResults
    redis/          RedisImplementation            (persistent, the usual master)
    postgresql/     PostgresqlImplementation       (persistent, alternative master)
    memory/         MemoryImplementation           (in-memory, optionally file-backed)
    cache/          CacheImplementation            (in-memory mirror of a master, event-synced)
    merge/          MergeImplementation            (one cache over several masters)
    corba/          idl/ client/ server/ convert/ stubs/
    http/           client/ server/ common/
  dataServer/
    definition/     ServiceInterface, GridData, GridCoordinates, ValueRecord(List)
    implementation/ ServiceImplementation, GridFileManager
    corba/          idl/ client/ server/ convert/ stubs/
  queryServer/
    definition/     ServiceInterface, Query, QueryParameter, ParameterValues,
                    Alias/AliasFile(Collection), ParameterMapping(File),
                    QueryConfigurator, QueryStreamer, UnitConversion
    implementation/ ServiceImplementation
    corba/          idl/ client/ server/ convert/ stubs/
  functions/        Built-in C++ functions (Function_*.cpp) + FunctionCollection
  lua/              LuaFile, LuaFileCollection, LuaFunction
  sessionManagement/, userManagement/
python/             omniidl Python bindings for the IDL interfaces
```

Namespaces follow the directories: `SmartMet::ContentServer`, `SmartMet::DataServer`,
`SmartMet::QueryServer`, `SmartMet::Functions`, `SmartMet::Lua`, and the `Corba` /
`HTTP` sub-namespaces. The record types live in `SmartMet::T`, next to the grid-files
typedefs (`T::ProducerInfo`, `T::ContentInfoList`, …).

## 4. The service-interface pattern

Each service is an abstract `ServiceInterface` class. Every API method exists twice:

```cpp
public:
  virtual int getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo);
protected:
  virtual int _getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo);
```

The public method (in `definition/ServiceInterface.cpp`) returns `SERVICE_DISABLED` if
the service is disabled. Otherwise it calls the protected `_` method and, if the
processing log is enabled, logs the call with its arguments, result and duration.
**Implementations override only the `_` methods.** The base `_` methods throw
`"Implementation required!"`.

All methods return an `int` result code (`ServiceResults.h`: `OK = 0`, negative values
for errors, `getResultString()` for text) and pass data through reference arguments.
Implementations catch exceptions and turn them into `Result::UNEXPECTED_EXCEPTION`. The
stack trace goes to the debug log, if one is set. So an error on the far side of a
CORBA or HTTP call reaches the caller only as a number. When you debug, enable the
debug log of the server that failed.

Because every backend and transport implements the same interface, services can be
stacked:

```
QueryServer ─┬─> ContentServer::CacheImplementation ──> RedisImplementation ──> Redis
             │                                          (or Corba/HTTP::ClientImplementation ──> remote server)
             └─> DataServer::ServiceImplementation ──> ContentServer (same cache)
                                                   └─> grid-files GridFile/Message
```

The calling code does not know whether a `ServiceInterface*` is local or remote.

## 5. Content model

```
Producer (ProducerInfo)                 e.g. ECG = ECMWF global
  └─ Generation (GenerationInfo)        one model run: analysis time, status
       ├─ Geometry (GeometryInfo)       the grid geometries that run has, with their own status
       └─ File (FileInfo)               one physical file: name, server, protocol, size
            └─ Content (ContentInfo)    one field = one grid-files Message
```

Key fields:

* **`ContentInfo`**: `mFileId` + `mMessageIndex` (the primary key),
  `mFilePosition`, `mMessageSize`, `mFileType`, producer and generation ids,
  `mFmiParameterId` / FMI name, `mFmiParameterLevelId`, `mParameterLevel`,
  `mForecastTimeUTC`, `mForecastType`, `mForecastNumber`, `mGeometryId`, and the
  aggregation and processing-type ids. These are exactly the fields that the Data Server
  hands to `GridFile::newMessage()`, so that a file can be served **without parsing it
  first** (see the grid-files guide, §5).
* **`GenerationInfo::mStatus`** and **`GeometryInfo::mStatus`**: `Disabled` (0),
  `Ready` (1), `Running` (2). By default, queries use only ready generations and
  geometries (`Query::Flags::AcceptNotReadyGenerations` / `…Geometries` override that).
  The ingest tool sets the status to *Ready* once a run is complete.
* **`mDeletionTime`** (generation, geometry, file, content): the time after which the
  record may be removed. The Data Server stops serving a file 180 s before it, so that
  readers are not still mapping a file when it is unlinked.
* **`mSourceId`**: which ingest process or source created the record. Every
  `delete…BySourceId` method uses it, so one ingest tool can replace its own content
  without touching the others.
* **`FileInfo::mServerType`, `mProtocol`, `mServer`**: where the bytes come from
  (local filesystem, S3, THREDDS, HTTP). The Data Server copies them into the
  `GridFile`, and the grid-files memory mapper uses them.
* **`FileInfo::Flags::LocalCacheRecommended`**: the Data Server may copy the file to its
  local cache directory (`setFileCache()`).

The record types serialise to and from `;`-separated CSV (`getCsv()` / `setCsv()`).
That format is used as the Redis value, in the Memory and Cache save files, and by the
HTTP transport. **Adding a field changes the CSV format**; see [§15.3](#153-adding-a-field-to-a-record).

The `*List` containers (`ContentInfoList`, `FileInfoList`, …) are arrays of pointers
with optional sorting (`ComparisonMethod::…`) and binary search. `ContentInfoList`
supports many sort orders (by file and message, by parameter and time, by time and
parameter, …). The cache keeps several copies of the content list, sorted differently,
so that each common query shape is a binary search.

## 6. Content Server backends

| Class | Storage | Events | Typical use |
|-------|---------|--------|-------------|
| `RedisImplementation` | Redis sorted sets (below) | yes, stored in Redis | The master registry. Ingest tools write here; servers read from it. Supports a secondary instance. |
| `PostgresqlImplementation` | PostgreSQL tables | yes, stored in an `event` table | An alternative master. All SQL string literals are escaped with `PQescapeStringConn` (`sqlText()`). Keep it that way. |
| `MemoryImplementation` | process memory, optionally loaded from and saved to CSV files in a directory | optional | Standalone and test setups, and small local content sources. |
| `CacheImplementation` | process memory, loaded from a master, then kept in sync from its events | re-publishes events locally | Put in front of Redis or Postgres in every server. All reads are served from memory. |
| `MergeImplementation` | like the cache, but over **several** masters | yes | When the engine has more than one content source. |
| `Corba::ClientImplementation`, `HTTP::ClientImplementation` | none (proxy) | forwarded | Access to a Content Server in another process. |

### Redis layout

All keys have a configurable table prefix (`mTablePrefix`, for example `a.`), so several
registries can share one Redis database. The collections are sorted sets whose score is
the numeric id and whose member is the record's CSV line:

| Key | Score | Member |
|-----|-------|--------|
| `<prefix>producers` | producer id | `ProducerInfo` CSV |
| `<prefix>generations` | generation id | `GenerationInfo` CSV |
| `<prefix>geometries` | generation id (queried with id ranges) | `GeometryInfo` CSV |
| `<prefix>files` | file id | `FileInfo` CSV |
| `<prefix>content` | `(fileId << 24) + messageIndex` | `ContentInfo` CSV |
| `<prefix>events` | event id | `EventInfo` CSV |
| `<prefix>filenames` (hash) | filename → file id | |
| `<prefix>producerCounter`, `generationCounter`, `fileCounter`, `eventCounter` | | id generators (`INCR`) |
| `<prefix>lockRequestCounter`, `lockReleaseCounter` | | a ticket lock serialising writers |

File ids are cut to 32 bits, and a file can hold at most 2^24 messages.

Writers take the **ticket lock**: `INCR lockRequestCounter` returns a ticket, and the
writer waits until `lockReleaseCounter + 1` equals its ticket. If the lock is not
released in time, the waiter resets the release counter and proceeds. That keeps a
crashed writer from blocking everyone forever, but a writer that is merely slow can
then overlap with the next one.

## 7. Events and synchronisation

Every modifying call on a master adds an `EventInfo` (`definition/EventInfo.h`): event
id, type (`FILE_ADDED`, `CONTENT_DELETED`, `GENERATION_STATUS_CHANGED`, …), the server
start time, the affected ids in `mId1..mId3` (for `FILE_ADDED`: file id, file
type and content count), and an optional payload in `mEventData`. Consumers read the
records themselves from the master when they need them. The event list is capped (`setEventListMaxLength`); the oldest events are
trimmed.

Consumers poll it. `CacheImplementation::processEvents()` (run every second by the
event thread) does the following:

1. `getLastEventInfo()` on the master. If the event's `mServerTime` is newer than the
   one seen at start, the master was restarted (or `CONTENT_SERVER_RELOAD` / `CLEAR`
   was issued): the cache runs `reloadData()` and reads everything again.
2. Otherwise it fetches up to 10 000 events after `mLastProcessedEventId` and applies
   each one with `processEvent()` → `event_fileAdded()`, `event_contentDeleted()`, ….
   The event is also appended to the cache's own event list, so that downstream
   consumers (the Data Server) can follow the cache instead of the master.
3. Every `contentUpdateInterval` seconds, `updateContent()` compacts records that were
   marked deleted. In **content swap** mode (`setContentSwap(true, …)`) it builds a
   completely new `SearchStructure` (all sorted lists) and swaps it in, so readers never
   see a half-updated index. Without swap mode, events update the live lists under the
   write lock.

The Data Server runs the same loop against its Content Server (normally the cache) to
learn which files appeared, changed or disappeared
(`DataServer::ServiceImplementation::processEvents()`).

**When you add a new modifying operation:**

* add an event type to `EventInfo.h` (keep the numbering scheme: 1x producers,
  2x generations, 3x geometries, 4x files, 5x content),
* emit it from **every** master implementation (Redis, Postgres, Memory),
* handle it in `CacheImplementation::processEvent()`, in
  `MergeImplementation::processEvent()` and, if it affects files, in
  `DataServer::ServiceImplementation::processEvent()`.

Forgetting one of these means caches silently disagree with the master until the next
full reload.

## 8. Data Server

`DataServer::ServiceImplementation` owns a `GridFileManager`: a map of
`fileId → GRID::GridFile_sptr`.

* **Loading.** On `FILE_ADDED` (followed by the content events for that file), and
  lazily on first access through `getGridFile(fileId)`, `addFile()` creates a
  `GridFile`. It copies the `FileInfo` fields into it and calls
  `newMessage(index, MessageInfo)` for every `ContentInfo`. Nothing is parsed or
  mapped until a request touches the file. File names are relative to the data
  directory given to `init()`, unless the directory is empty or `/`.
* **Serving.** Every value method has the same shape:

  ```cpp
  GRID::GridFile_sptr gridFile = getGridFile(fileId);      // FILE_NOT_FOUND if missing
  GRID::Message* message = gridFile->getMessageByIndex(messageIndex);  // MESSAGE_NOT_FOUND
  message->getGridValueByPoint(...);                       // the grid-files Message API
  ```

  The two-message and four-message variants (`…ByLevel…`, `…ByTime…`,
  `…ByTimeLevel…`) take `fileId1/messageIndex1 … fileId4/messageIndex4` and use
  `GRID::MessageProcessing` to interpolate first. Contours come back as WKB
  (`T::ByteData_vec`).
* **Unhealthy files.** If a file has a memory-mapper or message-position error,
  `getGridFile()` moves its deletion time up to *now + 60 s* and returns nothing, so the
  file is dropped soon and read again from the content registry.
* **Cleanup and caching.** Every `interval` seconds, `setCleanup(age, interval)` (defaults
  300 s / 60 s) drops `GridFile` objects that have not been accessed for `age` seconds.
  They are re-created lazily on the next request. Files whose deletion time passed more
  than 60 s ago are dropped every 30 s. `setFileCache(true, dir)` starts the cache thread, which
  copies files flagged `LocalCacheRecommended` into `dir`. `setDem()` and
  `setLandCover()` provide terrain data for the landscape interpolation method.

The Data Server is where grid-files' own global state (value cache, memory mapper,
`gridDef`) is used. Those are configured by the grid engine, not by this library.

## 9. Query Server

`QueryServer::ServiceImplementation` turns a `Query` into values.

### The Query object

`Query` (`definition/Query.h`) holds:

* **what:** `mQueryParameterList`, one `QueryParameter` per requested parameter or
  function;
* **where:** `mCoordinateType` + `mAreaCoordinates` (points, or polygon rings),
  `mRadius` for circles, and `mAttributeList` for grid and geometry requests
  (`grid.geometryId`, `grid.crs`, `grid.bbox`, …, the same keys as in grid-files);
* **when:** `mSearchType` (`TimeSteps` = the given `mForecastTimeList`, or
  `TimeRange` = everything between `mStartTime` and `mEndTime`), plus
  `mTimesteps` and `mTimestepSizeInMinutes`;
* **which data:** `mProducerNameList` (tried in order), `mAnalysisTime`,
  `mGeometryIdList`, `mGenerationFlags`;
* **behaviour:** `mFlags` (`Query::Flags`: time range from data, latest or oldest
  generation, same analysis time for all parameters, accept not-ready generations,
  `GeometryHitNotRequired`, …), `mLanguage`, `mTimezone`,
  `mMaxProcessingTimeInSeconds`.

Results are written back into each `QueryParameter::mValueList` as `ParameterValues`
(one per time step, with the producer, generation, geometry, level and the values).

`QueryConfigurator` builds a `Query` from HTTP-style key/value parameters, and
`QueryStreamer` serialises results. Plugins that use the grid engine mostly build the
`Query` themselves.

### Parameter strings

A parameter can carry its own overrides, separated by `:`:

```
name:producer:geometryId:levelId:level:forecastType:forecastNumber:generationFlags:areaInterp:timeInterp:levelInterp
T-K:ECG:1008:2:850            temperature, producer ECG, geometry 1008, pressure level 850
T-K::::M2                     "M" prefix: level in metres (height interpolation)
```

The fields are parsed by `getParameterStringInfo()`. Empty fields fall back to the
query-level values. The level and forecast-number fields may be lists.

### Name resolution

For each parameter, the Query Server:

1. **expands aliases** from the alias files (`name:definition` lines, for example
   `TempC:K2C{TempK}`). Aliases may refer to other aliases and to functions.
2. **parses functions**: `FUNC{arg1;arg2;…}`. The arguments are parameters or nested
   functions. Each argument becomes an internal `QueryParameter`, and the function is
   evaluated after the values have been fetched ([§10](#10-functions-c-and-lua)).
3. **resolves the parameter name** through the **parameter mapping files**
   (`ParameterMappingFile`, lines of `producer;mappingName;keyType;key;geometryId;
   levelIdType;levelId;level;areaInterp;timeInterp;levelInterp;…`). A mapping says
   which stored field (by FMI id or name, GRIB id, Newbase id or name, or NetCDF name)
   the name means for a given producer and geometry, and how to interpolate it.
4. **chooses the producer and geometry.** If the query names producers, they are tried
   in order. Otherwise the **producer file** decides: each line is
   `PRODUCER:geometryId[;CONCAT_PRODUCER:geometryId…]`, and the first line whose
   geometry contains the requested location wins. The extra producers on a line are
   used to complete the time series when the first one does not cover it
   (`mProducerConcatMap`). Producers and geometries missing from this file cannot be
   queried at all.
5. **picks the generation** (latest ready, or the one matching `mAnalysisTime` or the
   generation flags) and finds the content records for the needed times and levels.
   It uses the Content Server with a local content cache for these lookups.
6. **fetches values** from the Data Server. If the time or level falls between stored
   fields, it uses the `…ByTime…`, `…ByLevel…` and `…ByTimeLevel…` methods with
   the interpolation methods from the mapping. Height queries (`M` levels) go through
   `getPointValuesByHeight()` and the height conversion file.
7. **post-processes**: evaluates functions, applies unit conversions
   (`UnitConversion`, from the unit conversion file), and removes internal helper
   parameters and aggregation-only times (`mAggregationTimes`).

The producer, mapping, alias, unit and height files and the Lua files are **reloaded
automatically** when their modification time changes. A background thread
(`updateProcessing()`) checks them periodically, and the grid engine can also generate
extra mapping files for parameters found in the content
(`query-server.mappingUpdateFile.*`).

Every `executeQuery()` checks `mMaxProcessingTimeInSeconds` between parameters and
throws `"Maximum processing time reached"` when it is exceeded.

## 10. Functions: C++ and Lua

Functions are looked up by name, **first in the C++ `FunctionCollection`**, then in the
Lua files. A C++ function therefore shadows a Lua function with the same name.

* **C++** (`src/functions/`): each `Function_xxx` derives from `Functions::Function` and
  implements the call types it supports: `executeFunctionCall1` (one value from a list
  of values, for example `SUM{a;b}` per point), `executeFunctionCall4` (two input grids
  plus the grid-point angles, one output grid) and `executeFunctionCall9` (any number of
  input grids plus extra parameters). Each comes in `float` and `double` versions. They are registered in `QueryServer::ServiceImplementation::init()`
  with `mFunctionCollection.addFunction("NAME", new Function_xxx(...))`. Some names
  reuse a class with a constant (`K2C` = `Function_add(-273.15)`).
* **Lua** (`src/lua/`): `LuaFileCollection` loads the configured `.lua` files and
  reloads them when they change. Each file must define
  `getFunctionNames(type)`, which returns a comma-separated list of the functions it
  implements for that call type:
  * type 1: `f(numOfParams, params)` → `result.value`, `result.message`
  * type 4: `f(columns, rows, params1, params2, angles)` → a grid (vector
    functions, for example wind rotation)
  * others: see the header comment in the engine's `cfg/lua/function_basic.lua`.

  Each `LuaFile` keeps a pool of `NUM_OF_LUA_HANDLES` (20) independent Lua states, so
  up to 20 calls into one file can run at the same time. The functions must use
  `ParamValueMissing` (`-16777216`) for missing values.

Move a function from Lua to C++ when it is hot. Lua calls convert every value array
across the C/Lua boundary.

## 11. Remote access: CORBA and HTTP

Each service has a CORBA IDL (`src/*/corba/idl/*.idl`), a `ClientImplementation`
(a `ServiceInterface` whose `_` methods make the remote call), a `ServerInterface`
(which receives the call and forwards it to a local `ServiceInterface`), and a
`Converter` that copies between the C++ types and the CORBA types.

The Content Server also has an **HTTP** transport (`http/client`, `http/server`). A
request is a list of `key=value` lines starting with `method=<name>` and `sessionId=…`,
and the response is lines as well; records travel as their CSV. The HTTP transport
exists for places where CORBA is blocked. It covers most but **not all** Content Server
methods (for example `getContentListOfInvalidIntegrity` is CORBA-only).

The servers run in:

* `tools-grid/src/servers/` (`corbaContentServer`, `corbaDataServer`,
  `corbaQueryServer`, `corbaGridServer` (all three in one process),
  `httpContentServer`),
* the `grid-admin` plugin, which hosts `ContentServer::HTTP::ServerInterface` over the
  engine's content sources,
* the grid engine, which can use `Corba::ClientImplementation` for any of the three
  services instead of a local implementation.

## 12. How the pieces are deployed

The usual production setup, everything inside `smartmetd` through the grid engine:

```
filesys2smartmet / radon2smartmet ──writes──> Redis  (RedisImplementation)
                                                │  events
smartmetd (grid engine)                         ▼
  ContentServer::CacheImplementation  <── polls Redis, holds all records in memory
      │  events
      ▼
  DataServer::ServiceImplementation   <── maps files lazily via grid-files
      ▲
  QueryServer::ServiceImplementation  ──> uses the cache + the data server
      ▲
  plugins (timeseries, wms, edr, download, …)
```

The grid engine configuration (`smartmet.engine.grid.content-server`,
`.data-server`, `.query-server`) chooses each service: local implementation or a remote
IOR. For the content server it also lists one or more `content-source` entries (type
`redis`, `postgresql`, `corba`, `http` or `file`, the last one being a
`MemoryImplementation` over a CSV directory) and whether to wrap them in a cache or a
merge. With
several sources, `MergeImplementation` replaces the cache. The engine also initialises
grid-files (`gridDef`, the memory mapper and the value cache) before creating the Data
Server.

## 13. Concurrency

Request threads call all three services concurrently. The rules the code follows:

* `CacheImplementation` and `MergeImplementation` protect their lists with
  `ModificationLock`s (read/write; `mModificationLock` for the primary lists,
  `mSearchModificationLock` for the search structure). Readers take read locks, and
  event handlers upgrade to a write lock only for the actual modification. In swap mode
  the search structure is instead rebuilt off-line and swapped in. `processEvents()` called from a request thread returns at once while a reload
  is running, so requests are not blocked behind a full reload.
* `RedisImplementation` uses one hiredis connection per object, guarded by a thread
  lock. On an error it closes the connection and returns `PERMANENT_STORAGE_ERROR`, and
  the next call opens a new connection.
* The Data Server's `addFile()` is serialised by `mThreadLock`. Reads go through
  `GridFile_sptr`, so a file removed by an event stays alive until the last request
  using it finishes.
* The Query Server's configuration maps (producers, mappings, aliases) each have their
  own read/write lock. A reload clears and refills a map under its write lock, so
  queries wait for the reload to finish.

When you add state to one of these classes, use the same lock types
(`ModificationLock` with `AutoReadLock` / `AutoWriteLock`, `ThreadLock` with
`AutoThreadLock` from grid-files `common/`) and do not call a remote service while
holding a write lock.

## 14. Coding conventions

The same as in grid-files:

* `FUNCTION_TRACE` + `try { … } catch (...) { throw Fmi::Exception(BCP,"Operation failed!",nullptr); }`
  in every function. Add context with `exception.addParameter(...)`.
* Service methods do not throw across the interface. Catch, log the stack trace to the
  debug log, and return a `Result::…` code.
* Hand-aligned declarations, 2-space indentation, Doxygen `/*! \brief */` on public
  members. clang-format is off.
* Use `fmt::format` (not `sprintf` into fixed buffers), `boost::regex` (not
  `std::regex`), and, in PostgreSQL code, `sqlText()` for every value placed inside an
  SQL literal.
* Use the `T::` types for ids (`T::FileId`, `T::GenerationId`, …) and `uchar` status
  codes from the record classes' `Status` constants.

## 15. Common tasks

### 15.1 Adding a Content Server method

A single method touches many files. For reference, `getContentListOfInvalidIntegrity`
is implemented in:

| Layer | Files |
|-------|-------|
| Interface | `definition/ServiceInterface.{h,cpp}` (public method with logging + `_` hook) |
| Masters | `redis/`, `postgresql/`, `memory/` |
| Caches | `cache/`, `merge/` |
| CORBA | `corba/idl/ContentServer_serviceInterface.idl`, `corba/client/`, `corba/server/`, `corba/convert/` if new types |
| HTTP (optional) | `http/client/ClientImplementation.cpp`, `http/server/ServerInterface.cpp` |

A backend that does not override the `_` method throws "Implementation required!"
when the method is called. Make sure every backend used in production implements it.

### 15.2 Adding a Data Server or Query Server method

The same pattern, but with only one implementation and the CORBA client, server and
IDL. Data Server methods that take a `(fileId, messageIndex)` should use
`getGridFile()` and `getMessageByIndex()` exactly as the existing ones do, and should
return `FILE_NOT_FOUND` / `MESSAGE_NOT_FOUND` rather than throwing.

### 15.3 Adding a field to a record

The CSV form is the storage and wire format. Adding a field to `ContentInfo`,
`FileInfo`, … means:

* append it to `getCsv()`, `getCsvHeader()` and `setCsv()`. Append at the end, and
  make `setCsv()` accept the old field count, so that existing Redis data and CSV
  files still load;
* update the CORBA IDL struct and `Converter`, and the PostgreSQL table and
  statements;
* update `duplicate()`, the copy constructor, `print()`, and the hash if the field
  should count as a change;
* release the ingest tools (tools-grid), the engine and this library together.
  Mixed versions will mis-parse each other's records.

### 15.4 Adding a built-in function

Implement `src/functions/Function_xxx.{h,cpp}` (derive from `Functions::Function` and
override the call types you support, `duplicate()` and `print()`), and register it in
`QueryServer::ServiceImplementation::init()`. Check that the name does not collide with
a Lua function you still want to use.

### 15.5 Changing public headers

All headers under `src/` are installed and used by the grid engine, tools-grid and
several plugins. Changing virtual methods or data members of `ServiceInterface`,
the record classes, `Query` or `QueryParameter` changes the ABI. Bump the spec version
and the dependency floors, and rebuild and release the dependants together.

## 16. Known pitfalls

* **Sessions are not enforced.** `isSessionValid()` returns true in the Redis, Cache
  and Data Server implementations. Anyone who can reach a CORBA or HTTP server port can
  call every method, including the deleting ones. Protect the ports at the network level.
* **`make CORBA=disabled` leaves out `merge/` and `postgresql/`.** The non-CORBA `SRCS`
  list does not include those directories, although neither depends on CORBA. The grid
  engine includes `PostgresqlImplementation.h` unconditionally, so a CORBA-less build of
  the stack fails to link.
* **The Redis ticket lock can be stolen.** A writer that waits too long resets the
  lock and proceeds ([§6](#6-content-server-backends)). Two slow ingest processes can
  then write at the same time.
* **Master restarts force a full reload.** A Redis content server restart, `clear` or
  `reload` makes every cache re-read everything. With millions of content records,
  that takes a while, during which the cache answers from the old data (swap mode) or
  may block.
* **Error details stay on the server side.** Clients see only result codes. Enable the
  debug log (`setDebugLog`) on the failing server to see the stack trace.
* **`DataServer::ServiceImplementation::getDataServerByFileId()` always returns
  `nullptr`.** The forwarding to another Data Server that the callers expect is not
  implemented.
* **Files near deletion disappear early.** Files whose `mDeletionTime` is less than
  180 s away are no longer served. Ingest tools must set deletion times with that margin
  in mind.
