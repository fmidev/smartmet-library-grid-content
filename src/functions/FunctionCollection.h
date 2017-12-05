#pragma once

#include "Function.h"
#include <map>


namespace SmartMet
{
namespace Functions
{

typedef std::map<std::string,FunctionPtr> FunctionList;


class FunctionCollection
{
  public:
                        FunctionCollection();
    virtual             ~FunctionCollection();

    virtual void        addFunction(const char *functionName,FunctionPtr function);
    virtual void        addFunction(std::string& functionName,FunctionPtr function);
    virtual FunctionPtr getFunction(std::string& functionName);

    // Type 1: Takes one or several parameters, returns one value
    virtual float       executeFunctionCall1(std::string& functionName,std::vector<float>& parameters);
    virtual double      executeFunctionCall1(std::string& functionName,std::vector<double>& parameters);

    // Type 2: Takes and returns an vector
    virtual void        executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<float>& inOutParameters);
    virtual void        executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters);
    virtual void        executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<double>& inOutParameters);
    virtual void        executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters);

    // Type 3: Takes two vectors and returns one vector
    virtual void        executeFunctionCall3(std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters);
    virtual void        executeFunctionCall3(std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters);

    virtual void        print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    FunctionList        mFunctionList;
};



}  // namespace Functions
}  // namespace SmartMet
