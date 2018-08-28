#include "ServiceResults.h"
#include <grid-files/common/Exception.h>


namespace SmartMet
{
namespace DataServer
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

      case Result::FILE_NOT_FOUND:
        return std::string("File not found!");

      case Result::MESSAGE_NOT_FOUND:
        return std::string("Message not found!");

      case Result::UNKNOW_COORDINATE_TYPE:
        return std::string("Unknown coordinate type!");

      case Result::INVALID_NUMBER_OF_COORDINATES:
        return std::string("Invalid number of coordinates!");
    }

    return std::string("Unknown result code : ") + std::to_string((int)resultCode);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
