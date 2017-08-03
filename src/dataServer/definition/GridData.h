#pragma once

#include "grid-files/common/Exception.h"
#include "grid-files/common/AttributeList.h"
#include "grid-files/grid/Typedefs.h"

namespace SmartMet
{
namespace T
{



class GridData
{
  public:
                        GridData();
                        GridData(GridData& gridData);
    virtual             ~GridData();

    virtual void        print(std::ostream& stream,uint level,uint optionFlags);

    uint                mServerId;
    uint                mGroupFlags;
    uint                mProducerId;
    uint                mGenerationId;
    uint                mFileId;
    T::FileType         mFileType;
    std::string         mFileName;
    uint                mMessageIndex;
    std::string         mStartTime;
    std::string         mEndTime;
    T::ParamId          mGribParameterId;
    T::ParamLevelId     mGrib1ParameterLevelId;
    T::ParamLevelId     mGrib2ParameterLevelId;
    std::string         mGribParameterName;
    std::string         mGribParameterDescription;
    std::string         mGribParameterUnits;
    T::ParamId          mFmiParameterId;
    T::ParamLevelId     mFmiParameterLevelId;
    std::string         mFmiParameterName;
    std::string         mFmiParameterDescription;
    std::string         mFmiParameterUnits;
    T::ParamId          mCdmParameterId;
    std::string         mCdmParameterName;
    T::ParamId          mNewbaseParameterId;
    std::string         mNewbaseParameterName;
    T::ParamLevel       mParameterLevel;
    uint                mProjection;
    uint                mColumns;
    uint                mRows;
    unsigned char       mTypeOfEnsembleForecast;
    unsigned char       mPerturbationNumber;
    T::AttributeList    mProjectionAttributes;
    T::ParamValue_vec   mValues;
};



}
}  // namespace SmartMet
