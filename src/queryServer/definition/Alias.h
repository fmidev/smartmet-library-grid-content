#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace QueryServer
{


class Alias
{
  public:
                   Alias();
                   Alias(const Alias& alias);
    virtual        ~Alias();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mName;   // Alias name (must be unique)
    std::string    mAliasString;
};


typedef std::map<std::string,Alias> Alias_map;


}  // namespace QueryServer
}  // namespace SmartMet
