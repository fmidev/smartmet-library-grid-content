// Tests for the in-memory content server (ContentServer::MemoryImplementation) through the public
// ContentServer::ServiceInterface: registration of producers, generations, files and content,
// lookups and searches, cascading deletes and the event list that the content caches follow.

#define BOOST_TEST_MODULE ContentServerMemoryTest
#include <boost/test/included/unit_test.hpp>

#include "TestCommon.h"
#include "ContentFixtures.h"
#include "contentServer/definition/EventInfo.h"
#include "contentServer/definition/ServiceResults.h"
#include "contentServer/memory/MemoryImplementation.h"

#include <grid-files/identification/GridDef.h>

#include <filesystem>
#include <map>
#include <memory>

using namespace SmartMet;
using namespace GridTest;
namespace CS = ContentServer;

namespace
{
std::unique_ptr<CS::MemoryImplementation> newServer()
{
  auto cs = std::make_unique<CS::MemoryImplementation>();
  // no load/save/sync, events enabled
  cs->init(false, false, false, true, "/nonexistent", 0);
  return cs;
}

std::vector<uint> eventTypes(CS::ServiceInterface &cs, T::EventId startId = 0)
{
  T::EventInfoList list;
  BOOST_TEST_REQUIRE(cs.getEventInfoList(SESSION, 0, startId, 100000, list) == 0);
  std::vector<uint> types;
  for (T::EventInfo *e = list.getFirstEvent(); e != nullptr; e = e->nextItem)
    types.push_back(e->mType);
  return types;
}

T::EventId lastEventId(CS::ServiceInterface &cs)
{
  T::EventInfo e;
  cs.getLastEventInfo(SESSION, 0, e);
  return e.mEventId;
}

// Parameter name searches use the FMI parameter definitions
struct GridDefInit
{
  GridDefInit()
  {
    if (exists(CONFIG))
      Identification::gridDef.init(CONFIG);
  }
};

const std::vector<std::pair<T::FmiParamId, std::string>> PARAMS = {
    {153, "T-K"}, {139, "P-PA"}, {163, "RH-PRCNT"}};

// A producer with two generations of 4 forecast times x 3 parameters
struct World
{
  std::unique_ptr<CS::MemoryImplementation> cs = newServer();
  T::ProducerId producer = 0;
  T::GenerationId gen1 = 0;
  T::GenerationId gen2 = 0;
  std::vector<T::FileId> files1;
  std::vector<T::FileId> files2;

  World()
  {
    T::ProducerInfo p = makeProducer("ECMWF");
    BOOST_TEST_REQUIRE(cs->addProducerInfo(SESSION, p) == 0);
    producer = p.mProducerId;

    T::GenerationInfo g1 = makeGeneration(producer, "ECMWF:20260101T000000", BASE_TIME);
    BOOST_TEST_REQUIRE(cs->addGenerationInfo(SESSION, g1) == 0);
    gen1 = g1.mGenerationId;

    T::GenerationInfo g2 = makeGeneration(producer, "ECMWF:20260101T120000", BASE_TIME + 43200);
    BOOST_TEST_REQUIRE(cs->addGenerationInfo(SESSION, g2) == 0);
    gen2 = g2.mGenerationId;

    files1 = addFiles(*cs, producer, gen1, "gen1", 4, PARAMS);
    files2 = addFiles(*cs, producer, gen2, "gen2", 4, PARAMS);
  }
};
}  // namespace

BOOST_TEST_GLOBAL_FIXTURE(GridDefInit);

