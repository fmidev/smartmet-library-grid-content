#pragma once

#include <grid-files/grid/Typedefs.h>
#include <unordered_map>

namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Defines a bidirectional unit conversion rule between two physical units.
 *
 *  Holds the names of a forward and a reverse Lua or built-in conversion function
 *  (e.g. K → °C and °C → K) that are applied when a parameter mapping specifies
 *  a unit transformation. */
// ====================================================================================

class UnitConversion
{
  public:
                   UnitConversion();
    virtual        ~UnitConversion();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mSourceUnit;                 //!< Name of the source unit (e.g. "K").
    std::string    mTargetUnit;                 //!< Name of the target unit (e.g. "C").
    std::string    mConversionFunction;         //!< Function name for the forward (source → target) conversion.
    std::string    mReverseConversionFunction;  //!< Function name for the reverse (target → source) conversion.
};


typedef std::vector<UnitConversion> UnitConversion_vec;  //!< Ordered list of UnitConversion rules.


}  // namespace QueryServer
}  // namespace SmartMet
