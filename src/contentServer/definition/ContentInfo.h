#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/GeneralFunctions.h>
#include <string>

namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Metadata record describing one grid message (field) in the Content Server.
 *
 *  Each ContentInfo instance corresponds to a single GRIB message or equivalent grid
 *  field stored in a registered file. It records the parameter identity, level, forecast
 *  time, geometry, and various classification identifiers used to locate and retrieve
 *  the field. */
// ====================================================================================

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

    char*              getForecastTime();
    void               setForecastTime(time_t ftime);
    void               setForecastTime(const char *ftime);
    void               setForecastTime(const std::string& ftime);

    char*              getFmiParameterName();
    void               setFmiParameterName(const char *name);
    void               setFmiParameterName(const std::string& name);

    void               setCsv(const char *csv);
    void               setCsv(const std::string& csv);
    ContentInfo*       duplicate();
    void               print(std::ostream& stream,uint level,uint optionFlags);

    T::FileId          mFileId;                //!< Identifier of the file that contains this content record.
    T::MessageIndex    mMessageIndex;          //!< Zero-based index of the GRIB message within its file.
    T::FilePosition    mFilePosition;          //!< Byte offset of the message within the file.
    uint               mMessageSize;           //!< Size of the message in bytes.
    T::ProducerId      mProducerId;            //!< Identifier of the data producer.
    T::GenerationId    mGenerationId;          //!< Identifier of the model generation (run) this field belongs to.
    T::FmiParamId      mFmiParameterId;        //!< FMI numeric parameter identifier.
    uchar              mFileType;              //!< Format of the containing file (e.g. GRIB1, GRIB2, NetCDF).
    T::ParamLevelId    mFmiParameterLevelId;   //!< FMI level type identifier for the vertical level.
    T::ParamLevel      mParameterLevel;        //!< Numeric vertical level value.
    time_t             mForecastTimeUTC;       //!< Forecast valid time as a UTC Unix timestamp.
    time_t             mModificationTime;      //!< Timestamp of the last modification of this record.
    time_t             mDeletionTime;          //!< Timestamp after which the record may be removed (0 = never).
    T::ForecastType    mForecastType;          //!< Forecast type code (e.g. deterministic, ensemble member).
    T::ForecastNumber  mForecastNumber;        //!< Ensemble member number or perturbation index.
    T::GeometryId      mGeometryId;            //!< Grid geometry identifier.
    ushort             mFlags;                 //!< Bitmask of status and control flags (see ContentInfo::Flags).
    T::StorageId       mStorageId;             //!< Storage partition identifier used to group records.
    T::SourceId        mSourceId;              //!< Source system identifier from which this content was ingested.
    int                mAggregationId;         //!< Aggregation method identifier (0 = none).
    int                mAggregationPeriod;     //!< Length of the aggregation period in minutes.
    int                mProcessingTypeId;      //!< Post-processing type identifier applied to the field.
    float              mProcessingTypeValue1;  //!< First auxiliary value associated with the processing type.
    float              mProcessingTypeValue2;  //!< Second auxiliary value associated with the processing type.

  protected:

    uint               mFmiParameterName;  //!< Hashed FMI parameter name string (set via setFmiParameterName).
    uint               mForecastTime;      //!< Compact encoded forecast time (set via setForecastTime).

  public:

    /*! \brief Sorting-key constants for multi-field comparisons used by ContentInfoList. */
    class ComparisonMethod
    {
      public:
        static const uint none                                            = 0;  //!< No comparison.
        static const uint file_message                                    = 1;  //!< Sort by file id, then message index.
        static const uint fmiId_producer_generation_level_time            = 2;  //!< Sort by producer, generation, FMI parameter id, level, time.
        static const uint fmiName_producer_generation_level_time          = 3;  //!< Sort by producer, generation, FMI parameter name, level, time.
        static const uint starttime_file_message                          = 4;  //!< Sort by forecast time, file id, message index.
        static const uint fmiName_starttime_level_file_message            = 5;  //!< Sort by FMI parameter name, time, level, file, message.
        static const uint fmiId_fmiLevelId_level_starttime_file_message   = 6;  //!< Sort by FMI parameter id, level type, level, forecast type/number, time, geometry.
        static const uint starttime_fmiId_fmiLevelId_level_file_message   = 7;  //!< Sort by time, FMI parameter id, level type, level.
        static const uint starttime_fmiName_fmiLevelId_level_file_message = 8;  //!< Sort by time, FMI parameter name, level type, level.
        static const uint generationId_starttime_file_message             = 9;  //!< Sort by generation id, forecast time, file, message.
        static const uint fmiName_fmiLevelId_level_starttime_file_message = 10; //!< Sort by FMI parameter name, level type, level, forecast type/number, geometry, time.
        static const uint starttime_generationId_file_message             = 11; //!< Sort by time, generation id, forecast type/number, file, message.
        static const uint producer_file_message                           = 12; //!< Sort by producer id, file id, message index.
    };

    /*! \brief Bitmask flags controlling content record visibility and caching behaviour. */
    class Flags
    {
      public:
        static const uint UnusedFlag              = 0x0001; //!< Reserved for future use.
        static const uint PreloadRequired         = 0x0004; //!< Content must be preloaded into memory before access.
        static const uint DeletedContent          = 0x1000; //!< Record is logically deleted and should be ignored.
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


typedef ContentInfo* ContentInfoPtr;  //!< Non-owning pointer to a ContentInfo record.


}
}
