#pragma once

#include "Function.h"
#include <unordered_map>


namespace SmartMet
{
namespace Functions
{

typedef std::unordered_map<std::string,FunctionPtr> FunctionList;


class FunctionCollection
{
  public:
                        FunctionCollection();
    virtual             ~FunctionCollection();

    virtual void        addFunction(const char *functionName,FunctionPtr function);
    virtual void        addFunction(const std::string& functionName,FunctionPtr function);
    virtual FunctionPtr getFunction(const std::string& functionName);

    // Type 1: Takes one or several parameters, returns one value
    virtual float       executeFunctionCall1(const std::string& functionName,std::vector<float>& parameters);
    virtual double      executeFunctionCall1(const std::string& functionName,std::vector<double>& parameters);

    // Type 4: Takes two data vectors and angle vector, returns one vector
    virtual void        executeFunctionCall4(const std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    virtual void        executeFunctionCall4(const std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void        executeFunctionCall9(const std::string& functionName,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void        executeFunctionCall9(const std::string& functionName,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual void        print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    FunctionList        mFunctionList;
};



}  // namespace Functions
}  // namespace SmartMet