BOOST_AUTO_TEST_CASE(registration_and_lookups)
{
  World w;
  auto &cs = *w.cs;

  BOOST_TEST(producerCount(cs) == 1U);
  BOOST_TEST(generationCount(cs) == 2U);
  BOOST_TEST(fileCount(cs) == 8U);
  BOOST_TEST(contentCount(cs) == 24U);

  // ids are assigned by the server
  BOOST_TEST(w.producer > 0U);
  BOOST_TEST(w.gen1 > 0U);
  BOOST_TEST(w.gen2 != w.gen1);

  T::ProducerInfo p;
  BOOST_TEST(cs.getProducerInfoByName(SESSION, "ECMWF", p) == 0);
  BOOST_TEST(p.mProducerId == w.producer);
  BOOST_TEST(cs.getProducerInfoByName(SESSION, "nonexistent", p) == CS::Result::DATA_NOT_FOUND);

  T::GenerationInfo g;
  BOOST_TEST(cs.getGenerationInfoByName(SESSION, "ECMWF:20260101T120000", g) == 0);
  BOOST_TEST(g.mGenerationId == w.gen2);

  T::FileInfo f;
  BOOST_TEST(cs.getFileInfoByName(SESSION, "gen1_002.grib", f) == 0);
  BOOST_TEST(f.mFileId == w.files1[2]);
  BOOST_TEST(f.mGenerationId == w.gen1);

  T::ContentInfoList list;
  BOOST_TEST(cs.getContentListByFileId(SESSION, w.files1[2], list) == 0);
  BOOST_TEST(list.getLength() == 3U);
  for (uint i = 0; i < list.getLength(); i++)
  {
    // the server fills the file data into the content records
    const T::ContentInfo *c = list.getContentInfoByIndex(i);
    BOOST_TEST(c->mFileId == w.files1[2]);
    BOOST_TEST(c->mProducerId == w.producer);
    BOOST_TEST(c->mGenerationId == w.gen1);
    BOOST_TEST(c->mForecastTimeUTC == BASE_TIME + 2 * 3600);
  }

  BOOST_TEST(contentKeys(cs, w.gen1).size() == 12U);
  BOOST_TEST(contentKeys(cs, w.gen2).size() == 12U);
}

BOOST_AUTO_TEST_CASE(duplicates_and_inconsistent_records_are_rejected)
{
  World w;
  auto &cs = *w.cs;

  T::ProducerInfo p = makeProducer("ECMWF");
  BOOST_TEST(cs.addProducerInfo(SESSION, p) == CS::Result::PRODUCER_NAME_ALREADY_REGISTERED);

  // a file must refer to an existing producer and generation of that producer
  T::ProducerInfo other = makeProducer("OTHER");
  BOOST_TEST_REQUIRE(cs.addProducerInfo(SESSION, other) == 0);

  T::ContentInfoList empty;
  T::FileInfo f = makeFile(other.mProducerId, w.gen1, "mismatch.grib");
  BOOST_TEST(cs.addFileInfoWithContentList(SESSION, f, empty) ==
             CS::Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH);

  T::FileInfo f2 = makeFile(w.producer, 999999, "nogeneration.grib");
  BOOST_TEST(cs.addFileInfoWithContentList(SESSION, f2, empty) == CS::Result::UNKNOWN_GENERATION_ID);

  BOOST_TEST(fileCount(cs) == 8U);
}

BOOST_AUTO_TEST_CASE(parameter_and_time_searches, *fixtures({CONFIG}))
{
  requireFixture(CONFIG);
  World w;
  auto &cs = *w.cs;

  // All forecast times of P-PA in generation 1
  T::ContentInfoList list;
  BOOST_TEST(cs.getContentListByParameterAndGenerationId(SESSION,
                                                         w.gen1,
                                                         T::ParamKeyTypeValue::FMI_NAME,
                                                         "P-PA",
                                                         1,
                                                         0,
                                                         0,
                                                         -1,
                                                         -1,
                                                         -1,
                                                         BASE_TIME,
                                                         BASE_TIME + 10 * 3600,
                                                         0,
                                                         list) == 0);
  BOOST_TEST(list.getLength() == 4U);
  for (uint i = 0; i < list.getLength(); i++)
    BOOST_TEST(list.getContentInfoByIndex(i)->mFmiParameterId == 139U);

  // Time range limited search
  T::ContentInfoList list2;
  BOOST_TEST(cs.getContentListByGenerationIdAndTimeRange(
                 SESSION, w.gen2, BASE_TIME + 3600, BASE_TIME + 2 * 3600, list2) == 0);
  BOOST_TEST(list2.getLength() == 6U);

  time_t start = 0;
  time_t end = 0;
  BOOST_TEST(cs.getContentTimeRangeByGenerationId(SESSION, w.gen1, start, end) == 0);
  BOOST_TEST(start == BASE_TIME);
  BOOST_TEST(end == BASE_TIME + 3 * 3600);

  std::set<std::string> times;
  BOOST_TEST(cs.getContentTimeListByGenerationId(SESSION, w.gen1, times) == 0);
  BOOST_TEST(times.size() == 4U);

  T::GenerationInfo latest;
  BOOST_TEST(cs.getLastGenerationInfoByProducerIdAndStatus(
                 SESSION, w.producer, T::GenerationInfo::Status::Ready, latest) == 0);
  BOOST_TEST(latest.mGenerationId == w.gen2);
}

