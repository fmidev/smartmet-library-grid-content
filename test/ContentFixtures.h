// Builders for content server records used by the content server tests.
//
// A small "model" world: producers with generations, each generation has files and each file
// has content records (parameter x level x forecast time). All records are created through the
// public ContentServer::ServiceInterface so that the tested implementation assigns the ids and
// generates the events exactly as in production.

#pragma once

#include "contentServer/definition/ServiceInterface.h"

#include <boost/test/unit_test.hpp>
#include <fmt/format.h>

#include <ctime>
#include <string>
#include <vector>

namespace GridTest
{
using namespace SmartMet;

inline const T::SessionId SESSION = 0;

// 2026-01-01 00:00:00 UTC
inline const time_t BASE_TIME = 1767225600;

inline T::ProducerInfo makeProducer(const std::string &name)
{
  T::ProducerInfo p;
  p.mName = name;
  p.mTitle = name + " title";
  p.mDescription = name + " description";
  p.mSourceId = 100;
  return p;
}

inline T::GenerationInfo makeGeneration(T::ProducerId producerId,
                                        const std::string &name,
                                        time_t analysisTime,
                                        uchar status = T::GenerationInfo::Status::Ready)
{
  T::GenerationInfo g;
  g.mProducerId = producerId;
  g.mName = name;
  g.mDescription = name;
  char buf[32];
  struct tm tm;
  gmtime_r(&analysisTime, &tm);
  strftime(buf, sizeof(buf), "%Y%m%dT%H%M%S", &tm);
  g.mAnalysisTime = buf;
  g.mStatus = status;
  g.mSourceId = 100;
  return g;
}

inline T::FileInfo makeFile(T::ProducerId producerId,
                            T::GenerationId generationId,
                            const std::string &name)
{
  T::FileInfo f;
  f.mProducerId = producerId;
  f.mGenerationId = generationId;
  f.mName = name;
  f.mFileType = T::FileTypeValue::Grib2;
  f.mSourceId = 100;
  return f;
}

inline T::ContentInfo *makeContent(T::FmiParamId paramId,
                                   const std::string &paramName,
                                   T::ParamLevelId levelId,
                                   T::ParamLevel level,
                                   time_t forecastTime,
                                   T::MessageIndex messageIndex,
                                   T::GeometryId geometryId = 1008)
{
  auto *c = new T::ContentInfo();
  c->mMessageIndex = messageIndex;
  c->mFilePosition = 1000 * messageIndex;
  c->mMessageSize = 1000;
  c->mFmiParameterId = paramId;
  c->setFmiParameterName(paramName);
  c->mFmiParameterLevelId = levelId;
  c->mParameterLevel = level;
  c->setForecastTime(forecastTime);
  c->mForecastType = 1;
  c->mForecastNumber = -1;
  c->mGeometryId = geometryId;
  c->mSourceId = 100;
  return c;
}

// One file per forecast time, each with the given parameters at level 0
inline std::vector<T::FileId> addFiles(ContentServer::ServiceInterface &cs,
                                       T::ProducerId producerId,
                                       T::GenerationId generationId,
                                       const std::string &prefix,
                                       int times,
                                       const std::vector<std::pair<T::FmiParamId, std::string>> &params)
{
  std::vector<T::FileId> ids;
  for (int t = 0; t < times; t++)
  {
    T::FileInfo f = makeFile(producerId, generationId, fmt::format("{}_{:03d}.grib", prefix, t));
    T::ContentInfoList contents;
    T::MessageIndex index = 0;
    for (const auto &p : params)
      contents.addContentInfo(makeContent(p.first, p.second, 1, 0, BASE_TIME + 3600 * t, index++));
    BOOST_TEST_REQUIRE(cs.addFileInfoWithContentList(SESSION, f, contents) == 0);
    ids.push_back(f.mFileId);
  }
  return ids;
}

inline uint contentCount(ContentServer::ServiceInterface &cs)
{
  uint count = 0;
  BOOST_TEST_REQUIRE(cs.getContentCount(SESSION, count) == 0);
  return count;
}

inline uint fileCount(ContentServer::ServiceInterface &cs)
{
  uint count = 0;
  BOOST_TEST_REQUIRE(cs.getFileInfoCount(SESSION, count) == 0);
  return count;
}

inline uint generationCount(ContentServer::ServiceInterface &cs)
{
  uint count = 0;
  BOOST_TEST_REQUIRE(cs.getGenerationInfoCount(SESSION, count) == 0);
  return count;
}

inline uint producerCount(ContentServer::ServiceInterface &cs)
{
  uint count = 0;
  BOOST_TEST_REQUIRE(cs.getProducerInfoCount(SESSION, count) == 0);
  return count;
}

// Content records of a generation, e.g. to compare two content servers
inline std::vector<std::string> contentKeys(ContentServer::ServiceInterface &cs,
                                            T::GenerationId generationId)
{
  T::ContentInfoList list;
  cs.getContentListByGenerationId(SESSION, generationId, 0, 0, 1000000, 0, list);
  std::vector<std::string> keys;
  for (uint i = 0; i < list.getLength(); i++)
  {
    const T::ContentInfo *c = list.getContentInfoByIndex(i);
    keys.push_back(fmt::format("{}:{}:{}:{}:{}",
                               c->mFileId,
                               c->mMessageIndex,
                               c->mFmiParameterId,
                               c->mParameterLevel,
                               c->mForecastTimeUTC));
  }
  std::sort(keys.begin(), keys.end());
  return keys;
}

}  // namespace GridTest
