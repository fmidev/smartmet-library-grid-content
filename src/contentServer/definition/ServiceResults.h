#pragma once

#include <string>


namespace SmartMet
{
namespace ContentServer
{

// ====================================================================================
/*! \brief Return-code constants for all Content Server service operations.
 *
 *  Every public method of ServiceInterface returns one of these values. Zero (OK)
 *  indicates success; all negative values indicate specific error conditions. The
 *  helper function getResultString() maps a code to a human-readable string. */
// ====================================================================================

enum Result
{
  OK                                          =  0,    //!< Operation completed successfully.
  NOT_IMPLEMENTED                             = -1,    //!< The called method is not implemented by this backend.
  DATA_NOT_FOUND                              = -2,    //!< The requested data does not exist.
  UNEXPECTED_EXCEPTION                        = -3,    //!< An unexpected C++ exception was caught.
  UPDATE_IN_PROGRESS                          = -4,    //!< A content update is currently in progress; retry later.
  NO_PERMANENT_STORAGE_DEFINED                = -5,    //!< No persistent storage backend has been configured.
  MISSING_PARAMETER                           = -6,    //!< A required input parameter was not supplied.
  PERMANENT_STORAGE_ERROR                     = -7,    //!< An error occurred while accessing permanent storage.
  NO_CONNECTION_TO_PERMANENT_STORAGE          = -8,    //!< Cannot connect to the persistent storage backend.
  INVALID_SESSION                             = -10,   //!< The supplied session id is invalid or expired.
  UNKNOWN_METHOD                              = -11,   //!< The requested service method is not recognised.
  SERVICE_DISABLED                            = -20,   //!< The service has been administratively disabled.
  SERVER_ID_ALREADY_REGISTERED                = -1000, //!< A server with the same id is already registered.
  SERVER_NAME_ALREADY_REGISTERED              = -1001, //!< A server with the same name is already registered.
  SERVER_IOR_ALREADY_REGISTERED               = -1002, //!< A server with the same CORBA IOR is already registered.
  UNKNOWN_SERVER_ID                           = -1003, //!< The supplied server id does not exist.
  INVALID_SERVER_ID                           = -1004, //!< The supplied server id is syntactically invalid.
  PRODUCER_ID_ALREADY_REGISTERED              = -1020, //!< A producer with the same id is already registered.
  PRODUCER_NAME_ALREADY_REGISTERED            = -1021, //!< A producer with the same name is already registered.
  UNKNOWN_PRODUCER_ID                         = -1022, //!< The supplied producer id does not exist.
  UNKNOWN_PRODUCER_NAME                       = -1023, //!< The supplied producer name does not exist.
  PRODUCER_AND_GENERATION_DO_NOT_MATCH        = -1024, //!< The generation does not belong to the given producer.
  PRODUCER_AND_FILE_DO_NOT_MATCH              = -1025, //!< The file does not belong to the given producer.
  FILE_NAME_ALREADY_REGISTERED                = -1040, //!< A file with the same name is already registered.
  UNKNOWN_FILE_ID                             = -1041, //!< The supplied file id does not exist.
  UNKNOWN_FILE_NAME                           = -1042, //!< The supplied file name does not exist.
  UNKNOWN_CONTENT                             = -1050, //!< The requested content record does not exist.
  GENERATION_NAME_ALREADY_REGISTERED          = -1060, //!< A generation with the same name is already registered.
  UNKNOWN_GENERATION_ID                       = -1061, //!< The supplied generation id does not exist.
  UNKNOWN_GENERATION_NAME                     = -1062, //!< The supplied generation name does not exist.
  GENERATION_AND_FILE_DO_NOT_MATCH            = -1063, //!< The file does not belong to the given generation.
  UNKNOWN_GEOMETRY                            = -1070, //!< The specified geometry does not exist.
  GEOMETRY_ALREADY_REGISTERED                 = -1071, //!< The geometry is already registered for this generation.
  UNKNOWN_PARAMETER_NAME                      = -1080, //!< The supplied parameter name is not recognised.
  UNKNOWN_PARAMETER_KEY_TYPE                  = -1081, //!< The supplied parameter key type is not recognised.
  CONTENT_ALREADY_EXISTS                      = -1100, //!< The content record already exists.
  CONTENT_ADDITION_FAILED                     = -1101  //!< The content record could not be added.
};


std::string getResultString(int resultCode);

}
}