BOOST_AUTO_TEST_CASE(time_range_follows_new_content)
{
  // The content time range of a generation must include content added after it was first asked
  World w;
  auto &cs = *w.cs;
  time_t start = 0;
  time_t end = 0;
  BOOST_TEST_REQUIRE(cs.getContentTimeRangeByProducerAndGenerationId(SESSION, w.producer, w.gen1, start, end) == 0);
  BOOST_TEST(end == BASE_TIME + 3 * 3600);

  T::FileInfo f = makeFile(w.producer, w.gen1, "gen1_late.grib");
  T::ContentInfoList contents;
  contents.addContentInfo(makeContent(153, "T-K", 1, 0, BASE_TIME + 10 * 3600, 0));
  BOOST_TEST_REQUIRE(cs.addFileInfoWithContentList(SESSION, f, contents) == 0);

  BOOST_TEST_REQUIRE(cs.getContentTimeRangeByProducerAndGenerationId(SESSION, w.producer, w.gen1, start, end) == 0);
  BOOST_TEST(start == BASE_TIME);
  BOOST_TEST(end == BASE_TIME + 10 * 3600);
}

BOOST_AUTO_TEST_CASE(cascading_deletes)
{
  World w;
  auto &cs = *w.cs;

  // Deleting a file deletes its content
  BOOST_TEST(cs.deleteFileInfoById(SESSION, w.files1[0]) == 0);
  BOOST_TEST(fileCount(cs) == 7U);
  BOOST_TEST(contentCount(cs) == 21U);
  T::ContentInfoList list;
  cs.getContentListByFileId(SESSION, w.files1[0], list);
  BOOST_TEST(list.getLength() == 0U);

  // Deleting a generation deletes its files and content
  BOOST_TEST(cs.deleteGenerationInfoById(SESSION, w.gen1) == 0);
  BOOST_TEST(generationCount(cs) == 1U);
  BOOST_TEST(fileCount(cs) == 4U);
  BOOST_TEST(contentCount(cs) == 12U);
  BOOST_TEST(contentKeys(cs, w.gen1).empty());

  // Deleting the producer deletes everything below it
  BOOST_TEST(cs.deleteProducerInfoById(SESSION, w.producer) == 0);
  BOOST_TEST(producerCount(cs) == 0U);
  BOOST_TEST(generationCount(cs) == 0U);
  BOOST_TEST(fileCount(cs) == 0U);
  BOOST_TEST(contentCount(cs) == 0U);
}

BOOST_AUTO_TEST_CASE(events_follow_the_modifications)
{
  auto cs = newServer();
  const T::EventId start = lastEventId(*cs);

  T::ProducerInfo p = makeProducer("P");
  cs->addProducerInfo(SESSION, p);
  T::GenerationInfo g = makeGeneration(p.mProducerId, "P:1", BASE_TIME, T::GenerationInfo::Status::Running);
  cs->addGenerationInfo(SESSION, g);
  auto files = addFiles(*cs, p.mProducerId, g.mGenerationId, "p", 2, PARAMS);
  cs->setGenerationInfoStatusById(SESSION, g.mGenerationId, T::GenerationInfo::Status::Ready);
  cs->deleteFileInfoById(SESSION, files[0]);
  cs->deleteGenerationInfoById(SESSION, g.mGenerationId);

  const std::vector<uint> expected = {CS::EventType::PRODUCER_ADDED,
                                      CS::EventType::GENERATION_ADDED,
                                      CS::EventType::FILE_ADDED,
                                      CS::EventType::FILE_ADDED,
                                      CS::EventType::GENERATION_STATUS_CHANGED,
                                      CS::EventType::FILE_DELETED,
                                      CS::EventType::GENERATION_DELETED};
  const auto types = eventTypes(*cs, start + 1);
  BOOST_TEST(types == expected, boost::test_tools::per_element());

  // Event ids increase by one
  T::EventInfoList list;
  cs->getEventInfoList(SESSION, 0, start + 1, 1000, list);
  T::EventId prev = start;
  for (T::EventInfo *e = list.getFirstEvent(); e != nullptr; e = e->nextItem)
  {
    BOOST_TEST(e->mEventId == prev + 1);
    prev = e->mEventId;
  }
  BOOST_TEST(lastEventId(*cs) == prev);
}

