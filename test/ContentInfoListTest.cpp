// Property tests for T::ContentInfoList, the sorted container behind every content server search.
//
// A few thousand random content records are searched with the indexed list methods, and every
// result is compared with a brute force filter over the same records. The searches are run on
// lists sorted by different comparison methods, since several methods take a fast path only
// when the list happens to be sorted for them.

#define BOOST_TEST_MODULE ContentInfoListTest
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include "TestCommon.h"
#include "contentServer/definition/ContentInfoList.h"
#include "contentServer/definition/RequestFlags.h"

#include <fmt/format.h>

#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <vector>

using namespace SmartMet;
namespace bdata = boost::unit_test::data;
using CM = T::ContentInfo::ComparisonMethod;

namespace
{
const time_t BASE = 1767225600;  // 2026-01-01 00:00 UTC

struct Rng
{
  unsigned seed;
  unsigned operator()(unsigned n) { return (seed = seed * 1103515245 + 12345, (seed >> 16) & 0x7fff) % n; }
};

// Generations 1..4 belong to producers 1,1,2,2
T::ProducerId producerOf(T::GenerationId g) { return g <= 2 ? 1 : 2; }

std::vector<T::ContentInfo> randomRecords(std::size_t n, unsigned seed)
{
  Rng rnd{seed};
  const T::FmiParamId params[] = {153, 139, 163};
  const T::ParamLevel levels[] = {0, 10, 500, 850};
  std::set<std::pair<T::FileId, T::MessageIndex>> used;
  std::vector<T::ContentInfo> records;
  while (records.size() < n)
  {
    T::ContentInfo c;
    c.mFileId = 1 + rnd(400);
    c.mMessageIndex = rnd(20);
    if (!used.insert({c.mFileId, c.mMessageIndex}).second)
      continue;
    c.mGenerationId = 1 + rnd(4);
    c.mProducerId = producerOf(c.mGenerationId);
    c.mFmiParameterId = params[rnd(3)];
    c.setFmiParameterName(fmt::format("P{}", c.mFmiParameterId));
    c.mFmiParameterLevelId = 1 + rnd(3);
    c.mParameterLevel = levels[rnd(4)];
    c.mForecastType = rnd(2) ? 1 : 3;
    c.mForecastNumber = c.mForecastType == 1 ? -1 : static_cast<T::ForecastNumber>(rnd(4));
    c.mGeometryId = rnd(2) ? 1008 : 1009;
    c.setForecastTime(BASE + 3600 * static_cast<time_t>(rnd(24)));
    records.push_back(c);
  }
  return records;
}

T::ContentInfoList makeList(const std::vector<T::ContentInfo> &records, uint method)
{
  T::ContentInfoList list;
  list.setComparisonMethod(method);
  for (const auto &r : records)
    list.addContentInfo(new T::ContentInfo(r));
  return list;
}

using Key = std::string;

std::set<Key> keys(const T::ContentInfoList &list)
{
  std::set<Key> ret;
  for (uint i = 0; i < list.getLength(); i++)
  {
    const T::ContentInfo *c = list.getContentInfoByIndex(i);
    ret.insert(fmt::format("{}:{}", c->mFileId, c->mMessageIndex));
  }
  return ret;
}

std::set<Key> filter(const std::vector<T::ContentInfo> &records,
                     const std::function<bool(const T::ContentInfo &)> &pred)
{
  std::set<Key> ret;
  for (const auto &r : records)
    if ((r.mFlags & T::ContentInfo::Flags::DeletedContent) == 0 && pred(r))
      ret.insert(fmt::format("{}:{}", r.mFileId, r.mMessageIndex));
  return ret;
}

const std::vector<uint> METHODS = {CM::file_message,
                                   CM::fmiId_producer_generation_level_time,
                                   CM::starttime_file_message,
                                   CM::generationId_starttime_file_message};

}  // namespace

BOOST_DATA_TEST_CASE(sorted_by_every_method, bdata::make(std::vector<uint>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}), method)
{
  auto records = randomRecords(2000, 1 + method);
  T::ContentInfoList list = makeList(records, CM::none);
  list.sort(method);
  BOOST_TEST_REQUIRE(list.getLength() == records.size());
  for (uint i = 1; i < list.getLength(); i++)
  {
    T::ContentInfo *a = list.getContentInfoByIndex(i - 1);
    T::ContentInfo *b = list.getContentInfoByIndex(i);
    if (a->compare(method, b) > 0)
    {
      BOOST_ERROR("records " << i - 1 << " and " << i << " are out of order");
      break;
    }
  }
}

