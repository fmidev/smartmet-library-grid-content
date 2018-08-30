#include "ServiceResults.h"
#include <grid-files/common/Exception.h>


namespace SmartMet
{
namespace ContentServer
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

      case Result::MISSING_PARAMETER:
        return std::string("Missing parameter!");

      case Result::UPDATE_IN_PROGRESS:
        return std::string("Update in progress!");

      case Result::NO_PERMANENT_STORAGE_DEFINED:
        return std::string("No permanent storage defined!");

      case Result::PERMANENT_STORAGE_ERROR:
        return std::string("Permanent storage error!");

      case Result::NO_CONNECTION_TO_PERMANENT_STORAGE:
        return std::string("No connection to the permanent storage!");

      case Result::INVALID_SESSION:
        return std::string("Invalid session!");

      case Result::UNKNOWN_METHOD:
        return std::string("Unknown method!");

      case Result::SERVER_ID_ALREADY_REGISTERED:
        return std::string("The server id was already registered!");

      case Result::SERVER_NAME_ALREADY_REGISTERED:
        return std::string("The server name was already registered!");

      case Result::SERVER_IOR_ALREADY_REGISTERED:
        return std::string("The server IOR was already registered!");

      case Result::UNKNOWN_SERVER_ID:
        return std::string("Unknown server id!");

      case Result::INVALID_SERVER_ID:
        return std::string("Invalid server id!");

      case Result::PRODUCER_ID_ALREADY_REGISTERED:
        return std::string("The producer id was already registered!");

      case Result::PRODUCER_NAME_ALREADY_REGISTERED:
        return std::string("The producer name was already registered!");

      case Result::UNKNOWN_PRODUCER_ID:
        return std::string("Unknown producer id!");

      case Result::UNKNOWN_PRODUCER_NAME:
        return std::string("Unknown producer name!");

      case Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH:
        return std::string("The producer and the generation do not match!");

      case Result::PRODUCER_AND_FILE_DO_NOT_MATCH:
        return std::string("The producer and the file do not match!");

      case Result::GENERATION_AND_FILE_DO_NOT_MATCH:
        return std::string("The generation and the file do not match!");

      case Result::FILE_NAME_ALREADY_REGISTERED:
        return std::string("The file name was already registered!");

      case Result::UNKNOWN_FILE_ID:
        return std::string("Unknown file id!");

      case Result::UNKNOWN_FILE_NAME:
        return std::string("Unknown filename!");

      case Result::UNKNOWN_CONTENT:
        return std::string("Unknown content!");

      case Result::GENERATION_NAME_ALREADY_REGISTERED:
        return std::string("The generation name was already registered!");

      case Result::UNKNOWN_GENERATION_ID:
        return std::string("Unknown generation id!");

      case Result::UNKNOWN_GENERATION_NAME:
        return std::string("Unknown generation name!");

      case Result::UNKNOWN_PARAMETER_NAME:
        return std::string("Unknown parameter name!");

      case Result::UNKNOWN_PARAMETER_KEY_TYPE:
        return std::string("Unknown parameter key type!");

      case Result::CONTENT_ALREADY_EXISTS:
        return std::string("Content already exists!");
    }

    return std::string("Unknown result code : ") + std::to_string(resultCode);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
