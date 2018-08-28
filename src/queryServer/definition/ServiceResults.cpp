#include "ServiceResults.h"
#include <grid-files/common/Exception.h>


namespace SmartMet
{
namespace QueryServer
{


std::string getResultString(int resultCode)
{
  try
  {
    switch ((Result)resultCode)
    {
      case Result::OK:
        return std::string("Service call was successful!");

      case Result::NOT_IMPLEMENTED:
        return std::string("Service not implemented!");

      case Result::DATA_NOT_FOUND:
        return std::string("Requested data not found!");

      case Result::UNEXPECTED_EXCEPTION:
        return std::string("Unexpected exception!");

      case Result::NO_PRODUCERS_FOUND:
        return std::string("No producers found!");
    }

    return std::string("Unknown result code : ") + std::to_string((int)resultCode);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