BOOST_DATA_TEST_CASE(lookup_by_file_and_message, bdata::make(METHODS), method)
{
  auto records = randomRecords(3000, 7);
  T::ContentInfoList list = makeList(records, method);
  BOOST_TEST_REQUIRE(list.getLength() == records.size());

  // Duplicates of the same message are not added
  T::ContentInfo dup = records[17];
  T::ContentInfo *dupPtr = dup.duplicate();
  T::ContentInfo *ret = list.addContentInfo(dupPtr);
  if (method != CM::none && method == CM::file_message)
  {
    BOOST_TEST((ret != dupPtr), "a duplicate record was added");
    delete dupPtr;
  }
  else if (ret != dupPtr)
    delete dupPtr;

  list.sort(CM::file_message);
  for (std::size_t i = 0; i < records.size(); i += 7)
  {
    T::ContentInfo found;
    BOOST_TEST(list.getContentInfoByFileIdAndMessageIndex(records[i].mFileId, records[i].mMessageIndex, found));
    BOOST_TEST(found.mFmiParameterId == records[i].mFmiParameterId);
  }
  T::ContentInfo notFound;
  BOOST_TEST(!list.getContentInfoByFileIdAndMessageIndex(100000, 0, notFound));
}

BOOST_DATA_TEST_CASE(parameter_search_matches_brute_force, bdata::make(METHODS), method)
{
  auto records = randomRecords(3000, 11);
  T::ContentInfoList list = makeList(records, method);
  Rng rnd{99};

  for (int q = 0; q < 200; q++)
  {
    const T::GenerationId gen = 1 + rnd(4);
    const T::FmiParamId param = std::vector<T::FmiParamId>{153, 139, 163}[rnd(3)];
    const T::ParamLevelId levelId = rnd(4) == 0 ? -1 : static_cast<T::ParamLevelId>(1 + rnd(3));
    const T::ParamLevel minLevel = rnd(2) ? 0 : 10;
    const T::ParamLevel maxLevel = rnd(2) ? 500 : 1000;
    const T::ForecastType ftype = rnd(3) == 0 ? -1 : (rnd(2) ? 1 : 3);
    const T::ForecastNumber fnum = rnd(2) ? -1 : static_cast<T::ForecastNumber>(rnd(4));
    const T::GeometryId geom = rnd(3) == 0 ? -1 : (rnd(2) ? 1008 : 1009);
    const time_t t1 = BASE + 3600 * static_cast<time_t>(rnd(24));
    const time_t t2 = t1 + 3600 * static_cast<time_t>(rnd(6));

    T::ContentInfoList result;
    list.getContentInfoListByFmiParameterIdAndGenerationId(
        producerOf(gen), gen, param, levelId, minLevel, maxLevel, ftype, fnum, geom, t1, t2, 0, result);

    auto expected = filter(records,
                           [&](const T::ContentInfo &c)
                           {
                             return c.mGenerationId == gen && c.mFmiParameterId == param &&
                                    (ftype < 0 || (c.mForecastType == ftype &&
                                                   (fnum < 0 || c.mForecastNumber == fnum))) &&
                                    (geom < 0 || c.mGeometryId == geom) &&
                                    (levelId < 0 || (c.mFmiParameterLevelId == levelId &&
                                                     c.mParameterLevel >= minLevel &&
                                                     c.mParameterLevel <= maxLevel)) &&
                                    c.mForecastTimeUTC >= t1 && c.mForecastTimeUTC <= t2;
                           });

    BOOST_TEST_CONTEXT("query " << q << " gen " << gen << " param " << param << " levelId "
                                << levelId << " [" << minLevel << "," << maxLevel << "] ftype "
                                << ftype << " fnum " << fnum << " geom " << geom)
    {
      BOOST_TEST(keys(result) == expected, boost::test_tools::per_element());
    }
  }
}

