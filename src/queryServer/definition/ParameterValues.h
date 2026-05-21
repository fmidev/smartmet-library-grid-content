#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/grid/GridValueList.h>



namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Holds the decoded values returned for one parameter at one forecast time.
 *
 *  Carries the full provenance (producer, generation, geometry, file, message)
 *  alongside the actual value payload which can be a grid value list, a flat value
 *  vector, or raw byte data depending on the query type. */
// ====================================================================================

class ParameterValues
{
  public:
                            ParameterValues();
                            //ParameterValues(const ParameterValues& parameterValues);
    virtual                 ~ParameterValues();

    void                    clear();
    void                    print(std::ostream& stream,uint level,uint optionFlags);

    //std::string             mForecastTime;
    time_t                  mForecastTimeUTC;    //!< Forecast valid time as UTC epoch seconds.
    T::ProducerId           mProducerId;         //!< Identifier of the producer that generated this data.
    T::GenerationId         mGenerationId;       //!< Identifier of the model run (generation).
    UInt64                  mGenerationFlags;    //!< Flags describing properties of the generation.
    T::GeometryId           mGeometryId;         //!< Identifier of the grid geometry used.
    T::FileId               mFileId[4];          //!< File identifiers for up to four contributing messages.
    T::MessageIndex         mMessageIndex[4];    //!< Message indices within the corresponding files.
    std::string             mAnalysisTime;       //!< Analysis (reference) time string for the model run.
    time_t                  mModificationTime;   //!< Epoch time when this value record was last updated.

    T::ParamKeyType         mParameterKeyType;   //!< Key encoding used in mParameterKey.
    T::ParamId              mParameterKey;       //!< Concrete parameter key within the key encoding.
    T::ParamLevelId         mParameterLevelId;   //!< Level type identifier for this record.
    T::ParamLevel           mParameterLevel;     //!< Numeric level value for this record.
    T::ForecastType         mForecastType;       //!< Forecast type (deterministic, ensemble member, …).
    T::ForecastNumber       mForecastNumber;     //!< Ensemble member number, or 0 for deterministic.
    uint                    mFlags;              //!< Bitmask from Flags classifying this value record.
    T::GridValueList        mValueList;          //!< Coordinate-tagged grid values (used for point/polygon queries).
    T::ParamValue_vec       mValueVector;        //!< Flat array of grid values (used for full-grid output).
    T::ByteData_vec         mValueData;          //!< Raw byte payloads (used for grid file output).


    // ====================================================================================
    /*! \brief Bit-flag constants for mFlags. */
    // ====================================================================================
    class Flags
    {
      public:
        static const uint  AdditionalValue                       = 1 << 0;  //!< Value inserted to fill a missing time step.
        static const uint  AggregationValue                      = 1 << 1;  //!< Value inserted for aggregation purposes.
        static const uint  DataAvailable                         = 1 << 2;  //!< Actual data was found and loaded.
        static const uint  DataAvailableByTimeInterpolation      = 1 << 3;  //!< Value obtained via time interpolation.
        static const uint  DataAvailableByLevelInterpolation     = 1 << 4;  //!< Value obtained via level interpolation.
        static const uint  InternalAggregationValue              = 1 << 5;  //!< Value used internally during aggregation.
    };

};


typedef std::shared_ptr<ParameterValues> ParameterValues_sptr;             //!< Shared pointer to a ParameterValues record.
typedef std::vector<ParameterValues> ParameterValues_vec;                  //!< Owned vector of ParameterValues records.
typedef std::vector<ParameterValues_sptr> ParameterValues_sptr_vec;        //!< Vector of shared pointers to ParameterValues records.


}  // namespace QueryServer
}  // namespace SmartMet

