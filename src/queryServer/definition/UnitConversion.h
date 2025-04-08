#pragma once

#include <grid-files/grid/Typedefs.h>
#include <unordered_map>

namespace SmartMet
{
namespace QueryServer
{


class UnitConversion
{
  public:
                   UnitConversion();
    virtual        ~UnitConversion();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mSourceUnit;
    std::string    mTargetUnit;
    std::string    mConversionFunction;
    std::string    mReverseConversionFunction;
};


typedef std::vector<UnitConversion> UnitConversion_vec;


}  // namespace QueryServer
}  // namespace SmartMet
