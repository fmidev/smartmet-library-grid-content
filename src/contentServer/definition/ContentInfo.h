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

    std::string        getCsv();
    std::string        getCsvHeader();
    ulonglong          getRequestCounterKey();

    char*              getForecastTime();
    void               setForecastTime(time_t ftime);
    void               setForecastTime(const char *ftime);
    void               setForecastTime(const std::string& ftime);

    char*              getFmiParameterName();
    void               setFmiParameterName(const char *name);
    void               setFmiParameterName(const std::string& name);

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
    ushort             mSourceId;

  protected:

    uint              mFmiParameterName;
    uint              mForecastTime;

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
        static const uint VirtualContent          = 0x0002;
        static const uint PreloadRequired         = 0x0004;
        static const uint DeletedContent          = 0x1000;
    };
};


typedef ContentInfo* ContentInfoPtr;


}
}
