#include "ServiceResults.h"
#include <grid-files/common/Exception.h>
#include <macgyver/StringConversion.h>


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

      case Result::DATA_SETS_NOT_COMPATIBLE:
        return std::string("Data sets are not compatible!");

      case Result::INVALID_GEOMETRY_ID:
        return std::string("Invalid geometry id!");

      case Result::GEOMETRY_NOT_FOUND:
        return std::string("Geometry not found!");

      case Result::INVALID_DIMENSIONS:
        return std::string("Invalid dimensions!");

      case Result::UNSUPPORTED_INTERPOLATION_METHOD:
        return std::string("Unsupported or unknown interpolation method!");
    }

    return std::string("Unknown result code : ") + Fmi::to_string(resultCode);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
