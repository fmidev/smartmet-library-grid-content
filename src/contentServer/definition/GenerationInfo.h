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
                     GenerationInfo(GenerationInfo& generationInfo);
                     GenerationInfo(const char *csv);
    virtual          ~GenerationInfo();

    std::string      getCsv();
    std::string      getCsvHeader();
    void             setCsv(const char *csv);
    void             setCsv(std::string csv);

    GenerationInfo&  operator=(GenerationInfo& generationInfo);
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

    class Status
    {
      public:
        const static uchar Disabled    = 0;   // Should not be accessed
        const static uchar Ready       = 1;   // The generation process is ready.
        const static uchar Running     = 2;   // Can be accessed, but the generation process is still active
    };
};



}
}
