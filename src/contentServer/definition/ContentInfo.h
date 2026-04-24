#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/GeneralFunctions.h>
#include <string>

namespace SmartMet
{
namespace T
{

class ContentInfo
{
  public:
                       ContentInfo();
                       ContentInfo(const ContentInfo& contentInfo);
                       ContentInfo(const char *csv);
                       ~ContentInfo();

    ContentInfo&       operator=(const ContentInfo& contentInfo);

    std::string        getCsv();
    std::string        getCsvHeader();
    //UInt64             getRequestCounterKey();

    char*              getForecastTime();
    void               setForecastTime(time_t ftime);
    void               setForecastTime(const char *ftime);
    void               setForecastTime(const std::string& ftime);

    char*              getFmiParameterName();
    void               setFmiParameterName(const char *name);
    void               setFmiParameterName(const std::string& name);

    void               setCsv(const char *csv);
    void               setCsv(const std::string& csv);
    //int                compare(uint comparisonMethod,ContentInfo *contentInfo);
    ContentInfo*       duplicate();
    void               print(std::ostream& stream,uint level,uint optionFlags);

    T::FileId          mFileId;
    T::MessageIndex    mMessageIndex;
    T::FilePosition    mFilePosition;
    uint               mMessageSize;
    T::ProducerId      mProducerId;
    T::GenerationId    mGenerationId;
    T::FmiParamId      mFmiParameterId;
    uchar              mFileType;
    T::ParamLevelId    mFmiParameterLevelId;
    T::ParamLevel      mParameterLevel;
    time_t             mForecastTimeUTC;
    time_t             mModificationTime;
    time_t             mDeletionTime;
    T::ForecastType    mForecastType;
    T::ForecastNumber  mForecastNumber;
    T::GeometryId      mGeometryId;
    ushort             mFlags;
    T::StorageId       mStorageId;
    T::SourceId        mSourceId;
    int                mAggregationId;
    int                mAggregationPeriod;
    int                mProcessingTypeId;
    float              mProcessingTypeValue1;
    float              mProcessingTypeValue2;

  protected:

    uint               mFmiParameterName;
    uint               mForecastTime;

  public:

    class ComparisonMethod
    {
      public:
        static const uint none                                            = 0;  // No comparison
        static const uint file_message                                    = 1;
        static const uint fmiId_producer_generation_level_time            = 2;
        static const uint fmiName_producer_generation_level_time          = 3;
        static const uint starttime_file_message                          = 4;
        static const uint fmiName_starttime_level_file_message            = 5;
        static const uint fmiId_fmiLevelId_level_starttime_file_message   = 6;
        static const uint starttime_fmiId_fmiLevelId_level_file_message   = 7;
        static const uint starttime_fmiName_fmiLevelId_level_file_message = 8;
        static const uint generationId_starttime_file_message             = 9;
        static const uint fmiName_fmiLevelId_level_starttime_file_message = 10;
        static const uint starttime_generationId_file_message             = 11;
        static const uint producer_file_message                           = 12;
    };

    class Flags
    {
      public:
        static const uint UnusedFlag              = 0x0001;
        static const uint PreloadRequired         = 0x0004;
        static const uint DeletedContent          = 0x1000;
    };

  public:

    inline int compare(uint comparisonMethod,ContentInfo *contentInfo)
    {
      try
      {
        if (contentInfo == nullptr)
          return 0;

        int res = 0;

        switch (comparisonMethod)
        {
          case ContentInfo::ComparisonMethod::file_message:
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            return num_compare(mMessageIndex,contentInfo->mMessageIndex);

          case ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time:
            res = num_compare(mProducerId,contentInfo->mProducerId);
            if (res != 0)
              return res;
            res = num_compare(mGenerationId,contentInfo->mGenerationId);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time:
            res = num_compare(mProducerId,contentInfo->mProducerId);
            if (res != 0)
              return res;
            res = num_compare(mGenerationId,contentInfo->mGenerationId);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::starttime_file_message:
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            return num_compare(mMessageIndex,contentInfo->mMessageIndex);

          case ContentInfo::ComparisonMethod::fmiName_starttime_level_file_message:
            res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::fmiId_fmiLevelId_level_starttime_file_message:
            res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message:
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message:
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::generationId_starttime_file_message:
            res = num_compare(mGenerationId,contentInfo->mGenerationId);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message:
            res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
            if (res != 0)
              return res;
            res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
            if (res != 0)
              return res;
            res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mGeometryId,contentInfo->mGeometryId);
            if (res != 0)
              return res;
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::starttime_generationId_file_message:
            res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
            if (res != 0)
              return res;
            res = num_compare(mGenerationId,contentInfo->mGenerationId);
            if (res != 0)
              return res;
            res = num_compare(mForecastType,contentInfo->mForecastType);
            if (res != 0)
              return res;
            res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;

          case ContentInfo::ComparisonMethod::producer_file_message:
            res = num_compare(mProducerId,contentInfo->mProducerId);
            if (res != 0)
              return res;
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
            if (res != 0)
              return res;
            return 0;


          default:
            res = num_compare(mFileId,contentInfo->mFileId);
            if (res != 0)
              return res;
            return num_compare(mMessageIndex,contentInfo->mMessageIndex);
        }
        return 0;
      }
      catch (...)
      {
        throw Fmi::Exception(BCP,"Operation failed!",nullptr);
      }
    }

};


typedef ContentInfo* ContentInfoPtr;


}
}
