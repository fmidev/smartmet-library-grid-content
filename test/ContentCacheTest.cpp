// Tests for the content cache (ContentServer::CacheImplementation).
//
// The cache mirrors a content storage (Redis in production, an in-memory content server here) and
// follows its event list. After every batch of modifications the test processes the events
// synchronously and checks that the cache answers exactly like the storage, in both the normal
// and the content swap mode. The storage can be told to fail reads of a generation, which is how
// a generation was lost in production (a failed read for GENERATION_ADDED).

#define BOOST_TEST_MODULE ContentCacheTest
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include "TestCommon.h"
#include "ContentFixtures.h"
#include "contentServer/cache/CacheImplementation.h"
#include "contentServer/definition/ServiceResults.h"
#include "contentServer/memory/MemoryImplementation.h"

#include <grid-files/identification/GridDef.h>

#include <map>
#include <memory>
#include <set>

using namespace SmartMet;
using namespace GridTest;
namespace CS = ContentServer;
namespace bdata = boost::unit_test::data;

namespace
{
// Content storage whose generation reads can be made to fail
class Storage : public CS::MemoryImplementation
{
 public:
  Storage() { init(false, false, false, true, "/nonexistent", 0); }

  std::set<T::GenerationId> failingGenerations;

 protected:
  int _getGenerationInfoById(T::SessionId sessionId,
                             T::GenerationId generationId,
                             T::GenerationInfo &generationInfo) override
  {
    if (failingGenerations.count(generationId) > 0)
      return CS::Result::DATA_NOT_FOUND;
    return CS::MemoryImplementation::_getGenerationInfoById(sessionId, generationId, generationInfo);
  }
};

// Cache whose event processing and search structure update are driven by the test
class Cache : public CS::CacheImplementation
{
 public:
  explicit Cache(Storage &storage, bool swap)
  {
    if (swap)
      setContentSwap(true, 0, 0);
    init(SESSION, 1, &storage);
  }

  // Process all pending events and rebuild the search structures
  void sync()
  {
    processEvents(true);
    mContentUpdateTime = 0;
    mContentUpdateRequired = true;
    updateContent();
  }
};

const std::vector<std::pair<T::FmiParamId, std::string>> PARAMS = {
    {153, "T-K"}, {139, "P-PA"}, {163, "RH-PRCNT"}};

struct GridDefInit
{
  GridDefInit()
  {
    if (exists(CONFIG))
      Identification::gridDef.init(CONFIG);
  }
};

std::string describe(CS::ServiceInterface &cs)
{
  // Everything the cache must agree on, as a comparable string
  std::string out;
  T::ProducerInfoList producers;
  cs.getProducerInfoList(SESSION, producers);
  for (uint i = 0; i < producers.getLength(); i++)
  {
    const T::ProducerInfo *p = producers.getProducerInfoByIndex(i);
    out += fmt::format("producer {} {}\n", p->mProducerId, p->mName);
  }
  T::GenerationInfoList generations;
  cs.getGenerationInfoList(SESSION, generations);
  std::vector<std::string> lines;
  for (uint i = 0; i < generations.getLength(); i++)
  {
    const T::GenerationInfo *g = generations.getGenerationInfoByIndex(i);
    lines.push_back(fmt::format("generation {} {} producer {} status {}\n",
                                g->mGenerationId,
                                g->mName,
                                g->mProducerId,
                                static_cast<int>(g->mStatus)));
    for (const auto &key : contentKeys(cs, g->mGenerationId))
      lines.push_back("  content " + key + "\n");
  }
  for (const auto &l : lines)
    out += l;
  T::FileInfoList files;
  cs.getFileInfoList(SESSION, 0, 1000000, files);
  std::vector<std::string> names;
  for (uint i = 0; i < files.getLength(); i++)
  {
    const T::FileInfo *f = files.getFileInfoByIndex(i);
    names.push_back(fmt::format("file {} {} generation {}\n", f->mFileId, f->mName, f->mGenerationId));
  }
  std::sort(names.begin(), names.end());
  for (const auto &n : names)
    out += n;
  return out;
}

void requireSame(Storage &storage, Cache &cache)
{
  const std::string expected = describe(storage);
  const std::string actual = describe(cache);
  BOOST_TEST(actual == expected);
  BOOST_TEST(contentCount(cache) == contentCount(storage));
  BOOST_TEST(fileCount(cache) == fileCount(storage));
  BOOST_TEST(generationCount(cache) == generationCount(storage));
}

const std::vector<bool> MODES = {false, true};

}  // namespace

