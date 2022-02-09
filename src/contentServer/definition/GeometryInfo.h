#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{


class GeometryInfo
{
  public:

                    GeometryInfo();
                    GeometryInfo(const GeometryInfo& geometryInfo);
                    GeometryInfo(const char *csv);
    virtual         ~GeometryInfo();

    GeometryInfo&   operator=(const GeometryInfo& geometryInfo);

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(const std::string& csv);

    int             compare(uint comparisonMethod,GeometryInfo *geometryInfo);
    GeometryInfo*   duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    uint            mProducerId;
    uint            mGenerationId;
    T::GeometryId   mGeometryId;
    T::ParamLevelId mLevelId;
    uint            mFlags;
    uint            mSourceId;
    time_t          mModificationTime;
    time_t          mDeletionTime;
    uchar           mStatus;

    class ComparisonMethod
    {
      public:
        static const uint none              = 0;    // No comparison
        static const uint generationId      = 1;    // Comparison according to the generation id
        static const uint producerId        = 2;    // Comparison according to the producer id
    };

    class Status
    {
      public:
        const static uchar Disabled    = 0;   // Should not be accessed
        const static uchar Ready       = 1;   // The generation process is ready.
        const static uchar Running     = 2;   // Can be accessed, but the generation process is still active
    };

    class Flags
    {
      public:
        static const uint DeletedGeometry   = 4;
    };
};


typedef GeometryInfo* GeometryInfoPtr;





}
}
