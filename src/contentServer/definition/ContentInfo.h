#pragma once

#include <grid-files/grid/Typedefs.h>
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

    bool               hasKey(T::ParamKeyType parameterKeyType,const std::string& parameterKey);
    std::string        getCsv();
    std::string        getCsvHeader();
    ulonglong          getRequestCounterKey();

    char*              getForecastTime();
    void               setForecastTime(const char *ftime);
    void               setForecastTime(const std::string& ftime);

    char*              getFmiParameterName();
    void               setFmiParameterName(const char *name);
    void               setFmiParameterName(const std::string& name);

    char*              getNewbaseParameterName();
    void               setNewbaseParameterName(const char *name);
    void               setNewbaseParameterName(const std::string& name);

    void               setCsv(const char *csv);
    void               setCsv(const std::string& csv);
    int                compare(uint comparisonMethod,ContentInfo *contentInfo);
    ContentInfo*       duplicate();
    void               print(std::ostream& stream,uint level,uint optionFlags);

    uint               mFileId;
    uint               mMessageIndex;
    ulonglong          mFilePosition;

    uint               mMessageSize;
    uint               mProducerId;

    uint               mGenerationId;
    T::FmiParamId      mFmiParameterId;

    T::NewbaseParamId  mNewbaseParameterId;
    T::GribParamId     mGribParameterId;

    uchar              mFileType;
    T::ParamLevelId    mFmiParameterLevelId;
    T::ParamLevelId    mGrib1ParameterLevelId;
    T::ParamLevelId    mGrib2ParameterLevelId;
    T::ParamLevel      mParameterLevel;

    time_t             mForecastTimeUTC;
    time_t             mModificationTime;
    time_t             mDeletionTime;

    T::ForecastType    mForecastType;
    T::ForecastNumber  mForecastNumber;
    T::GeometryId      mGeometryId;
    ushort             mFlags;
    ushort             mSourceId;

  protected:

    //char*              mFmiParameterName;
    //char*              mNewbaseParameterName;
    //char*              mForecastTime;

    uint              mFmiParameterName;
    uint              mNewbaseParameterName;
    uint              mForecastTime;

  public:

    class ComparisonMethod
    {
      public:
        static const uint none                                            = 0;  // No comparison
        static const uint file_message                                    = 1;
        static const uint fmiId_producer_generation_level_time            = 2;
        static const uint fmiName_producer_generation_level_time          = 3;
        static const uint gribId_producer_generation_level_time           = 4;
        static const uint newbaseId_producer_generation_level_time        = 5;
        static const uint newbaseName_producer_generation_level_time      = 6;
        static const uint starttime_file_message                          = 9;
        static const uint fmiName_starttime_level_file_message            = 10;
        static const uint fmiId_fmiLevelId_level_starttime_file_message   = 11;
        static const uint starttime_fmiId_fmiLevelId_level_file_message   = 12;
        static const uint starttime_fmiName_fmiLevelId_level_file_message = 13;
        static const uint generationId_starttime_file_message             = 14;
        static const uint fmiName_fmiLevelId_level_starttime_file_message = 15;
        static const uint starttime_generationId_file_message             = 16;
        static const uint producer_file_message                           = 17;
    };

    class Flags
    {
      public:
        static const uint UnusedFlag              = 0x0001;
        static const uint VirtualContent          = 0x0002;
        static const uint PreloadRequired         = 0x0004;
        static const uint DeletedContent          = 0x1000;
    };
};


typedef ContentInfo* ContentInfoPtr;


}
}