BOOST_AUTO_TEST_CASE(readding_a_file_replaces_its_content_and_reports_an_update)
{
  // Re-registering a file with the same name keeps the file id, replaces the content and must
  // be reported as FILE_UPDATED so that the content caches reload the content of the file.
  // This must also hold for the most recently added file.
  World w;
  auto &cs = *w.cs;

  for (T::FileId fileId : {w.files1[1], w.files2.back()})
  {
    T::FileInfo old;
    BOOST_TEST_REQUIRE(cs.getFileInfoById(SESSION, fileId, old) == 0);

    const T::EventId before = lastEventId(cs);
    T::FileInfo f = makeFile(old.mProducerId, old.mGenerationId, old.mName);
    T::ContentInfoList contents;
    contents.addContentInfo(makeContent(9, "WS-MS", 1, 0, BASE_TIME, 0));
    BOOST_TEST_REQUIRE(cs.addFileInfoWithContentList(SESSION, f, contents) == 0);

    BOOST_TEST_CONTEXT("file " << old.mName)
    {
      BOOST_TEST(f.mFileId == fileId);
      T::ContentInfoList list;
      cs.getContentListByFileId(SESSION, fileId, list);
      BOOST_TEST_REQUIRE(list.getLength() == 1U);
      BOOST_TEST(list.getContentInfoByIndex(0)->mFmiParameterId == 9U);

      const auto types = eventTypes(cs, before + 1);
      BOOST_TEST_REQUIRE(types.size() == 1U);
      BOOST_TEST(types[0] == static_cast<uint>(CS::EventType::FILE_UPDATED));
    }
  }
  BOOST_TEST(fileCount(cs) == 8U);
  BOOST_TEST(contentCount(cs) == 24U - 6U + 2U);
}

BOOST_AUTO_TEST_CASE(batch_additions_report_every_new_file)
{
  // Every new file of a batch must be reported as FILE_ADDED, and an existing file of the batch
  // as FILE_UPDATED when its content is replaced
  World w;
  auto &cs = *w.cs;

  std::vector<T::FileAndContent> batch(4);
  for (int i = 0; i < 3; i++)
  {
    batch[i].mFileInfo = makeFile(w.producer, w.gen2, fmt::format("batch_{}.grib", i));
    batch[i].mContentInfoList.addContentInfo(makeContent(153, "T-K", 1, 0, BASE_TIME + 3600 * i, 0));
  }
  T::FileInfo existing;
  BOOST_TEST_REQUIRE(cs.getFileInfoById(SESSION, w.files2[0], existing) == 0);
  batch[3].mFileInfo = makeFile(w.producer, w.gen2, existing.mName);
  batch[3].mContentInfoList.addContentInfo(makeContent(153, "T-K", 1, 0, BASE_TIME, 0));

  const T::EventId before = lastEventId(cs);
  BOOST_TEST_REQUIRE(cs.addFileInfoListWithContent(SESSION, 1, batch) == 0);

  std::vector<uint> expected(3, static_cast<uint>(CS::EventType::FILE_ADDED));
  expected.push_back(static_cast<uint>(CS::EventType::FILE_UPDATED));
  const auto types = eventTypes(cs, before + 1);
  BOOST_TEST(types == expected, boost::test_tools::per_element());

  BOOST_TEST(fileCount(cs) == 11U);
  BOOST_TEST(contentCount(cs) == 24U + 3U - 3U + 1U);
}

