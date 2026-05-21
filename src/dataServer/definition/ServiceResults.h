#pragma once

#include <string>

namespace SmartMet
{
namespace DataServer
{

// ====================================================================================
/*! \brief Return/error codes for DataServer service methods.
 *
 *  Every DataServer::ServiceInterface method returns one of these values.
 *  Non-negative values indicate success; negative values indicate errors. */
// ====================================================================================

enum Result
{
  OK                                  =  0,   //!< Operation completed successfully.
  NOT_IMPLEMENTED                     = -1,   //!< Method is not implemented by this backend.
  DATA_NOT_FOUND                      = -2,   //!< Requested data was not found.
  UNEXPECTED_EXCEPTION                = -3,   //!< An unexpected C++ exception was caught.
  INVALID_SESSION                     = -10,  //!< The supplied session identifier is invalid.
  SERVICE_DISABLED                    = -20,  //!< The service has been administratively disabled.
  FILE_NOT_FOUND                      = -2000, //!< The grid file identified by fileId was not found.
  MESSAGE_NOT_FOUND                   = -2010, //!< The message index does not exist in the file.
  UNKNOW_COORDINATE_TYPE              = -2020, //!< The coordinate type is not recognised.
  INVALID_NUMBER_OF_COORDINATES       = -2021, //!< The coordinate list has an unexpected size.
  DATA_SETS_NOT_COMPATIBLE            = -2022, //!< Two grid messages have incompatible geometry.
  INVALID_GEOMETRY_ID                 = -2023, //!< The geometry identifier is invalid.
  GEOMETRY_NOT_FOUND                  = -2024, //!< The geometry was not found in the registry.
  INVALID_DIMENSIONS                  = -2025, //!< Grid dimensions are zero or otherwise invalid.
  UNSUPPORTED_INTERPOLATION_METHOD    = -2030  //!< The requested interpolation method is not supported.
};


std::string getResultString(int resultCode);

}
}
