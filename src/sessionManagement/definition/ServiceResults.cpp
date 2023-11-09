#include "ServiceResults.h"
#include <macgyver/Exception.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace SessionManagement
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

      case INVALID_SESSION:
        return std::string("Invalid session!");

      case SESSION_ATTRIBUTE_NOT_FOUND:
        return std::string("Session attribute not found!");

      case Result::SERVICE_DISABLED:
        return std::string("Service disabled!");

      case Result::UNEXPECTED_EXCEPTION:
        return std::string("Unexpected exception!");
    }

    return std::string("Unknown result code : ") + Fmi::to_string(resultCode);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
