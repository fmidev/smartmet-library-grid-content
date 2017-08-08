#pragma once

#include <string>

#include "grid-files/grid/Typedefs.h"


namespace SmartMet
{
namespace T
{



class ContentInfo
{
  public:
    enum class ComparisonMethod
    {
      none                                            = 0,  // No comparison
      file_message                                    = 1,  // Comparison order: fileId,messageIndex
      fmiId_level_starttime_file_message              = 2,  // Comparison order: fmiId,level,startTime,fileId,messageIndex
      fmiName_level_starttime_file_message            = 3,  // Comparison order: fmiName,level,startTime,fileId,messageIndex
      gribId_level_starttime_file_message             = 4,  // Comparison order: gribId,level,startTime,fileId,messageIndex
      newbaseId_level_starttime_file_message          = 5,  // Comparison order: newbaseId,level,startTime,fileId,messageIndex
      newbaseName_level_starttime_file_message        = 6,  // Comparison order: newbaseName,level,startTime,fileId,messageIndex
      cdmId_level_starttime_file_message              = 7,  // Comparison order: cdmId,level,startTime,fileId,messageIndex
      cdmName_level_starttime_file_message            = 8,  // Comparison order: cdmName,level,startTime,fileId,messageIndex
      starttime_file_message                          = 9,  // Comparison order: startTime,fileId,messageIndex
      fmiName_starttime_level_file_message            = 10, // Comparison order: fmiName,level,startTime,fileId,messageIndex
      fmiId_fmiLevelId_level_starttime_file_message   = 11,  // Comparison order: fmiId,fmiLevelId,level,startTime,fileId,messageIndex
      starttime_fmiId_fmiLevelId_level_file_message   = 12
    };

  public:
                        ContentInfo();
                        ContentInfo(ContentInfo& contentInfo);
                        ContentInfo(const char *csv);
     virtual            ~ContentInfo();

     bool               hasKey(T::ParamKeyType parameterKeyType,std::string parameterKey);
     std::string        getCsv();
     std::string        getCsvHeader();
     void               setCsv(const char *csv);
     void               setCsv(std::string csv);

     void               operator=(ContentInfo& contentInfo);
     int                compare(ComparisonMethod comparisonMethod,ContentInfo *contentInfo);
     ContentInfo*       duplicate();
     void               print(std::ostream& stream,uint level,uint optionFlags);

     uint               mFileId;
     T::FileType        mFileType;
     uint               mMessageIndex;
     uint               mProducerId;
     uint               mGenerationId;
     uint               mGroupFlags;
     std::string        mStartTime;
     std::string        mEndTime;
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
     unsigned char      mTypeOfEnsembleForecast;
     unsigned char      mPerturbationNumber;
     unsigned long long mServerFlags;
     uint               mFlags;
     uint               mSourceId;
};


typedef ContentInfo* ContentInfoPtr;


}
}
