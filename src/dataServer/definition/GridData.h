#pragma once

#include <grid-files/common/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

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
    uchar         mFileType;
    std::string         mFileName;
    uint                mMessageIndex;
    std::string         mForecastTime;
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
    uint                mGeometryId;
    uint                mProjection;
    uint                mColumns;
    uint                mRows;
    short               mForecastType;
    short               mForecastNumber;
    T::AttributeList    mProjectionAttributes;
    T::ParamValue_vec   mValues;
};


typedef GridData* GridData_ptr;

}
}  // namespace SmartMet
