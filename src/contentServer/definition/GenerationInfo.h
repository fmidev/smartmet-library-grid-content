#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{


class GenerationInfo
{
  public:
                     GenerationInfo();
                     GenerationInfo(const GenerationInfo& generationInfo);
                     GenerationInfo(const char *csv);
    virtual          ~GenerationInfo();

    std::string      getCsv();
    std::string      getCsvHeader();
    void             setCsv(const char *csv);
    void             setCsv(const std::string& csv);

    int              compare(uint comparisonMethod,GenerationInfo *generationInfo);
    GenerationInfo&  operator=(const GenerationInfo& generationInfo);
    GenerationInfo*  duplicate();
    void             print(std::ostream& stream,uint level,uint optionFlags);

    uint             mGenerationId;
    uint             mGenerationType;
    uint             mProducerId;
    std::string      mName;
    std::string      mDescription;
    std::string      mAnalysisTime;
    uchar            mStatus;
    uint             mFlags;
    uint             mSourceId;
    time_t           mDeletionTime;

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
        static const uint DeletedGeneration  = 4;
    };


    class ComparisonMethod
    {
      public:
        static const uint none                         = 0;  // No comparison
        static const uint generationId                 = 1;  // Comparison order: generationId
        static const uint generationName               = 2;  // Comparison order: generationName
        static const uint analysisTime_generationId    = 3;  // Comparison order: analysisTime,generationId
        static const uint producerId                   = 4;  // Comparison order: producerId
        static const uint analysisTime                 = 5;  // Comparison order: analysisTime
    };

};


typedef GenerationInfo* GenerationInfoPtr;


}
}
