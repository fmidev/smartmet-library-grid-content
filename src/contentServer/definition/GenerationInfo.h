#pragma once

#include <string>



namespace SmartMet
{
namespace T
{


enum GenerationStatus
{
  STATUS_DISABLED    = 0,   // Should not be accessed
  STATUS_READY       = 1,   // The generation process is ready.
  STATUS_GENERATING  = 2    // Can be accessed, but the generation process is still active
};



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

     void             operator=(GenerationInfo& generationInfo);
     GenerationInfo*  duplicate();
     void             print(std::ostream& stream,uint level,uint optionFlags);

     uint             mGenerationId;
     uint             mGenerationType;
     uint             mProducerId;
     std::string      mName;
     std::string      mDescription;
     GenerationStatus mStatus;
     uint             mFlags;
};



}
}
