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
                       ContentInfo(ContentInfo& contentInfo);
                       ContentInfo(const char *csv);
    virtual            ~ContentInfo();

    ContentInfo&       operator=(ContentInfo& contentInfo);

    bool               hasKey(T::ParamKeyType parameterKeyType,std::string parameterKey);
    std::string        getCsv();
    std::string        getCsvHeader();
    void               setCsv(const char *csv);
    void               setCsv(std::string csv);
    int                compare(uint comparisonMethod,ContentInfo *contentInfo);
    ContentInfo*       duplicate();
    void               print(std::ostream& stream,uint level,uint optionFlags);

    uint               mFileId;
    uchar              mFileType;
    uint               mMessageIndex;
    uint               mProducerId;
    uint               mGenerationId;
    uint               mGroupFlags;
    std::string        mForecastTime;
    T::ParamId         mFmiParameterId;
    std::string        mFmiParameterName;
    T::ParamId         mGribParameterId;
    T::ParamId         mCdmParameterId;
    std::string        mCdmParameterName;
    T::ParamId         mNewbaseParameterId;
    std::string        mNewbaseParameterName;
    T::ParamLevelId    mFmiParameterLevelId;
    T::ParamLevelId    mGrib1ParameterLevelId;
    T::ParamLevelId    mGrib2ParameterLevelId;
    T::ParamLevel      mParameterLevel;
    std::string        mFmiParameterUnits;
    std::string        mGribParameterUnits;
    T::ForecastType    mForecastType;
    T::ForecastNumber  mForecastNumber;
    unsigned long long mServerFlags;
    uint               mFlags;
    uint               mSourceId;
    T::GeometryId      mGeometryId;
    std::string        mModificationTime;


    class ComparisonMethod
    {
      public:
        static const uint none                                            = 0;  // No comparison
        static const uint file_message                                    = 1;  // Comparison order: fileId,messageIndex
        static const uint fmiId_level_starttime_file_message              = 2;  // Comparison order: fmiId,level,startTime,fileId,messageIndex
        static const uint fmiName_level_starttime_file_message            = 3;  // Comparison order: fmiName,level,startTime,fileId,messageIndex
        static const uint gribId_level_starttime_file_message             = 4;  // Comparison order: gribId,level,startTime,fileId,messageIndex
        static const uint newbaseId_level_starttime_file_message          = 5;  // Comparison order: newbaseId,level,startTime,fileId,messageIndex
        static const uint newbaseName_level_starttime_file_message        = 6;  // Comparison order: newbaseName,level,startTime,fileId,messageIndex
        static const uint cdmId_level_starttime_file_message              = 7;  // Comparison order: cdmId,level,startTime,fileId,messageIndex
        static const uint cdmName_level_starttime_file_message            = 8;  // Comparison order: cdmName,level,startTime,fileId,messageIndex
        static const uint starttime_file_message                          = 9;  // Comparison order: startTime,fileId,messageIndex
        static const uint fmiName_starttime_level_file_message            = 10; // Comparison order: fmiName,level,startTime,fileId,messageIndex
        static const uint fmiId_fmiLevelId_level_starttime_file_message   = 11;  // Comparison order: fmiId,fmiLevelId,level,startTime,fileId,messageIndex
        static const uint starttime_fmiId_fmiLevelId_level_file_message   = 12;
        static const uint starttime_fmiName_fmiLevelId_level_file_message = 13;
        static const uint generationId_starttime_file_message             = 14;
        static const uint fmiName_fmiLevelId_level_starttime_file_message = 15;
    };

    class Flags
    {
      public:
        static const uint VirtualContent          = 0x00000002;
        static const uint PreloadRequired         = 0x00000004;
        static const uint DeletedContent          = 0x80000000;
    };
};


typedef ContentInfo* ContentInfoPtr;


}
}
