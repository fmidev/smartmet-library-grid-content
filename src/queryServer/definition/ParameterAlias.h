#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace QueryServer
{


class ParameterAlias
{
  public:
                   ParameterAlias();
                   ParameterAlias(const ParameterAlias& alias);
    virtual        ~ParameterAlias();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mName;   // Alias name (must be unique)
    std::string    mTitle;
    std::string    mParameterString;
};


typedef std::map<std::string,ParameterAlias> ParameterAlias_vec;


}  // namespace QueryServer
}  // namespace SmartMet
