#pragma once

#include <grid-files/grid/Typedefs.h>
#include <unordered_map>

namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief A single parameter name alias mapping a short name to its expanded form.
 *
 *  The alias name must be unique within an AliasFile.  The alias string holds the
 *  canonical parameter name (or expression) that the short name expands to. */
// ====================================================================================

class Alias
{
  public:
                   Alias();
                   //Alias(const Alias& alias);
    virtual        ~Alias();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mName;        //!< Alias name (must be unique within the owning AliasFile).
    std::string    mAliasString; //!< Canonical parameter name or expression the alias expands to.
};


typedef std::unordered_map<std::string,Alias> Alias_map;  //!< Hash map from alias name to Alias record.
typedef std::vector<Alias> Alias_vec;                      //!< Ordered list of Alias records (supports duplicates).


}  // namespace QueryServer
}  // namespace SmartMet