BOOST_AUTO_TEST_CASE(content_directory_round_trip)
{
  // A content server saving its content to a directory, and another one loading it (the
  // "file" content source of the grid engine). The loaded server must have the same records.
  char tmpl[] = "/tmp/gridcontent-XXXXXX";
  const std::string dir = mkdtemp(tmpl);

  std::string before;
  {
    CS::MemoryImplementation saver;
    saver.init(false, true, false, true, dir, 0);
    T::ProducerInfo p = makeProducer("P");
    BOOST_TEST_REQUIRE(saver.addProducerInfo(SESSION, p) == 0);
    T::GenerationInfo g1 = makeGeneration(p.mProducerId, "P:1", BASE_TIME);
    BOOST_TEST_REQUIRE(saver.addGenerationInfo(SESSION, g1) == 0);
    T::GenerationInfo g2 = makeGeneration(p.mProducerId, "P:2", BASE_TIME + 3600);
    BOOST_TEST_REQUIRE(saver.addGenerationInfo(SESSION, g2) == 0);
    addFiles(saver, p.mProducerId, g1.mGenerationId, "g1", 3, PARAMS);
    addFiles(saver, p.mProducerId, g2.mGenerationId, "g2", 2, PARAMS);
    BOOST_TEST(generationCount(saver) == 2U);
    before = fmt::format("{} {} {} {}", producerCount(saver), generationCount(saver), fileCount(saver), contentCount(saver));
    for (auto gen : {g1.mGenerationId, g2.mGenerationId})
      for (const auto &k : contentKeys(saver, gen))
        before += " " + k;
  }

  CS::MemoryImplementation loader;
  loader.init(true, false, false, false, dir, 0);
  std::string after = fmt::format("{} {} {} {}", producerCount(loader), generationCount(loader), fileCount(loader), contentCount(loader));
  T::GenerationInfoList generations;
  loader.getGenerationInfoList(SESSION, generations);
  std::vector<T::GenerationId> ids;
  for (uint i = 0; i < generations.getLength(); i++)
    ids.push_back(generations.getGenerationInfoByIndex(i)->mGenerationId);
  std::sort(ids.begin(), ids.end());
  for (auto gen : ids)
    for (const auto &k : contentKeys(loader, gen))
      after += " " + k;

  BOOST_TEST(after == before);
  std::filesystem::remove_all(dir);
}

BOOST_AUTO_TEST_CASE(many_records_stay_consistent)
{
  // A larger randomized sequence of additions and deletions compared against a simple model
  auto cs = newServer();
  T::ProducerInfo p = makeProducer("P");
  cs->addProducerInfo(SESSION, p);

  std::map<T::GenerationId, std::vector<T::FileId>> model;
  unsigned seed = 12345;
  auto rnd = [&seed]() { return seed = seed * 1103515245 + 12345, (seed >> 16) & 0x7fff; };

  for (int step = 0; step < 300; step++)
  {
    const unsigned op = rnd() % 10;
    if (op < 3 || model.empty())
    {
      T::GenerationInfo g = makeGeneration(p.mProducerId, fmt::format("P:{}", step), BASE_TIME + step);
      BOOST_TEST_REQUIRE(cs->addGenerationInfo(SESSION, g) == 0);
      model[g.mGenerationId];
    }
    else if (op < 8)
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      auto files = addFiles(*cs, p.mProducerId, it->first, fmt::format("f{}", step), 1 + rnd() % 3, PARAMS);
      it->second.insert(it->second.end(), files.begin(), files.end());
    }
    else if (op < 9)
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      if (!it->second.empty())
      {
        const auto k = rnd() % it->second.size();
        BOOST_TEST_REQUIRE(cs->deleteFileInfoById(SESSION, it->second[k]) == 0);
        it->second.erase(it->second.begin() + k);
      }
    }
    else
    {
      auto it = std::next(model.begin(), rnd() % model.size());
      BOOST_TEST_REQUIRE(cs->deleteGenerationInfoById(SESSION, it->first) == 0);
      model.erase(it);
    }
  }

  std::size_t files = 0;
  for (const auto &g : model)
  {
    files += g.second.size();
    BOOST_TEST(contentKeys(*cs, g.first).size() == 3 * g.second.size());
    uint count = 0;
    cs->getFileInfoCountByGenerationId(SESSION, g.first, count);
    BOOST_TEST(count == g.second.size());
  }
  BOOST_TEST(generationCount(*cs) == model.size());
  BOOST_TEST(fileCount(*cs) == files);
  BOOST_TEST(contentCount(*cs) == 3 * files);
}
