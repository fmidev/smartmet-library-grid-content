// In-process grid data stack for tests: content server + data server + query server.
//
// Production runs these services behind Redis and CORBA, which makes them hard to test. Here
// the content server is the in-memory implementation, the data server reads the GRIB files of
// smartmet-test-data directly, and the query server is configured with the grid engine test
// configuration (smartmet-engine-grid-test): producers, parameter mappings, aliases and Lua
// functions. Files are registered the way filesys2smartmet registers them: one content record
// per message, with the metadata grid-files identifies.

#pragma once

#include "TestCommon.h"
#include "contentServer/memory/MemoryImplementation.h"
#include "dataServer/implementation/ServiceImplementation.h"
#include "queryServer/implementation/ServiceImplementation.h"

#include <grid-files/grid/GridFile.h>
#include <grid-files/grid/Message.h>
#include <grid-files/identification/GridDef.h>

#include <map>
#include <set>
#include <memory>
#include <string>
#include <vector>

namespace GridTest
{
using namespace SmartMet;

inline std::string engineFile(const std::string &name)
{
  return std::string(GRID_TEST_DIR) + "/engine/" + name;
}

class GridWorld
{
 public:
  GridWorld()
  {
    contentServer.init(false, false, false, true, "/nonexistent", 0);
    dataServer.init(0, 1, "test", "", "", &contentServer);
  }

  // Start the query server once the data has been registered. Like the grid engine does with
  // mapping_fmi_auto.csv, a mapping is generated for every registered parameter (the mappings
  // shipped with the test configuration were made for an older parameter identification).
  void start()
  {
    std::string mappings;
    for (const auto &line : mappingLines)
      mappings += line + "\n";
    mappingFile = std::make_unique<TempFile>(mappings, "gridmapping");

    string_vec mappingFiles = {mappingFile->name(),
                               engineFile("mapping_fmi_test.csv"),
                               engineFile("mapping_newbase_test.csv")};
    string_vec mappingAliasFiles;
    string_vec aliasFiles = {engineFile("alias_demo.cfg"), engineFile("alias_newbase_extension.cfg")};
    string_vec producerAliasFiles = {engineFile("producerAlias_test.cfg")};
    string_vec luaFiles = {engineFile("function_basic.lua"),
                           engineFile("function_interpolation.lua"),
                           engineFile("function_conversion.lua"),
                           engineFile("function_newbase.lua"),
                           engineFile("function_demo.lua")};

    queryServer.init(&contentServer,
                     &dataServer,
                     CONFIG,
                     engineFile("height_conversions.csv"),
                     mappingFiles,
                     "",
                     mappingAliasFiles,
                     aliasFiles,
                     engineFile("producers_test.csv"),
                     producerAliasFiles,
                     luaFiles,
                     false,
                     true);
  }

  ~GridWorld()
  {
    queryServer.shutdown();
    dataServer.shutdown();
    contentServer.shutdown();
  }

  // Register a producer with one generation and the given GRIB files
  T::GenerationId addProducer(const std::string &producer,
                              const std::string &analysisTime,
                              const std::vector<std::string> &files)
  {
    T::ProducerId producerId = producerIds[producer];
    if (producerId == 0)
    {
      T::ProducerInfo p;
      p.mName = producer;
      p.mTitle = producer;
      p.mSourceId = 100;
      BOOST_TEST_REQUIRE(contentServer.addProducerInfo(0, p) == 0);
      producerId = producerIds[producer] = p.mProducerId;
    }

    T::GenerationInfo g;
    g.mProducerId = producerId;
    g.mName = producer + ":" + analysisTime;
    g.mAnalysisTime = analysisTime;
    g.mStatus = T::GenerationInfo::Status::Ready;
    g.mSourceId = 100;
    BOOST_TEST_REQUIRE(contentServer.addGenerationInfo(0, g) == 0);

    for (const auto &file : files)
    {
      GRID::GridFile gridFile;
      gridFile.read(file);

      T::FileInfo f;
      f.mProducerId = producerId;
      f.mGenerationId = g.mGenerationId;
      f.mName = file;
      f.mFileType = gridFile.getFileType();
      f.mServerType = T::FileInfo::ServerType::Filesys;
      f.mSourceId = 100;

      T::ContentInfoList contents;
      for (uint i = 0; i < gridFile.getNumberOfMessages(); i++)
      {
        GRID::Message *message = gridFile.getMessageByIndex(i);
        auto *c = new T::ContentInfo();
        c->mMessageIndex = i;
        c->mFileType = message->getMessageType();
        c->mFilePosition = message->getFilePosition();
        c->mMessageSize = message->getMessageSize();
        c->setForecastTime(message->getForecastTime());
        c->mFmiParameterId = message->getFmiParameterId();
        c->setFmiParameterName(message->getFmiParameterName());
        c->mFmiParameterLevelId = message->getFmiParameterLevelId();
        c->mParameterLevel = message->getGridParameterLevel();
        c->mForecastType = message->getForecastType();
        c->mForecastNumber = message->getForecastNumber();
        c->mGeometryId = message->getGridGeometryId();
        c->mSourceId = 100;
        contents.addContentInfo(c);

        // producer;mapping name;FMI_NAME;name;geometry;FMI level type;level id;level;
        // area/time/level interpolation;group flags;search match;functions;precision
        const char *name = message->getFmiParameterName();
        if (name != nullptr && *name != '\0')
          mappingLines.insert(producer + ";" + name + ";2;" + name + ";" +
                              std::to_string(c->mGeometryId) + ";1;" +
                              std::to_string(c->mFmiParameterLevelId) + ";" +
                              std::to_string(c->mParameterLevel) + ";1;1;1;0;E;;;1;");
      }
      BOOST_TEST_REQUIRE(contentServer.addFileInfoWithContentList(0, f, contents) == 0);
    }
    return g.mGenerationId;
  }

  // The grid definitions are needed already when the services are constructed
  struct GridDefInit
  {
    GridDefInit() { Identification::gridDef.init(CONFIG); }
  } gridDefInit;

  ContentServer::MemoryImplementation contentServer;
  DataServer::ServiceImplementation dataServer;
  QueryServer::ServiceImplementation queryServer;

 private:
  std::map<std::string, T::ProducerId> producerIds;
  std::set<std::string> mappingLines;
  std::unique_ptr<TempFile> mappingFile;
};

}  // namespace GridTest
