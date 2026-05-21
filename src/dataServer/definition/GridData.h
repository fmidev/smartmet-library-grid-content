#pragma once

#include <macgyver/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

namespace SmartMet
{
namespace T
{



// ====================================================================================
/*! \brief Decoded grid data for a single message, including metadata and value array.
 *
 *  Carries the full identity of the grid message (file, producer, generation,
 *  parameter identifiers for GRIB/FMI/Newbase) together with the decoded
 *  floating-point value array in row-major order. */
// ====================================================================================

class GridData
{
  public:
                        GridData();
                        //GridData(GridData& gridData);
    virtual             ~GridData();

    virtual void        print(std::ostream& stream,uint level,uint optionFlags);

    uint                mServerId;                 //!< Identifier of the DataServer that supplied the data.
    T::ProducerId       mProducerId;               //!< Identifier of the data producer.
    T::GenerationId     mGenerationId;             //!< Identifier of the model generation (run).
    T::FileId           mFileId;                   //!< Numeric file identifier in the ContentServer.
    T::FileType         mFileType;                 //!< File format type (GRIB1, GRIB2, NetCDF, QueryData, …).
    std::string         mFileName;                 //!< Filesystem path of the source file.
    T::MessageIndex     mMessageIndex;             //!< Zero-based index of the message within the file.
    std::string         mForecastTime;             //!< ISO-8601 forecast valid time string.
    T::ParamId          mGribParameterId;          //!< GRIB parameter identifier.
    T::ParamLevelId     mGrib1ParameterLevelId;    //!< GRIB1 level type identifier.
    T::ParamLevelId     mGrib2ParameterLevelId;    //!< GRIB2 level type identifier.
    std::string         mGribParameterName;        //!< Human-readable GRIB parameter name.
    T::ParamId          mFmiParameterId;           //!< FMI internal parameter identifier.
    T::ParamLevelId     mFmiParameterLevelId;      //!< FMI internal level type identifier.
    std::string         mFmiParameterName;         //!< FMI internal parameter name.
    T::ParamId          mNewbaseParameterId;       //!< Newbase (QueryData) parameter identifier.
    std::string         mNewbaseParameterName;     //!< Newbase parameter name.
    T::ParamLevel       mParameterLevel;           //!< Numeric vertical level value.
    T::GeometryId       mGeometryId;               //!< Grid geometry identifier.
    bool                mReverseXDirection;        //!< True if the x-axis is stored in reverse order.
    bool                mReverseYDirection;        //!< True if the y-axis is stored in reverse order.
    uint                mProjection;               //!< Projection type identifier.
    uint                mColumns;                  //!< Number of grid columns.
    uint                mRows;                     //!< Number of grid rows.
    short               mForecastType;             //!< Forecast type code (deterministic, ensemble, …).
    short               mForecastNumber;           //!< Ensemble member number, or -1 for deterministic.
    T::AttributeList    mProjectionAttributes;     //!< Projection-specific attribute list.
    T::ParamValue_vec   mValues;                   //!< Decoded grid values in row-major order.
};


typedef GridData* GridData_ptr;  //!< Non-owning pointer to a GridData.

}
}  // namespace SmartMet