BOOST_DATA_TEST_CASE(generation_time_range_matches_brute_force, bdata::make(METHODS), method)
{
  auto records = randomRecords(3000, 13);
  T::ContentInfoList list = makeList(records, method);
  for (T::GenerationId gen = 1; gen <= 4; gen++)
  {
    const time_t t1 = BASE + 5 * 3600;
    const time_t t2 = BASE + 9 * 3600;
    T::ContentInfoList result;
    list.getContentInfoListByGenerationId(producerOf(gen), gen, t1, t2, result);
    auto expected = filter(records,
                           [&](const T::ContentInfo &c) {
                             return c.mGenerationId == gen && c.mForecastTimeUTC >= t1 &&
                                    c.mForecastTimeUTC <= t2;
                           });
    BOOST_TEST_CONTEXT("generation " << gen) { BOOST_TEST(keys(result) == expected, boost::test_tools::per_element()); }

    time_t start = 0;
    time_t end = 0;
    list.getForecastTimeRangeByGenerationId(producerOf(gen), gen, start, end);
    time_t emin = 0;
    time_t emax = 0;
    for (const auto &r : records)
    {
      if (r.mGenerationId != gen)
        continue;
      if (emin == 0 || r.mForecastTimeUTC < emin)
        emin = r.mForecastTimeUTC;
      emax = std::max(emax, r.mForecastTimeUTC);
    }
    BOOST_TEST(start == emin);
    BOOST_TEST(end == emax);
  }
}

BOOST_AUTO_TEST_CASE(include_time_before_and_after)
{
  // When nothing is found inside the time range the closest earlier/later fields are returned
  // on request, which is how interpolation in time gets its neighbours
  std::vector<T::ContentInfo> records;
  for (int h : {0, 6, 12})
  {
    T::ContentInfo c;
    c.mFileId = 1 + h;
    c.mGenerationId = 1;
    c.mProducerId = 1;
    c.mFmiParameterId = 153;
    c.mFmiParameterLevelId = 1;
    c.mForecastType = 1;
    c.mForecastNumber = -1;
    c.mGeometryId = 1008;
    c.setForecastTime(BASE + 3600 * h);
    records.push_back(c);
  }
  T::ContentInfoList list = makeList(records, CM::fmiId_producer_generation_level_time);

  auto query = [&](time_t t1, time_t t2, uint flags)
  {
    T::ContentInfoList result;
    list.getContentInfoListByFmiParameterIdAndGenerationId(1, 1, 153, 1, 0, 0, 1, -1, 1008, t1, t2, flags, result);
    std::vector<int> hours;
    for (uint i = 0; i < result.getLength(); i++)
      hours.push_back(static_cast<int>((result.getContentInfoByIndex(i)->mForecastTimeUTC - BASE) / 3600));
    std::sort(hours.begin(), hours.end());
    return hours;
  };

  const uint both = ContentServer::RequestFlags::INCLUDE_TIME_BEFORE | ContentServer::RequestFlags::INCLUDE_TIME_AFTER;
  BOOST_TEST(query(BASE + 3 * 3600, BASE + 3 * 3600, 0).empty());
  BOOST_TEST((query(BASE + 3 * 3600, BASE + 3 * 3600, both) == std::vector<int>{0, 6}));
  BOOST_TEST((query(BASE + 3 * 3600, BASE + 3 * 3600, ContentServer::RequestFlags::INCLUDE_TIME_BEFORE) == std::vector<int>{0}));
  BOOST_TEST((query(BASE + 7 * 3600, BASE + 8 * 3600, both) == std::vector<int>{6, 12}));
  BOOST_TEST((query(BASE + 6 * 3600, BASE + 6 * 3600, both) == std::vector<int>{6}));
  BOOST_TEST((query(BASE + 13 * 3600, BASE + 14 * 3600, both) == std::vector<int>{12}));
}

BOOST_DATA_TEST_CASE(mark_and_delete, bdata::make(METHODS), method)
{
  auto records = randomRecords(2000, 17);
  T::ContentInfoList list = makeList(records, method);

  const uint marked = list.markDeletedByGenerationId(2);
  const auto inGen2 = filter(records, [](const T::ContentInfo &c) { return c.mGenerationId == 2; });
  BOOST_TEST(marked == inGen2.size());

  // Marked records are invisible to the searches
  T::ContentInfoList result;
  list.getContentInfoListByGenerationId(1, 2, BASE, BASE + 86400, result);
  BOOST_TEST(result.getLength() == 0U);

  const uint deleted = list.deleteMarkedContent();
  BOOST_TEST(deleted == marked);
  BOOST_TEST(list.getLength() == records.size() - marked);

  const uint byFile = list.deleteContentInfoByFileId(records[0].mFileId);
  const auto inFile = filter(records,
                             [&](const T::ContentInfo &c)
                             { return c.mFileId == records[0].mFileId && c.mGenerationId != 2; });
  BOOST_TEST(byFile == inFile.size());
}
