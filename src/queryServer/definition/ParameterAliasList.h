#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "ParameterAlias.h"


namespace SmartMet
{
namespace QueryServer
{


class ParameterAliasList
{
  public:
                        ParameterAliasList(std::string filename);
                        ParameterAliasList(const ParameterAliasList& aliasList);
    virtual             ~ParameterAliasList();

    virtual void        init();
    virtual void        checkUpdates();
    virtual bool        getAlias(std::string name,std::string& alias);
    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void        load();

    std::string         mFilename;
    time_t              mLastModified;
    ParameterAlias_map  mAliasList;
    ThreadLock          mThreadLock;
};



}  // namespace QueryServer
}  // namespace SmartMet