BOOST_TEST_GLOBAL_FIXTURE(GridDefInit);

BOOST_DATA_TEST_CASE(initial_load, bdata::make(MODES), swap)
{
  Storage storage;
  T::ProducerInfo p = makeProducer("P");
  storage.addProducerInfo(SESSION, p);
  T::GenerationInfo g = makeGeneration(p.mProducerId, "P:1", BASE_TIME);
  storage.addGenerationInfo(SESSION, g);
  addFiles(storage, p.mProducerId, g.mGenerationId, "p", 5, PARAMS);

  Cache cache(storage, swap);
  cache.sync();
  BOOST_TEST(cache.isReady());
  requireSame(storage, cache);
  BOOST_TEST(contentCount(cache) == 15U);
}

BOOST_DATA_TEST_CASE(follows_additions_and_deletions, bdata::make(MODES), swap)
{
  Storage storage;
  Cache cache(storage, swap);
  cache.sync();
  BOOST_TEST(contentCount(cache) == 0U);

  T::ProducerInfo p = makeProducer("P");
  storage.addProducerInfo(SESSION, p);
  T::GenerationInfo g1 = makeGeneration(p.mProducerId, "P:1", BASE_TIME, T::GenerationInfo::Status::Running);
  storage.addGenerationInfo(SESSION, g1);
  auto files1 = addFiles(storage, p.mProducerId, g1.mGenerationId, "g1", 3, PARAMS);
  cache.sync();
  requireSame(storage, cache);

  storage.setGenerationInfoStatusById(SESSION, g1.mGenerationId, T::GenerationInfo::Status::Ready);
  T::GenerationInfo g2 = makeGeneration(p.mProducerId, "P:2", BASE_TIME + 3600);
  storage.addGenerationInfo(SESSION, g2);
  addFiles(storage, p.mProducerId, g2.mGenerationId, "g2", 4, PARAMS);
  cache.sync();
  requireSame(storage, cache);

  storage.deleteFileInfoById(SESSION, files1[1]);
  cache.sync();
  requireSame(storage, cache);

  storage.deleteGenerationInfoById(SESSION, g1.mGenerationId);
  cache.sync();
  requireSame(storage, cache);
  BOOST_TEST(contentCount(cache) == 12U);

  storage.deleteProducerInfoById(SESSION, p.mProducerId);
  cache.sync();
  requireSame(storage, cache);
  BOOST_TEST(contentCount(cache) == 0U);
}

BOOST_DATA_TEST_CASE(follows_content_replacement, bdata::make(MODES), swap)
{
  // A file registered again with new content (e.g. a rewritten file) must have its new content
  // in the cache, also when it is the most recently added file
  Storage storage;
  T::ProducerInfo p = makeProducer("P");
  storage.addProducerInfo(SESSION, p);
  T::GenerationInfo g = makeGeneration(p.mProducerId, "P:1", BASE_TIME);
  storage.addGenerationInfo(SESSION, g);
  auto files = addFiles(storage, p.mProducerId, g.mGenerationId, "p", 3, PARAMS);

  Cache cache(storage, swap);
  cache.sync();

  for (T::FileId fileId : {files[0], files.back()})
  {
    T::FileInfo old;
    BOOST_TEST_REQUIRE(storage.getFileInfoById(SESSION, fileId, old) == 0);
    T::FileInfo f = makeFile(p.mProducerId, g.mGenerationId, old.mName);
    T::ContentInfoList contents;
    contents.addContentInfo(makeContent(153, "T-K", 1, 0, BASE_TIME, 0));
    BOOST_TEST_REQUIRE(storage.addFileInfoWithContentList(SESSION, f, contents) == 0);
    cache.sync();
    BOOST_TEST_CONTEXT("file " << old.mName) { requireSame(storage, cache); }
  }
}

