#pragma once

#include <grid-files/grid/Typedefs.h>
#include <unordered_map>

namespace SmartMet
{
namespace QueryServer
{


class Alias
{
  public:
                   Alias();
                   //Alias(const Alias& alias);
    virtual        ~Alias();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mName;   // Alias name (must be unique)
    std::string    mAliasString;
};


typedef std::unordered_map<std::string,Alias> Alias_map;
typedef std::vector<Alias> Alias_vec;


}  // namespace QueryServer
}  // namespace SmartMet
