#pragma once

#include <grid-files/common/Coordinate.h>
#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace Functions
{

const double degreesToRadiansMultiplier = 0.0174532925199;
const double radiansToDegreesMultiplier = 57.2957795131;


// ====================================================================================
/*! \brief Abstract base class for all grid mathematical and meteorological functions. */
// ====================================================================================

class Function
{
  public:
                      Function();
                      Function(const Function& function);
    virtual           ~Function();
    Function& operator=(const Function&) = default;

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 4: Takes two data vectors and angle vector, returns one vector
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    /*! \brief Return 1 if the comparison of the values holds, 0 if it does not, and missing if either value is missing. */

    template <class Compare>
    static float      compareValues(float val1,float val2,Compare compare)
    {
      if (val1 == ParamValueMissing  ||  val2 == ParamValueMissing)
        return ParamValueMissing;

      if (compare(val1,val2))
        return 1.0;

      return 0.0;
    }

    /*! \brief Compare the grid values (as floats) with the external constant (one grid) or with the values of the
               other grid (two grids): 1 if the comparison holds, 0 if it does not, and missing if a value is missing. */

    template <typename T,class Compare>
    static void       compareGridValues(uint columns,uint rows,const std::vector<std::vector<T>>& inParameters,const std::vector<double>& extParameters,Compare compare,std::vector<T>& outParameters)
    {
      uint sz = columns*rows;
      outParameters.reserve(sz);

      if (inParameters.size() == 1  &&  extParameters.size() == 1)
      {
        float val2 = extParameters[0];

        for (uint s=0; s<sz; s++)
        {
          float val1 = (s < inParameters[0].size() ? inParameters[0][s] : ParamValueMissing);
          outParameters.emplace_back(compareValues(val1,val2,compare));
        }
      }

      if (inParameters.size() == 2)
      {
        for (uint s=0; s<sz; s++)
        {
          float val1 = (s < inParameters[0].size() ? inParameters[0][s] : ParamValueMissing);
          float val2 = (s < inParameters[1].size() ? inParameters[1][s] : ParamValueMissing);
          outParameters.emplace_back(compareValues(val1,val2,compare));
        }
      }
    }
};


using FunctionPtr = Function*;          //!< Non-owning pointer to a Function.
using FunctionPtr_vec = std::vector<FunctionPtr>;  //!< Ordered list of non-owning Function pointers.


}  // namespace Functions
}  // namespace SmartMet
