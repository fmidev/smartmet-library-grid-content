#pragma once

#include <string>


namespace SmartMet
{
namespace ContentServer
{

enum Result
{
  OK                                          =  0,
  NOT_IMPLEMENTED                             = -1,
  DATA_NOT_FOUND                              = -2,
  UNEXPECTED_EXCEPTION                        = -3,
  UPDATE_IN_PROGRESS                          = -4,
  NO_PERMANENT_STORAGE_DEFINED                = -5,
  MISSING_PARAMETER                           = -6,
  PERMANENT_STORAGE_ERROR                     = -7,
  NO_CONNECTION_TO_PERMANENT_STORAGE          = -8,
  INVALID_SESSION                             = -10,
  UNKNOWN_METHOD                              = -11,
  SERVER_ID_ALREADY_REGISTERED                = -1000,
  SERVER_NAME_ALREADY_REGISTERED              = -1001,
  SERVER_IOR_ALREADY_REGISTERED               = -1002,
  UNKNOWN_SERVER_ID                           = -1003,
  INVALID_SERVER_ID                           = -1004,
  PRODUCER_ID_ALREADY_REGISTERED              = -1020,
  PRODUCER_NAME_ALREADY_REGISTERED            = -1021,
  UNKNOWN_PRODUCER_ID                         = -1022,
  UNKNOWN_PRODUCER_NAME                       = -1023,
  PRODUCER_AND_GENERATION_DO_NOT_MATCH        = -1024,
  PRODUCER_AND_FILE_DO_NOT_MATCH              = -1025,
  FILE_NAME_ALREADY_REGISTERED                = -1040,
  UNKNOWN_FILE_ID                             = -1041,
  UNKNOWN_FILE_NAME                           = -1042,
  UNKNOWN_CONTENT                             = -1050,
  GENERATION_NAME_ALREADY_REGISTERED          = -1060,
  UNKNOWN_GENERATION_ID                       = -1061,
  UNKNOWN_GENERATION_NAME                     = -1062,
  GENERATION_AND_FILE_DO_NOT_MATCH            = -1063,
  UNKNOWN_PARAMETER_NAME                      = -1080,
  UNKNOWN_PARAMETER_KEY_TYPE                  = -1081,
  CONTENT_ALREADY_EXISTS                      = -1100,
  CONTENT_ADDITION_FAILED                     = -1101
};


std::string getResultString(int resultCode);

}
}
