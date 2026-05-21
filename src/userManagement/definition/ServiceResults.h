#pragma once

#include <string>

namespace SmartMet
{
namespace UserManagement
{

// ====================================================================================
/*! \brief Result codes returned by UserManagement service methods. */
// ====================================================================================

enum Result
{
  OK                                  =  0,    //!< Operation completed successfully.
  NOT_IMPLEMENTED                     = -1,    //!< Method not implemented by this backend.
  DATA_NOT_FOUND                      = -2,    //!< Requested user does not exist.
  UNEXPECTED_EXCEPTION                = -3,    //!< An unexpected C++ exception was caught.
  INVALID_SESSION                     = -10,   //!< The provided session identifier is invalid or expired.
  SERVICE_DISABLED                    = -20,   //!< Service has been administratively disabled.
  USER_ATTRIBUTE_NOT_FOUND            = -31,   //!< The requested user attribute was not found.
};


std::string getResultString(int resultCode);

}
}