BOOST_DATA_TEST_CASE(repairs_a_generation_whose_addition_failed, bdata::make(MODES), swap)
{
  // In production a failed read for GENERATION_ADDED hid the generation from the cache until a
  // restart, and the newest model run was never served. The status change event (or a later
  // update) must repair it.
  Storage storage;
  T::ProducerInfo p = makeProducer("P");
  storage.addProducerInfo(SESSION, p);
  T::GenerationInfo g1 = makeGeneration(p.mProducerId, "P:1", BASE_TIME);
  storage.addGenerationInfo(SESSION, g1);
  addFiles(storage, p.mProducerId, g1.mGenerationId, "g1", 2, PARAMS);

  Cache cache(storage, swap);
  cache.sync();

  // The next generation id: make its read fail while the addition event is processed
  T::GenerationInfo g2 = makeGeneration(p.mProducerId, "P:2", BASE_TIME + 3600, T::GenerationInfo::Status::Running);
  storage.failingGenerations.insert(g1.mGenerationId + 1);
  storage.addGenerationInfo(SESSION, g2);
  BOOST_TEST_REQUIRE(g2.mGenerationId == g1.mGenerationId + 1);
  addFiles(storage, p.mProducerId, g2.mGenerationId, "g2", 2, PARAMS);
  cache.sync();

  T::GenerationInfo info;
  BOOST_TEST(cache.getGenerationInfoById(SESSION, g2.mGenerationId, info) != 0,
             "the failed read should have hidden the generation");

  // The storage recovers and the model run completes
  storage.failingGenerations.clear();
  storage.setGenerationInfoStatusById(SESSION, g2.mGenerationId, T::GenerationInfo::Status::Ready);
  cache.sync();

  BOOST_TEST_REQUIRE(cache.getGenerationInfoById(SESSION, g2.mGenerationId, info) == 0);
  BOOST_TEST(static_cast<int>(info.mStatus) == static_cast<int>(T::GenerationInfo::Status::Ready));
  requireSame(storage, cache);

  T::GenerationInfo latest;
  BOOST_TEST(cache.getLastGenerationInfoByProducerIdAndStatus(
                 SESSION, p.mProducerId, T::GenerationInfo::Status::Ready, latest) == 0);
  BOOST_TEST(latest.mGenerationId == g2.mGenerationId);
}

BOOST_DATA_TEST_CASE(randomized_modifications, bdata::make(MODES), swap)
{
  Storage storage;
  Cache cache(storage, swap);
  T::ProducerInfo p = makeProducer("P");
  storage.addProducerInfo(SESSION, p);

  std::map<T::GenerationId, std::vector<T::FileId>> model;
  unsigned seed = 4711;
  auto rnd = [&seed]() { return seed = seed * 1103515245 + 12345, (seed >> 16) & 0x7fff; };

  for (int step = 0; step < 200; step++)
  {
    const unsigned op = rnd() % 10;
    if (op < 2 || model.empty())
    {
      T::GenerationInfo g = makeGeneration(p.mProducerId, fmt::format("P:{}", step), BASE_TIME + step);
      storage.addGenerationInfo(SESSION, g);
      model[g.mGenerationId];
    }
    else if (op < 7)
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      auto files = addFiles(storage, p.mProducerId, it->first, fmt::format("f{}", step), 1 + rnd() % 3, PARAMS);
      it->second.insert(it->second.end(), files.begin(), files.end());
    }
    else if (op < 9)
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      if (!it->second.empty())
      {
        const auto k = rnd() % it->second.size();
        storage.deleteFileInfoById(SESSION, it->second[k]);
        it->second.erase(it->second.begin() + k);
      }
    }
    else
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      storage.deleteGenerationInfoById(SESSION, it->first);
      model.erase(it);
    }

    // Synchronize at irregular intervals so that several events are processed at once
    if (rnd() % 4 == 0)
    {
      cache.sync();
      BOOST_TEST_CONTEXT("step " << step) { requireSame(storage, cache); }
    }
  }
  cache.sync();
  requireSame(storage, cache);
}
